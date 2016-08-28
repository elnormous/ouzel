// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererOGLMacOS.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace graphics
    {
        RendererOGLMacOS::~RendererOGLMacOS()
        {
            if (openGLContext)
            {
                [openGLContext release];
            }

            if (pixelFormat)
            {
                [pixelFormat release];
            }
        }

        void RendererOGLMacOS::free()
        {
            RendererOGL::free();

            if (openGLContext)
            {
                [openGLContext release];
                openGLContext = Nil;
            }

            if (pixelFormat)
            {
                [pixelFormat release];
                pixelFormat = Nil;
            }
        }

        bool RendererOGLMacOS::init(const WindowPtr& window,
                                  uint32_t newSampleCount,
                                  TextureFiltering newTextureFiltering,
                                  bool newVerticalSync)
        {
            free();

            // Create pixel format
            std::vector<NSOpenGLPixelFormatAttribute> openGL3Attributes =
            {
                NSOpenGLPFADoubleBuffer,
                NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersion3_2Core,
                NSOpenGLPFAColorSize, 24,
                NSOpenGLPFAAlphaSize, 8,
                //NSOpenGLPFADepthSize, 32, // set depth buffer size
            };

            if (newSampleCount > 1)
            {
                openGL3Attributes.push_back(NSOpenGLPFAMultisample);
                openGL3Attributes.push_back(NSOpenGLPFASampleBuffers);
                openGL3Attributes.push_back(static_cast<NSOpenGLPixelFormatAttribute>(1));
                openGL3Attributes.push_back(NSOpenGLPFASamples);
                openGL3Attributes.push_back(static_cast<NSOpenGLPixelFormatAttribute>(newSampleCount));
            }

            openGL3Attributes.push_back(0);

            pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:openGL3Attributes.data()];

            if (pixelFormat)
            {
                apiVersion = 3;
                log("Using OpenGL 3.2");
            }
            else
            {
                log("Failed to crete OpenGL 3.2 pixel format");

                std::vector<NSOpenGLPixelFormatAttribute> openGL2Attributes =
                {
                    NSOpenGLPFADoubleBuffer,
                    NSOpenGLPFAOpenGLProfile, NSOpenGLProfileVersionLegacy,
                    NSOpenGLPFAColorSize, 24,
                    NSOpenGLPFAAlphaSize, 8,
                    //NSOpenGLPFAMultisample,
                };

                if (newSampleCount > 1)
                {
                    openGL2Attributes.push_back(NSOpenGLPFAMultisample);
                    openGL2Attributes.push_back(NSOpenGLPFASampleBuffers);
                    openGL2Attributes.push_back(static_cast<NSOpenGLPixelFormatAttribute>(1));
                    openGL2Attributes.push_back(NSOpenGLPFASamples);
                    openGL2Attributes.push_back(static_cast<NSOpenGLPixelFormatAttribute>(newSampleCount));
                }

                openGL2Attributes.push_back(0);

                pixelFormat = [[NSOpenGLPixelFormat alloc] initWithAttributes:openGL2Attributes.data()];

                if (pixelFormat)
                {
                    apiVersion = 2;
                    log("Using OpenGL 2");
                }
            }

            if (!pixelFormat)
            {
                log("Failed to crete OpenGL 2 pixel format");
                return Nil;
            }

            // Create OpenGL context
            openGLContext = [[NSOpenGLContext alloc] initWithFormat:pixelFormat shareContext:NULL];

            return RendererOGL::init(window, newSampleCount, newTextureFiltering, newVerticalSync);
        }

        bool RendererOGLMacOS::present()
        {
            [openGLContext makeCurrentContext];

            if (!RendererOGL::present())
            {
                return false;
            }

            [openGLContext flushBuffer];

            return true;
        }

        void RendererOGLMacOS::setSize(const Size2& newSize)
        {
            RendererOGL::setSize(newSize);

            dirty = true;
        }

        bool RendererOGLMacOS::update()
        {
            if (dirty)
            {
                [openGLContext update];
            }

            return RendererOGL::update();
        }
    } // namespace graphics
} // namespace ouzel
