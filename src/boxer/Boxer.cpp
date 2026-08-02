#include "Boxer.hpp"
#include "AMReX_AmrCore.H"

namespace boxer {
void show(const amrex::AmrCore& container) { std::cout << "boxer::show called" << std::endl; }
} // namespace boxer