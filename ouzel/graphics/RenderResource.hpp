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
            RenderResource(RenderDevice& initRenderDevice):
                renderDevice(initRenderDevice) {}
            virtual ~RenderResource() {}

            RenderResource(const RenderResource&) = delete;
            RenderResource& operator=(const RenderResource&) = delete;

            RenderResource(RenderResource&&) = delete;
            RenderResource& operator=(RenderResource&&) = delete;

            virtual void reload() {}

        protected:
            RenderDevice& renderDevice;
        };
    } // namespace graphics
} // namespace ouzel
