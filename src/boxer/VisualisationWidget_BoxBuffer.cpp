#include "AmrMeshWrapper.hpp"
#include "VisualisationWidget.hpp"

#include <cmath>
#include <iterator>

namespace boxer {

void VisualisationWidget::updateBoxData() {
    if (boxVBO == 0)
        return;

    std::vector<AmrMeshWrapper::BoxInstanceData> instances;

    // -----------------------------------------------------------------------
    // 1. Add Full Physical Domain Box (Level = -1) and adjust camera distance
    // -----------------------------------------------------------------------
    if (container.finestLevel() >= 0) {
        AmrMeshWrapper::BoxInstanceData domain = container.getDomain();

        // Calculate domain center
        float cx = 0.5f * (domain.lo[0] + domain.hi[0]);
        float cy = 0.5f * (domain.lo[1] + domain.hi[1]);
        float cz = 0.5f * (domain.lo[2] + domain.hi[2]);

        // Calculate domain size
        float dx = domain.hi[0] - domain.lo[0];
        float dy = domain.hi[1] - domain.lo[1];
        float dz = domain.hi[2] - domain.lo[2];

        float diagonal = std::sqrt(dx * dx + dy * dy + dz * dz);
        cameraDistance = 1.8f * diagonal;

        cameraTarget = QVector3D(cx, cy, cz);

        instances.push_back(domain);
    }

    // ----------------------------------------------------
    // 2. Add Active Level Box Arrays
    // ----------------------------------------------------
    int maxLevel = container.finestLevel();
    int endLevel = std::min(finestDisplayLevel, maxLevel);

    for (int lev = coarsestDisplayLevel; lev <= endLevel; ++lev) {
        auto boxesAtLevel = container.getBoxesAtLevel(lev, ngrow, showHalo);
        instances.reserve(instances.size() + boxesAtLevel.size());
        instances.insert(instances.end(), std::make_move_iterator(boxesAtLevel.begin()),
                         std::make_move_iterator(boxesAtLevel.end()));
    }

    instanceCount = static_cast<int>(instances.size());

    makeCurrent();
    glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(instances.size() * sizeof(AmrMeshWrapper::BoxInstanceData)),
                 instances.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    update();
}

} // namespace boxer