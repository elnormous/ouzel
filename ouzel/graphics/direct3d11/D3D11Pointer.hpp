// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_D3D11POINTER_HPP
#define OUZEL_GRAPHICS_D3D11POINTER_HPP

namespace ouzel
{
    namespace graphics
    {
        namespace d3d11
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

                Pointer(const Pointer& o) = delete;
                Pointer& operator=(const Pointer& o) = delete;

                inline Pointer(Pointer&& o) noexcept : p(o.p)
                {
                    o.p = nil;
                }

                inline Pointer& operator=(Pointer&& o) noexcept
                {
                    if (p) p->Release();
                    p = o.p;
                    o.p = nil;
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
        } // namespace d3d11
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_GRAPHICS_D3D11POINTER_HPP
