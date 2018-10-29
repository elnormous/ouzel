// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

namespace ouzel
{
    namespace graphics
    {
        class RenderDevice;

        class RenderResource
        {
        public:
            explicit RenderResource(RenderDevice& initRenderDevice):
                renderDevice(initRenderDevice) {}
            virtual ~RenderResource() {}

            RenderResource(const RenderResource&) = delete;
            RenderResource& operator=(const RenderResource&) = delete;

            RenderResource(RenderResource&&) = delete;
            RenderResource& operator=(RenderResource&&) = delete;

        protected:
            RenderDevice& renderDevice;
        };
    } // namespace graphics
} // namespace ouzel
