#pragma once

#include <AMReX_GpuQualifiers.H>
#include <AMReX_REAL.H>
#include <cstring>
#include <type_traits>

// manually type-erased device-side functor with signature
// (amrex::Real, amrex::Real, amrex::Real, int, int, int) -> amrex::Real
/*
[HOST SIDE]
Lambda (Type: lambda#1) ---> [Strip Type] ---> DeviceFunction Payload
                                                ├─ m_storage: Raw bytes of captures
                                                └─ m_invoker: Address of invoker_impl<lambda#1>

============================== KERNEL LAUNCH ==============================

[HOST/DEVICE SIDE (where executed)]
ParallelFor Kernel Thread ──> DeviceFunction::operator()
                                   │
                                   ├──> Jumps to m_invoker
                                   │         │
                                   │         v
                                   └──> invoker_impl<lambda#1>(m_storage, ...)
                                             │
                                             ├──> Reinterprets m_storage as lambda#1&  <-- Resurrected!
                                             └──> Executes lambda#1::operator()
*/
class PortableFunction {
  public:
    using InvokerFn = amrex::Real (*)(const void*, amrex::Real, amrex::Real, amrex::Real, int, int, int);

    // Host-callable constructor
    template <typename F> PortableFunction(F f) {
        static_assert(
            std::is_invocable_r_v<amrex::Real, F, amrex::Real, amrex::Real, amrex::Real, int, int, int>,
            "Functor/Lambda signature must match: amrex::Real(amrex::Real, amrex::Real, amrex::Real, int, int, int)");
        static_assert(sizeof(F) <= BufferSize, "Captured state in functor/lambda exceeds DeviceFunction buffer size!");
        static_assert(std::is_trivially_copyable<F>::value,
                      "Functor/lambda must be trivially copyable for GPU transfer!");

        // Copy captured state into internal byte buffer
        std::memcpy(m_storage, &f, sizeof(F));

        // Use a static helper function template to generate a clean device function pointer
        m_invoker = &invoker_impl<F>;
    }

    PortableFunction() = default;

    // PURE DEVICE operator(): Called strictly inside amrex::ParallelFor kernels
    AMREX_GPU_DEVICE AMREX_FORCE_INLINE amrex::Real operator()(amrex::Real x, amrex::Real y, amrex::Real z, int i,
                                                               int j, int k) const noexcept {
        return m_invoker(m_storage, x, y, z, i, j, k);
    }

  private:
    // Static device invoker function
    template <typename F>
    AMREX_GPU_DEVICE static amrex::Real invoker_impl(const void* buf, amrex::Real x, amrex::Real y, amrex::Real z,
                                                     int i, int j, int k) noexcept {
        const F& functor = *reinterpret_cast<const F*>(buf);
        return functor(x, y, z, i, j, k);
    }

    static constexpr std::size_t BufferSize = 64; // size of the stored captured state
    alignas(8) char m_storage[BufferSize] = {0};  // stored captured state
    InvokerFn m_invoker = nullptr;                // pointer to the invoker_impl<F> code
};