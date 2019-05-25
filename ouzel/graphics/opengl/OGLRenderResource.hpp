// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_GRAPHICS_OGLRENDERRESOURCE_HPP
#define OUZEL_GRAPHICS_OGLRENDERRESOURCE_HPP

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

namespace ouzel
{
    namespace graphics
    {
        class OGLRenderDevice;

        class OGLRenderResource
        {
        public:
            explicit OGLRenderResource(OGLRenderDevice& initRenderDevice):
                renderDevice(initRenderDevice)
            {
            }
            virtual ~OGLRenderResource() = default;

            OGLRenderResource(const OGLRenderResource&) = delete;
            OGLRenderResource& operator=(const OGLRenderResource&) = delete;

            OGLRenderResource(OGLRenderResource&&) = delete;
            OGLRenderResource& operator=(OGLRenderResource&&) = delete;

            inline bool isInvalid() const { return invalid; }
            inline void invalidate() { invalid = true; }

            inline void restore()
            {
                if (invalid) reload();
                invalid = false;
            }

        protected:
            virtual void reload() = 0;

            OGLRenderDevice& renderDevice;

        private:
            bool invalid = false;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLRENDERRESOURCE_HPP
