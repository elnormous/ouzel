// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

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
            virtual ~OGLRenderResource() {}

            OGLRenderResource(const OGLRenderResource&) = delete;
            OGLRenderResource& operator=(const OGLRenderResource&) = delete;

            OGLRenderResource(OGLRenderResource&&) = delete;
            OGLRenderResource& operator=(OGLRenderResource&&) = delete;

            virtual void reload() = 0;

        protected:
            OGLRenderDevice& renderDevice;
        };
    } // namespace graphics
} // namespace ouzel

#endif

#endif // OUZEL_GRAPHICS_OGLRENDERRESOURCE_HPP
