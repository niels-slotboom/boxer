#include "AmrMeshWrapper.hpp"

#include <AMReX_AmrMesh.H>
#include <AMReX_Geometry.H>
#include <AMReX_RealBox.H>

namespace boxer {
AmrMeshWrapper::BoxInstanceData::BoxInstanceData(const amrex::RealBox& rb, int lev, bool halo)
    : level(lev), isHalo(halo ? 1 : 0) {
    lo[0] = static_cast<float>(rb.lo(0));
    lo[1] = static_cast<float>(rb.lo(1));
    lo[2] = static_cast<float>(rb.lo(2));

    hi[0] = static_cast<float>(rb.hi(0));
    hi[1] = static_cast<float>(rb.hi(1));
    hi[2] = static_cast<float>(rb.hi(2));
}

int AmrMeshWrapper::finestLevel() const { return container.finestLevel(); }

AmrMeshWrapper::BoxInstanceData AmrMeshWrapper::getDomain() const {
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

    return domainData;
}

std::vector<AmrMeshWrapper::BoxInstanceData> AmrMeshWrapper::getBoxesAtLevel(int lev, int ngrow, bool addHalo) const {
    std::vector<AmrMeshWrapper::BoxInstanceData> instances;

    if (lev < 0 || lev > container.finestLevel())
        return instances;

    const amrex::BoxArray& ba = container.boxArray(lev);
    const amrex::Geometry& geom = container.Geom(lev);

    // Reserve space (allocating up to 2x if halos are generated)
    instances.reserve(addHalo && ngrow > 0 ? ba.size() * 2 : ba.size());

    for (int i = 0; i < ba.size(); ++i) {
        const amrex::Box& bx = ba[i];
        amrex::RealBox realBox(bx, geom.CellSize(), geom.ProbLo());

        instances.emplace_back(realBox, lev);

        if (addHalo && ngrow > 0) {
            amrex::RealBox haloRealBox(amrex::grow(bx, ngrow), geom.CellSize(), geom.ProbLo());
            instances.emplace_back(haloRealBox, lev, true);
        }
    }

    return instances;
}
} // namespace boxer