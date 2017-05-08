// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextureOGL.h"
#include "core/Engine.h"
#include "RendererOGL.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        static GLint convertPixelFormat(PixelFormat pixelFormat)
        {
            switch (pixelFormat)
            {
                case PixelFormat::A8_UNORM: return 0;
                case PixelFormat::R8_UNORM: return GL_R8;
                case PixelFormat::R8_SNORM: return GL_R8_SNORM;
                case PixelFormat::R8_UINT: return GL_R8UI;
                case PixelFormat::R8_SINT: return GL_R8I;
                case PixelFormat::R16_UNORM: return GL_R16;
                case PixelFormat::R16_SNORM: return GL_R16_SNORM;
                case PixelFormat::R16_UINT: return GL_R16UI;
                case PixelFormat::R16_SINT: return GL_R16I;
                case PixelFormat::R16_FLOAT: return GL_R16F;
                case PixelFormat::R32_UINT: return GL_R32UI;
                case PixelFormat::R32_SINT: return GL_R32I;
                case PixelFormat::R32_FLOAT: return GL_R32F;
                case PixelFormat::RG8_UNORM: return GL_RG8;
                case PixelFormat::RG8_SNORM: return GL_RG8_SNORM;
                case PixelFormat::RG8_UINT: return GL_RG8UI;
                case PixelFormat::RG8_SINT: return GL_RG8I;
                case PixelFormat::RGB8_UNORM: return GL_RGB8;
                case PixelFormat::RGB8_SNORM: return GL_RGB8_SNORM;
                case PixelFormat::RGB8_UINT: return GL_RGB8UI;
                case PixelFormat::RGB8_SINT: return GL_RGB8I;
                case PixelFormat::RGBA8_UNORM: return GL_RGBA8;
                case PixelFormat::RGBA8_SNORM: return GL_RGBA8_SNORM;
                case PixelFormat::RGBA8_UINT: return GL_RGBA8UI;
                case PixelFormat::RGBA8_SINT: return GL_RGBA8I;
                case PixelFormat::RGBA16_UNORM: return GL_RGBA16;
                case PixelFormat::RGBA16_SNORM: return GL_RGBA16_SNORM;
                case PixelFormat::RGBA16_UINT: return GL_RGBA16UI;
                case PixelFormat::RGBA16_SINT: return GL_RGBA16I;
                case PixelFormat::RGBA16_FLOAT: return GL_RGBA16F;
                case PixelFormat::RGBA32_UINT: return GL_RGBA32UI;
                case PixelFormat::RGBA32_SINT: return GL_RGBA32I;
                case PixelFormat::RGBA32_FLOAT: return GL_RGBA32F;
                default: return 0;
            }
        }

        static GLenum getPixelDataFormat(PixelFormat pixelFormat)
        {
            switch (pixelFormat)
            {
                case PixelFormat::A8_UNORM:
                    return GL_ALPHA;
                case PixelFormat::R8_UNORM:
                case PixelFormat::R8_SNORM:
                case PixelFormat::R8_UINT:
                case PixelFormat::R8_SINT:
                case PixelFormat::R16_UNORM:
                case PixelFormat::R16_SNORM:
                case PixelFormat::R16_UINT:
                case PixelFormat::R16_SINT:
                case PixelFormat::R16_FLOAT:
                case PixelFormat::R32_UINT:
                case PixelFormat::R32_SINT:
                case PixelFormat::R32_FLOAT:
                     return GL_RED;
                case PixelFormat::RG8_UNORM:
                case PixelFormat::RG8_SNORM:
                case PixelFormat::RG8_UINT:
                case PixelFormat::RG8_SINT:
                    return GL_RG;
                case PixelFormat::RGB8_UNORM:
                case PixelFormat::RGB8_SNORM:
                case PixelFormat::RGB8_UINT:
                case PixelFormat::RGB8_SINT:
                    return GL_RGB;
                case PixelFormat::RGBA8_UNORM:
                case PixelFormat::RGBA8_SNORM:
                case PixelFormat::RGBA8_UINT:
                case PixelFormat::RGBA8_SINT:
                case PixelFormat::RGBA16_UNORM:
                case PixelFormat::RGBA16_SNORM:
                case PixelFormat::RGBA16_UINT:
                case PixelFormat::RGBA16_SINT:
                case PixelFormat::RGBA16_FLOAT:
                case PixelFormat::RGBA32_UINT:
                case PixelFormat::RGBA32_SINT:
                case PixelFormat::RGBA32_FLOAT:
                    return GL_RGBA;
                default:
                    return 0;
            }
        }

        static GLenum getPixelDataType(PixelFormat pixelFormat)
        {
            switch (pixelFormat)
            {
                case PixelFormat::A8_UNORM:
                case PixelFormat::R8_UNORM:
                case PixelFormat::R16_UNORM:
                case PixelFormat::RG8_UNORM:
                case PixelFormat::RGB8_UNORM:
                case PixelFormat::RGBA8_UNORM:
                case PixelFormat::RGBA16_UNORM:
                    return GL_UNSIGNED_BYTE;
                case PixelFormat::R8_SNORM:
                case PixelFormat::R16_SNORM:
                case PixelFormat::RG8_SNORM:
                case PixelFormat::RGB8_SNORM:
                case PixelFormat::RGBA8_SNORM:
                case PixelFormat::RGBA16_SNORM:
                    return GL_BYTE;
                case PixelFormat::R8_UINT:
                case PixelFormat::R16_UINT:
                case PixelFormat::R32_UINT:
                case PixelFormat::RG8_UINT:
                case PixelFormat::RGB8_UINT:
                case PixelFormat::RGBA8_UINT:
                case PixelFormat::RGBA16_UINT:
                case PixelFormat::RGBA32_UINT:
                    return GL_UNSIGNED_INT;
                case PixelFormat::R8_SINT:
                case PixelFormat::R16_SINT:
                case PixelFormat::R32_SINT:
                case PixelFormat::RG8_SINT:
                case PixelFormat::RGB8_SINT:
                case PixelFormat::RGBA8_SINT:
                case PixelFormat::RGBA16_SINT:
                case PixelFormat::RGBA32_SINT:
                    return GL_INT;
                case PixelFormat::R16_FLOAT:
                case PixelFormat::R32_FLOAT:
                case PixelFormat::RGBA16_FLOAT:
                case PixelFormat::RGBA32_FLOAT:
                    return GL_FLOAT;
                default:
                    return 0;
            }
        }

        TextureOGL::TextureOGL()
        {
        }

        TextureOGL::~TextureOGL()
        {
            if (depthBufferId)
            {
                RendererOGL::deleteRenderBuffer(depthBufferId);
            }

            if (frameBufferId)
            {
                RendererOGL::deleteFrameBuffer(frameBufferId);
            }

            if (textureId)
            {
                RendererOGL::deleteTexture(textureId);
            }
        }

        bool TextureOGL::upload()
        {
            std::lock_guard<std::mutex> lock(uploadMutex);

            if (dirty)
            {
                if (!textureId)
                {
                    glGenTextures(1, &textureId);

                    if (RendererOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to create texture";
                        return false;
                    }
                }

                RendererOGL::bindTexture(textureId, 0);

                RendererOGL* rendererOGL = static_cast<RendererOGL*>(sharedEngine->getRenderer());

                if (dirty & DIRTY_DATA)
                {
                    if (static_cast<GLsizei>(size.v[0]) != width ||
                        static_cast<GLsizei>(size.v[1]) != height)
                    {
                        width = static_cast<GLsizei>(size.v[0]);
                        height = static_cast<GLsizei>(size.v[1]);

                        for (size_t level = 0; level < levels.size(); ++level)
                        {
                            if (!levels[level].data.empty())
                            {
                                glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), convertPixelFormat(pixelFormat),
                                             static_cast<GLsizei>(levels[level].size.v[0]),
                                             static_cast<GLsizei>(levels[level].size.v[1]), 0,
                                             getPixelDataFormat(pixelFormat), getPixelDataType(pixelFormat),
                                             levels[level].data.data());
                            }
                        }

                        if (renderTarget && rendererOGL->isRenderTargetsSupported())
                        {
                            if (!frameBufferId)
                            {
                                glGenFramebuffersProc(1, &frameBufferId);

                                if (RendererOGL::checkOpenGLError())
                                {
                                    Log(Log::Level::ERR) << "Failed to create frame buffer";
                                    return false;
                                }
                            }

                            RendererOGL::bindFrameBuffer(frameBufferId);

                            if (glCheckFramebufferStatusProc(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                            {
                                RendererOGL::bindTexture(textureId, 0);

                                glTexImage2D(GL_TEXTURE_2D, 0, convertPixelFormat(pixelFormat),
                                             static_cast<GLsizei>(size.v[0]),
                                             static_cast<GLsizei>(size.v[1]), 0,
                                             getPixelDataFormat(pixelFormat), getPixelDataType(pixelFormat), 0);

                                // TODO: blit multisample render buffer to texture
                                glFramebufferTexture2DProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);
                                
                                if (depth)
                                {
                                    glGenRenderbuffersProc(1, &depthBufferId);
                                    glBindRenderbufferProc(GL_RENDERBUFFER, depthBufferId);

                                    if (sampleCount > 1 && rendererOGL->isMultisamplingSupported())
                                    {
                                        glRenderbufferStorageMultisampleProc(GL_RENDERBUFFER,
                                                                             static_cast<GLsizei>(sampleCount),
                                                                             GL_DEPTH_COMPONENT,
                                                                             static_cast<GLsizei>(size.v[0]),
                                                                             static_cast<GLsizei>(size.v[1]));
                                    }
                                    else
                                    {
                                        glRenderbufferStorageProc(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                                                                  static_cast<GLsizei>(size.v[0]),
                                                                  static_cast<GLsizei>(size.v[1]));
                                    }

                                    glFramebufferRenderbufferProc(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufferId);
                                }

                                if (glCheckFramebufferStatusProc(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                                {
                                    Log(Log::Level::ERR) << "Failed to create frame buffer";
                                    return false;
                                }
                            }
                        }
                    }
                    else
                    {
                        for (size_t level = 0; level < levels.size(); ++level)
                        {
                            if (!levels[level].data.empty())
                            {
                                glTexSubImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), 0, 0,
                                                static_cast<GLsizei>(levels[level].size.v[0]),
                                                static_cast<GLsizei>(levels[level].size.v[1]),
                                                getPixelDataFormat(pixelFormat), getPixelDataType(pixelFormat),
                                                levels[level].data.data());

                                if (RendererOGL::checkOpenGLError())
                                {
                                    Log(Log::Level::ERR) << "Failed to upload texture data";
                                    return false;
                                }
                            }
                        }
                    }
                }

                if (dirty & DIRTY_PARAMETERS)
                {
                    clearMask = 0;

                    if (clearColorBuffer) clearMask |= GL_COLOR_BUFFER_BIT;
                    if (clearDepthBuffer) clearMask |= GL_DEPTH_BUFFER_BIT;

                    frameBufferClearColor[0] = clearColor.normR();
                    frameBufferClearColor[1] = clearColor.normG();
                    frameBufferClearColor[2] = clearColor.normB();
                    frameBufferClearColor[3] = clearColor.normA();

                    Texture::Filter finalFilter = (filter == Texture::Filter::DEFAULT) ? rendererOGL->getTextureFilter() : filter;

                    switch (finalFilter)
                    {
                        case Texture::Filter::DEFAULT:
                        case Texture::Filter::POINT:
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                            break;
                        case Texture::Filter::LINEAR:
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                            break;
                        case Texture::Filter::BILINEAR:
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_LINEAR_MIPMAP_NEAREST : GL_LINEAR);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                            break;
                        case Texture::Filter::TRILINEAR:
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (levels.size() > 1) ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
                            break;
                    }

                    switch (addressX)
                    {
                        case Texture::Address::CLAMP:
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                            break;
                        case Texture::Address::REPEAT:
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                            break;
                    }

                    switch (addressY)
                    {
                        case Texture::Address::CLAMP:
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                            break;
                        case Texture::Address::REPEAT:
                            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                            break;
                    }

                    uint32_t finalMaxAnisotropy = (maxAnisotropy == 0) ? rendererOGL->getMaxAnisotropy() : maxAnisotropy;

                    if (finalMaxAnisotropy > 1 && rendererOGL->isAnisotropicFilteringSupported())
                    {
                        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<GLint>(finalMaxAnisotropy));
                    }
                    
                    if (RendererOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to set texture parameters";
                        return false;
                    }
                }

                dirty = 0;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
