#pragma once
#include "AMReX_AmrCore.H"

namespace boxer {
void show(const amrex::AmrCore& container, int ngrow = 0, bool blocking = true);
}