// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/CompileConfig.h"

#if OUZEL_PLATFORM_MACOS && OUZEL_SUPPORTS_OPENGL

#if defined(__OBJC__)
#import <CoreVideo/CoreVideo.h>
#import <AppKit/NSOpenGL.h>
typedef NSOpenGLContext* NSOpenGLContextPtr;
typedef NSOpenGLPixelFormat* NSOpenGLPixelFormatPtr;
#else
#include <objc/objc.h>
typedef void* CVDisplayLinkRef;
typedef id NSOpenGLContextPtr;
typedef id NSOpenGLPixelFormatPtr;
#endif

#include "graphics/opengl/RenderDeviceOGL.hpp"
#include "events/EventHandler.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGLMacOS: public RenderDeviceOGL
        {
            friend Renderer;
        public:
            virtual ~RenderDeviceOGLMacOS();

            NSOpenGLContextPtr getOpenGLContext() const { return openGLContext; }

            void renderCallback();

        protected:
            RenderDeviceOGLMacOS();
            virtual bool init(Window* newWindow,
                              const Size2& newSize,
                              uint32_t newSampleCount,
                              Texture::Filter newTextureFilter,
                              uint32_t newMaxAnisotropy,
                              bool newVerticalSync,
                              bool newDepth,
                              bool newDebugRenderer) override;

            virtual void setSize(const Size2& newSize) override;
            virtual bool lockContext() override;
            virtual bool swapBuffers() override;

            bool handleWindow(Event::Type type, const WindowEvent& event);

            NSOpenGLContextPtr openGLContext = nil;
            NSOpenGLPixelFormatPtr pixelFormat = nil;

            CVDisplayLinkRef displayLink = nullptr;
            EventHandler eventHandler;

            std::atomic<bool> running;
        };
    } // namespace graphics
} // namespace ouzel

#endif
