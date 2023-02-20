// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_WASAPIPOINTER_HPP
#define OUZEL_AUDIO_WASAPIPOINTER_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_WASAPI

namespace ouzel::audio::wasapi
{
    template <class T>
    class Pointer final
    {
    public:
        Pointer() noexcept = default;

        Pointer(T* a) noexcept : p{a} {}
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
            if (other.p) other.p->AddRef();
            if (p) p->Release();
            p = other.p;
        }

        Pointer(Pointer&& other) noexcept : p{other.p}
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

        [[nodiscard]] explicit operator bool() const noexcept
        {
            return p != nullptr;
        }

    private:
        T* p = nullptr;
    };
}
#endif

#endif // OUZEL_AUDIO_WASAPIPOINTER_HPP
