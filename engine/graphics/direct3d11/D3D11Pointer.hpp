// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_D3D11POINTER_HPP
#define OUZEL_GRAPHICS_D3D11POINTER_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

namespace ouzel::graphics::d3d11
{
    template <class T>
    class Pointer final
    {
    public:
        Pointer() noexcept = default;

        Pointer(T* a) noexcept: p{a} {}
        Pointer& operator=(T* a) noexcept
        {
            if (p) p->Release();
            p = a;
            return *this;
        }

        Pointer(const Pointer& other) noexcept: p{other.p}
        {
            if (p) p->AddRef();
        }

        Pointer& operator=(const Pointer& other) noexcept
        {
            if (&other == this) return *this;
            if (p) p->Release();
            p = other.p;
            if (p) p->AddRef();
        }

        Pointer(Pointer&& other) noexcept: p{other.p}
        {
            other.p = nullptr;
        }

        Pointer& operator=(Pointer&& other) noexcept
        {
            if (&other == this) return *this;
            if (p) p->Release();
            p = other.p;
            other.p = nullptr;
            return *this;
        }

        ~Pointer()
        {
            if (p) p->Release();
        }

        T* operator->() const noexcept
        {
            return p;
        }

        T* get() const noexcept
        {
            return p;
        }

        explicit operator bool() const noexcept
        {
            return p != nullptr;
        }

    private:
        T* p = nullptr;
    };
}
#endif

#endif // OUZEL_GRAPHICS_D3D11POINTER_HPP
