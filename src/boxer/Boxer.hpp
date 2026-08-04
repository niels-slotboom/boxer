#pragma once

namespace amrex {
class AmrMesh;
}

namespace boxer {

void show(const amrex::AmrMesh& container, int ngrow = 0, bool blocking = true);

} // namespace boxer