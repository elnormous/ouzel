// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_RENDERRESOURCEOGL_HPP
#define OUZEL_RENDERRESOURCEOGL_HPP

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGL;

        class RenderResourceOGL
        {
        public:
            explicit RenderResourceOGL(RenderDeviceOGL& initRenderDevice):
                renderDevice(initRenderDevice)
            {
            }
            virtual ~RenderResourceOGL() {}

            virtual void reload() = 0;

        protected:
            RenderDeviceOGL& renderDevice;
        };
    } // namespace graphics
} // namespace ouzel

#endif // OUZEL_RENDERRESOURCEOGL_HPP
