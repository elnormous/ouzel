// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "core/Setup.h"

#if defined(__EMSCRIPTEN__) && OUZEL_COMPILE_OPENGL

#include "OGLRenderDeviceEm.hpp"
#include "utils/Utils.hpp"

namespace ouzel
{
    namespace graphics
    {
        namespace opengl
        {
            RenderDeviceEm::RenderDeviceEm(const std::function<void(const Event&)>& initCallback):
                RenderDevice(initCallback)
            {
            }

            RenderDeviceEm::~RenderDeviceEm()
            {
                if (webGLContext)
                    emscripten_webgl_destroy_context(webGLContext);
            }

            void RenderDeviceEm::init(Window* newWindow,
                                      const Size2U& newSize,
                                      std::uint32_t newSampleCount,
                                      SamplerFilter newTextureFilter,
                                      std::uint32_t newMaxAnisotropy,
                                      bool newSrgb,
                                      bool newVerticalSync,
                                      bool newDepth,
                                      bool newStencil,
                                      bool newDebugRenderer)
            {
                apiVersion = ApiVersion(2, 0);

                EmscriptenWebGLContextAttributes attrs;
                emscripten_webgl_init_context_attributes(&attrs);

                attrs.alpha = true;
                attrs.depth = newDepth;
                attrs.stencil = newStencil;
                attrs.antialias = newSampleCount > 0;

                webGLContext = emscripten_webgl_create_context(0, &attrs);

                if (!webGLContext)
                    throw std::runtime_error("Failed to create WebGL context");

                EMSCRIPTEN_RESULT result = emscripten_webgl_make_context_current(webGLContext);

                if (result != EMSCRIPTEN_RESULT_SUCCESS)
                    throw std::runtime_error("Failed to make WebGL context current");

                emscripten_set_main_loop_timing(newVerticalSync ? EM_TIMING_RAF : EM_TIMING_SETTIMEOUT, 1);

                RenderDevice::init(newWindow,
                                   newSize,
                                   newSampleCount,
                                   newTextureFilter,
                                   newMaxAnisotropy,
                                   newSrgb,
                                   newVerticalSync,
                                   newDepth,
                                   newStencil,
                                   newDebugRenderer);
            }
        } // namespace opengl
    } // namespace graphics
} // namespace ouzel

#endif
