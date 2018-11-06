// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#ifndef RENDERRESOURCEOGL_HPP
#define RENDERRESOURCEOGL_HPP

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

#endif // RENDERRESOURCEOGL_HPP
