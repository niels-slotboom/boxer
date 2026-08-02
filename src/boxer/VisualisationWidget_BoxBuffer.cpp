#include "AMReX_Box.H"
#include "AMReX_Geometry.H"
#include "AMReX_RealBox.H"
#include "VisualisationWidget.hpp"

namespace boxer {

void VisualisationWidget::updateBoxData() {
    if (boxVBO == 0)
        return;

    std::vector<BoxInstanceData> instances;

    // ----------------------------------------------------
    // 1. Add Full Physical Domain Box (Level = -1)
    // ----------------------------------------------------
    if (container.finestLevel() >= 0) {
        const amrex::Geometry& level0Geom = container.Geom(0);
        const amrex::RealBox& probDomain = level0Geom.ProbDomain();

        BoxInstanceData domainData;
        domainData.lo[0] = static_cast<float>(probDomain.lo(0));
        domainData.lo[1] = static_cast<float>(probDomain.lo(1));
        domainData.lo[2] = static_cast<float>(probDomain.lo(2));

        domainData.hi[0] = static_cast<float>(probDomain.hi(0));
        domainData.hi[1] = static_cast<float>(probDomain.hi(1));
        domainData.hi[2] = static_cast<float>(probDomain.hi(2));

        domainData.level = -1; // Flag for Domain Box in shader

        instances.push_back(domainData);
    }

    // ----------------------------------------------------
    // 2. Add Active Level Box Arrays
    // ----------------------------------------------------
    int maxLevel = container.finestLevel();
    for (int lev = coarsestDisplayLevel; lev <= finestDisplayLevel; ++lev) {
        if (lev < 0 || lev > maxLevel)
            continue;

        const amrex::BoxArray& ba = container.boxArray(lev);
        const amrex::Geometry& geom = container.Geom(lev);

        for (int i = 0; i < ba.size(); ++i) {
            const amrex::Box& bx = ba[i];
            amrex::RealBox realBox(bx, geom.CellSize(), geom.ProbLo());

            instances.emplace_back(realBox, lev);

            if (showHalo && ngrow > 0) {
                amrex::RealBox haloRealBox(amrex::grow(bx, ngrow), geom.CellSize(), geom.ProbLo());
                instances.emplace_back(haloRealBox, lev, true);
            }
        }
    }

    instanceCount = static_cast<int>(instances.size());

    makeCurrent();
    glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
    glBufferData(GL_ARRAY_BUFFER, instances.size() * sizeof(BoxInstanceData), instances.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // set camera to look at center of problem domain
    if (container.finestLevel() >= 0) {
        const amrex::Geometry& level0Geom = container.Geom(0);
        const amrex::RealBox& probDomain = level0Geom.ProbDomain();

        // Calculate domain center
        float cx = 0.5f * static_cast<float>(probDomain.lo(0) + probDomain.hi(0));
        float cy = 0.5f * static_cast<float>(probDomain.lo(1) + probDomain.hi(1));
        float cz = 0.5f * static_cast<float>(probDomain.lo(2) + probDomain.hi(2));

        // Calculate domain size
        float dx = static_cast<float>(-probDomain.lo(0) + probDomain.hi(0));
        float dy = static_cast<float>(-probDomain.lo(1) + probDomain.hi(1));
        float dz = static_cast<float>(-probDomain.lo(2) + probDomain.hi(2));

        float diagonal = std::sqrt(dx * dx + dy * dy + dz * dz);
        cameraDistance = 1.8 * diagonal;

        cameraTarget = QVector3D(cx, cy, cz);
    }

    updateViewMatrix();

    update();
}

} // namespace boxer