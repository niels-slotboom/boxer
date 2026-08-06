#pragma once

#include <Boxer/boxer_export.hpp>

namespace amrex {
class AmrMesh;
}

namespace boxer {

BOXER_EXPORT void show(const amrex::AmrMesh& container, int ngrow = 0, bool blocking = true);

} // namespace boxer