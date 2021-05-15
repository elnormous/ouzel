// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_OSLPOINTER_HPP
#define OUZEL_AUDIO_OSLPOINTER_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENSL

#include <SLES/OpenSLES.h>

namespace ouzel::audio::opensl
{
    template <class T>
    class Pointer final
    {
    public:
        Pointer() noexcept = default;

        Pointer(T a) noexcept : p{a} {}
        Pointer& operator=(T a) noexcept
        {
            if (p) (*p)->Destroy(p);
            p = a;
            return *this;
        }

        Pointer(const Pointer& other) = delete;
        Pointer& operator=(const Pointer& other) = delete;

        Pointer(Pointer&& other) noexcept : p{other.p}
        {
            other.p = nullptr;
        }

        Pointer& operator=(Pointer&& other) noexcept
        {
            if (&other == this) return *this;
            if (p) (*p)->Destroy(p);
            p = other.p;
            other.p = nullptr;
            return *this;
        }

        ~Pointer()
        {
            if (p) (*p)->Destroy(p);
        }

        auto operator->() const noexcept
        {
            return *p;
        }

        auto get() const noexcept
        {
            return p;
        }

        explicit operator bool() const noexcept
        {
            return p != nullptr;
        }

    private:
        T p = nullptr;
    };
}
#endif

#endif // OUZEL_AUDIO_OSLPOINTER_HPP
