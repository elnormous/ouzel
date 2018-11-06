// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef RENDERRESOURCEMETAL_HPP
#define RENDERRESOURCEMETAL_HPP

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceMetal;

        class RenderResourceMetal
        {
        public:
            explicit RenderResourceMetal(RenderDeviceMetal& initRenderDevice):
                renderDevice(initRenderDevice)
            {
            }

            virtual ~RenderResourceMetal() {}

        protected:
            RenderDeviceMetal& renderDevice;
        };
    } // namespace graphics
} // namespace ouzel

#endif // RENDERRESOURCEMETAL_HPP
