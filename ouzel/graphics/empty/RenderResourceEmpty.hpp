// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#pragma once

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceEmpty;

        class RenderResourceEmpty
        {
        public:
            explicit RenderResourceEmpty(RenderDeviceEmpty& initRenderDevice):
                renderDevice(initRenderDevice)
            {
            }

            virtual ~RenderResourceEmpty() {}

        protected:
            RenderDeviceEmpty& renderDevice;
        };
    } // namespace graphics
} // namespace ouzel
