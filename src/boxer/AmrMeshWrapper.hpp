#pragma once
// !!! DO NOT ADD ANY AMReX/CUDA HEADERS HERE !!!

#include <vector>

namespace amrex { // forward declare to avoid propagating amrex headers
class AmrMesh;
class RealBox;
} // namespace amrex

namespace boxer {
class AmrMeshWrapper {
  public:
    struct BoxInstanceData {
        float lo[3]; // AMReX Box Low bounds  (x, y, z)
        float hi[3]; // AMReX Box High bounds (x, y, z)
        int level;   // Refinement level
        int isHalo;  // 0 = interior/normal, 1 = halo (32-bit for GPU attribute alignment)

        // Default constructor
        BoxInstanceData() : lo{0.0f, 0.0f, 0.0f}, hi{0.0f, 0.0f, 0.0f}, level(0), isHalo(0) {}

        // Declaration only - definition in .cpp where RealBox is complete
        BoxInstanceData(const amrex::RealBox& rb, int lev, bool halo = false);
    };

  public:
    AmrMeshWrapper() = delete;
    explicit AmrMeshWrapper(const amrex::AmrMesh& container) : container(container) {}

    int finestLevel() const;

    BoxInstanceData getDomain() const;

    std::vector<BoxInstanceData> getBoxesAtLevel(int lev, int ngrow = 0, bool addHalo = false) const;

  private:
    const amrex::AmrMesh& container;
};
} // namespace boxer