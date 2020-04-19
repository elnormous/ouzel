// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_DSPOINTER_HPP
#define OUZEL_AUDIO_DSPOINTER_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_DIRECTSOUND

namespace ouzel
{
    namespace audio
    {
        namespace directsound
        {
            template <class T>
            class Pointer final
            {
            public:
                Pointer() noexcept = default;

                inline Pointer(T* a) noexcept : p(a) {}
                inline Pointer& operator=(T* a) noexcept
                {
                    if (p) p->Release();
                    p = a;
                    return *this;
                }

                Pointer(const Pointer&) = delete;
                Pointer& operator=(const Pointer&) = delete;

                inline Pointer(Pointer&& other) noexcept : p(other.p)
                {
                    other.p = nullptr;
                }

                inline Pointer& operator=(Pointer&& other) noexcept
                {
                    if (this == &other) return *this;
                    if (p) p->Release();
                    p = other.p;
                    other.p = nullptr;
                    return *this;
                }

                ~Pointer()
                {
                    if (p) p->Release();
                }

                inline T* operator->() const
                {
                    return p;
                }

                inline T* get() const noexcept
                {
                    return p;
                }

                explicit inline operator bool() const noexcept
                {
                    return p != nullptr;
                }

            private:
                T* p = nullptr;
            };
        } // namespace audio
    } // namespace directsound
} // namespace ouzel

#endif

#endif // OUZEL_AUDIO_DSPOINTER_HPP
