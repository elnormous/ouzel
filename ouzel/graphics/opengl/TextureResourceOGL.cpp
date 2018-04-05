// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "TextureResourceOGL.hpp"
#include "RenderDeviceOGL.hpp"
#include "utils/Utils.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        TextureResourceOGL::TextureResourceOGL(RenderDeviceOGL* initRenderDeviceOGL):
            renderDeviceOGL(initRenderDeviceOGL)
        {
        }

        TextureResourceOGL::~TextureResourceOGL()
        {
            if (depthBufferId)
            {
                renderDeviceOGL->deleteRenderBuffer(depthBufferId);
            }

            if (frameBufferId)
            {
                renderDeviceOGL->deleteFrameBuffer(frameBufferId);
            }

            if (textureId)
            {
                renderDeviceOGL->deleteTexture(textureId);
            }
        }

        static GLint getOGLInternalPixelFormat(PixelFormat pixelFormat, uint32_t openGLVersion)
        {
#if OUZEL_SUPPORTS_OPENGLES
            if (openGLVersion >= 3)
            {
                switch (pixelFormat)
                {
                    case PixelFormat::A8_UNORM: return GL_ALPHA8_OES;
                    case PixelFormat::R8_UNORM: return GL_R8;
                    case PixelFormat::R8_SNORM: return GL_R8_SNORM;
                    case PixelFormat::R8_UINT: return GL_R8UI;
                    case PixelFormat::R8_SINT: return GL_R8I;

                    case PixelFormat::R16_UNORM: return GL_NONE;
                    case PixelFormat::R16_SNORM: return GL_NONE;
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
                    case PixelFormat::RGBA8_UNORM: return GL_RGBA8;
                    case PixelFormat::RGBA8_SNORM: return GL_RGBA8_SNORM;
                    case PixelFormat::RGBA8_UINT: return GL_RGBA8UI;
                    case PixelFormat::RGBA8_SINT: return GL_RGBA8I;

                    case PixelFormat::RGBA16_UNORM: return GL_NONE;
                    case PixelFormat::RGBA16_SNORM: return GL_NONE;
                    case PixelFormat::RGBA16_UINT: return GL_RGBA16UI;
                    case PixelFormat::RGBA16_SINT: return GL_RGBA16I;
                    case PixelFormat::RGBA16_FLOAT: return GL_RGBA16F;
                    case PixelFormat::RGBA32_UINT: return GL_RGBA32UI;
                    case PixelFormat::RGBA32_SINT: return GL_RGBA32I;
                    case PixelFormat::RGBA32_FLOAT: return GL_RGBA32F;
                    default: return GL_NONE;
                }
            }
            else
            {
                switch (pixelFormat)
                {
                    case PixelFormat::A8_UNORM: return GL_ALPHA;
                    case PixelFormat::RGBA8_UNORM: return GL_RGBA;
                    default: return GL_NONE;
                }
            }
#else
            OUZEL_UNUSED(openGLVersion);

            switch (pixelFormat)
            {
                case PixelFormat::A8_UNORM: return GL_ALPHA8_EXT;

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
                default: return GL_NONE;
            }
#endif
        }

        static GLenum getOGLPixelFormat(PixelFormat pixelFormat)
        {
            switch (pixelFormat)
            {
                case PixelFormat::A8_UNORM:
                    return GL_ALPHA;
                case PixelFormat::R8_UNORM:
                case PixelFormat::R8_SNORM:
                case PixelFormat::R16_UNORM:
                case PixelFormat::R16_SNORM:
                case PixelFormat::R16_FLOAT:
                case PixelFormat::R32_FLOAT:
                    return GL_RED;
                case PixelFormat::R8_UINT:
                case PixelFormat::R8_SINT:
                case PixelFormat::R16_UINT:
                case PixelFormat::R16_SINT:
                case PixelFormat::R32_UINT:
                case PixelFormat::R32_SINT:
                     return GL_RED_INTEGER;
                case PixelFormat::RG8_UNORM:
                case PixelFormat::RG8_SNORM:
                    return GL_RG;
                case PixelFormat::RG8_UINT:
                case PixelFormat::RG8_SINT:
                    return GL_RG_INTEGER;
                case PixelFormat::RGBA8_UNORM:
                case PixelFormat::RGBA8_SNORM:
                case PixelFormat::RGBA16_UNORM:
                case PixelFormat::RGBA16_SNORM:
                case PixelFormat::RGBA16_FLOAT:
                case PixelFormat::RGBA32_FLOAT:
                    return GL_RGBA;
                case PixelFormat::RGBA8_UINT:
                case PixelFormat::RGBA8_SINT:
                case PixelFormat::RGBA16_UINT:
                case PixelFormat::RGBA16_SINT:
                case PixelFormat::RGBA32_UINT:
                case PixelFormat::RGBA32_SINT:
                    return GL_RGBA_INTEGER;
                default:
                    return 0;
            }
        }

        static GLenum getOGLPixelType(PixelFormat pixelFormat)
        {
            switch (pixelFormat)
            {
                case PixelFormat::A8_UNORM:
                case PixelFormat::R8_UNORM:
                case PixelFormat::R16_UNORM:
                case PixelFormat::RG8_UNORM:
                case PixelFormat::RGBA8_UNORM:
                case PixelFormat::RGBA16_UNORM:
                    return GL_UNSIGNED_BYTE;
                case PixelFormat::R8_SNORM:
                case PixelFormat::R16_SNORM:
                case PixelFormat::RG8_SNORM:
                case PixelFormat::RGBA8_SNORM:
                case PixelFormat::RGBA16_SNORM:
                    return GL_BYTE;
                case PixelFormat::R8_UINT:
                case PixelFormat::R16_UINT:
                case PixelFormat::R32_UINT:
                case PixelFormat::RG8_UINT:
                case PixelFormat::RGBA8_UINT:
                case PixelFormat::RGBA16_UINT:
                case PixelFormat::RGBA32_UINT:
                    return GL_UNSIGNED_INT;
                case PixelFormat::R8_SINT:
                case PixelFormat::R16_SINT:
                case PixelFormat::R32_SINT:
                case PixelFormat::RG8_SINT:
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

        bool TextureResourceOGL::init(const Size2& newSize,
                                      uint32_t newFlags,
                                      uint32_t newMipmaps,
                                      uint32_t newSampleCount,
                                      PixelFormat newPixelFormat)
        {
            if (!TextureResource::init(newSize,
                                       newFlags,
                                       newMipmaps,
                                       newSampleCount,
                                       newPixelFormat))
            {
                return false;
            }

            clearMask = 0;
            if (clearColorBuffer) clearMask |= GL_COLOR_BUFFER_BIT;
            if (clearDepthBuffer) clearMask |= GL_DEPTH_BUFFER_BIT;

            frameBufferClearColor[0] = clearColor.normR();
            frameBufferClearColor[1] = clearColor.normG();
            frameBufferClearColor[2] = clearColor.normB();
            frameBufferClearColor[3] = clearColor.normA();

            if (!createTexture())
            {
                return false;
            }

            renderDeviceOGL->bindTexture(textureId, 0);

            if (!(flags & Texture::RENDER_TARGET))
            {
                if (!levels.empty())
                {
                    if (renderDeviceOGL->isTextureBaseLevelSupported()) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
                    if (renderDeviceOGL->isTextureMaxLevelSupported()) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<GLsizei>(levels.size()) - 1);

                    if (RenderDeviceOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to set texture base and max levels";
                        return false;
                    }
                }

                for (size_t level = 0; level < levels.size(); ++level)
                {
                    glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), oglInternalPixelFormat,
                                 static_cast<GLsizei>(levels[level].size.width),
                                 static_cast<GLsizei>(levels[level].size.height), 0,
                                 oglPixelFormat, oglPixelType, nullptr);
                }

                if (RenderDeviceOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to set texture size";
                    return false;
                }
            }

            return setTextureParameters();
        }

        bool TextureResourceOGL::init(const std::vector<uint8_t>& newData,
                                      const Size2& newSize,
                                      uint32_t newFlags,
                                      uint32_t newMipmaps,
                                      PixelFormat newPixelFormat)
        {
            if (!TextureResource::init(newData,
                                       newSize,
                                       newFlags,
                                       newMipmaps,
                                       newPixelFormat))
            {
                return false;
            }

            if (!createTexture())
            {
                return false;
            }

            renderDeviceOGL->bindTexture(textureId, 0);

            if (!(flags & Texture::RENDER_TARGET))
            {
                if (!levels.empty())
                {
                    if (renderDeviceOGL->isTextureBaseLevelSupported()) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
                    if (renderDeviceOGL->isTextureMaxLevelSupported()) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<GLsizei>(levels.size()) - 1);

                    if (RenderDeviceOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to set texture base and max levels";
                        return false;
                    }
                }

                for (size_t level = 0; level < levels.size(); ++level)
                {
                    if (!levels[level].data.empty())
                    {
                        glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), oglInternalPixelFormat,
                                     static_cast<GLsizei>(levels[level].size.width),
                                     static_cast<GLsizei>(levels[level].size.height), 0,
                                     oglPixelFormat, oglPixelType, levels[level].data.data());
                    }
                    else
                    {
                        glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), oglInternalPixelFormat,
                                     static_cast<GLsizei>(levels[level].size.width),
                                     static_cast<GLsizei>(levels[level].size.height), 0,
                                     oglPixelFormat, oglPixelType, nullptr);
                    }
                }

                if (RenderDeviceOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to upload texture data";
                    return false;
                }
            }

            return setTextureParameters();
        }

        bool TextureResourceOGL::init(const std::vector<Texture::Level>& newLevels,
                                      const Size2& newSize,
                                      uint32_t newFlags,
                                      PixelFormat newPixelFormat)
        {
            if (!TextureResource::init(newLevels,
                                       newSize,
                                       newFlags,
                                       newPixelFormat))
            {
                return false;
            }

            if (!createTexture())
            {
                return false;
            }

            renderDeviceOGL->bindTexture(textureId, 0);

            if (!(flags & Texture::RENDER_TARGET))
            {
                if (!levels.empty())
                {
                    if (renderDeviceOGL->isTextureBaseLevelSupported()) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
                    if (renderDeviceOGL->isTextureMaxLevelSupported()) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<GLsizei>(levels.size()) - 1);

                    if (RenderDeviceOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to set texture base and max levels";
                        return false;
                    }
                }

                for (size_t level = 0; level < levels.size(); ++level)
                {
                    if (!levels[level].data.empty())
                    {
                        glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), oglInternalPixelFormat,
                                     static_cast<GLsizei>(levels[level].size.width),
                                     static_cast<GLsizei>(levels[level].size.height), 0,
                                     oglPixelFormat, oglPixelType, levels[level].data.data());
                    }
                    else
                    {
                        glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), oglInternalPixelFormat,
                                     static_cast<GLsizei>(levels[level].size.width),
                                     static_cast<GLsizei>(levels[level].size.height), 0,
                                     oglPixelFormat, oglPixelType, nullptr);
                    }
                }

                if (RenderDeviceOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to upload texture data";
                    return false;
                }
            }

            return setTextureParameters();
        }

        bool TextureResourceOGL::reload()
        {
            textureId = 0;
            frameBufferId = 0;
            depthBufferId = 0;

            if (!createTexture())
            {
                return false;
            }

            renderDeviceOGL->bindTexture(textureId, 0);

            if (!(flags & Texture::RENDER_TARGET))
            {
                if (!levels.empty())
                {
                    if (renderDeviceOGL->isTextureBaseLevelSupported()) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
                    if (renderDeviceOGL->isTextureMaxLevelSupported()) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<GLsizei>(levels.size()) - 1);

                    if (RenderDeviceOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to set texture base and max levels";
                        return false;
                    }
                }

                for (size_t level = 0; level < levels.size(); ++level)
                {
                    if (!levels[level].data.empty())
                    {
                        glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), oglInternalPixelFormat,
                                     static_cast<GLsizei>(levels[level].size.width),
                                     static_cast<GLsizei>(levels[level].size.height), 0,
                                     oglPixelFormat, oglPixelType, levels[level].data.data());
                    }
                    else
                    {
                        glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), oglInternalPixelFormat,
                                     static_cast<GLsizei>(levels[level].size.width),
                                     static_cast<GLsizei>(levels[level].size.height), 0,
                                     oglPixelFormat, oglPixelType, nullptr);
                    }
                }

                if (RenderDeviceOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to upload texture data";
                    return false;
                }
            }

            return setTextureParameters();
        }

        bool TextureResourceOGL::setSize(const Size2& newSize)
        {
            if (!TextureResource::setSize(newSize))
            {
                return false;
            }

            if (!textureId)
            {
                Log(Log::Level::ERR) << "Texture not initialized";
                return false;
            }

            renderDeviceOGL->bindTexture(textureId, 0);

            if (static_cast<GLsizei>(size.width) != width ||
                static_cast<GLsizei>(size.height) != height)
            {
                width = static_cast<GLsizei>(size.width);
                height = static_cast<GLsizei>(size.height);

                if (flags & Texture::RENDER_TARGET)
                {
                    glTexImage2D(GL_TEXTURE_2D, 0, oglInternalPixelFormat,
                                 width, height, 0,
                                 oglPixelFormat, oglPixelType, nullptr);

                    if (flags & Texture::DEPTH_BUFFER)
                    {
                        glBindRenderbufferProc(GL_RENDERBUFFER, depthBufferId);

                        if (sampleCount > 1 && renderDeviceOGL->isMultisamplingSupported())
                        {
                            glRenderbufferStorageMultisampleProc(GL_RENDERBUFFER,
                                                                 static_cast<GLsizei>(sampleCount),
                                                                 GL_DEPTH_COMPONENT,
                                                                 width, height);
                        }
                        else
                        {
                            glRenderbufferStorageProc(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                                                      width, height);
                        }
                    }

                    if (RenderDeviceOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to create render buffer";
                        return false;
                    }
                }
                else
                {
                    if (!levels.empty())
                    {
                        if (renderDeviceOGL->isTextureBaseLevelSupported()) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
                        if (renderDeviceOGL->isTextureMaxLevelSupported()) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<GLsizei>(levels.size()) - 1);

                        if (RenderDeviceOGL::checkOpenGLError())
                        {
                            Log(Log::Level::ERR) << "Failed to set texture base and max levels";
                            return false;
                        }
                    }

                    for (size_t level = 0; level < levels.size(); ++level)
                    {
                        // resize all the mip levels
                        glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), oglInternalPixelFormat,
                                     static_cast<GLsizei>(levels[level].size.width),
                                     static_cast<GLsizei>(levels[level].size.height), 0,
                                     oglPixelFormat, oglPixelType,
                                     nullptr);
                    }

                    if (RenderDeviceOGL::checkOpenGLError())
                    {
                        Log(Log::Level::ERR) << "Failed to set texture size";
                        return false;
                    }
                }
            }

            return true;
        }

        bool TextureResourceOGL::setData(const std::vector<uint8_t>& newData, const Size2& newSize)
        {
            if (!TextureResource::setData(newData, newSize))
            {
                return false;
            }

            if (!textureId)
            {
                Log(Log::Level::ERR) << "Texture not initialized";
                return false;
            }

            renderDeviceOGL->bindTexture(textureId, 0);

            if (!(flags & Texture::RENDER_TARGET))
            {
                if (static_cast<GLsizei>(size.width) != width ||
                    static_cast<GLsizei>(size.height) != height)
                {
                    width = static_cast<GLsizei>(size.width);
                    height = static_cast<GLsizei>(size.height);

                    if (!levels.empty())
                    {
                        if (renderDeviceOGL->isTextureBaseLevelSupported()) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
                        if (renderDeviceOGL->isTextureMaxLevelSupported()) glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<GLsizei>(levels.size()) - 1);

                        if (RenderDeviceOGL::checkOpenGLError())
                        {
                            Log(Log::Level::ERR) << "Failed to set texture base and max levels";
                            return false;
                        }
                    }

                    for (size_t level = 0; level < levels.size(); ++level)
                    {
                        if (!levels[level].data.empty())
                        {
                            // resize and fill all the mip levels with data
                            glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), oglInternalPixelFormat,
                                         static_cast<GLsizei>(levels[level].size.width),
                                         static_cast<GLsizei>(levels[level].size.height), 0,
                                         oglPixelFormat, oglPixelType,
                                         levels[level].data.data());
                        }
                        else
                        {
                            // resize all the mip levels
                            glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), oglInternalPixelFormat,
                                         static_cast<GLsizei>(levels[level].size.width),
                                         static_cast<GLsizei>(levels[level].size.height), 0,
                                         oglPixelFormat, oglPixelType,
                                         nullptr);
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
                                            static_cast<GLsizei>(levels[level].size.width),
                                            static_cast<GLsizei>(levels[level].size.height),
                                            oglPixelFormat, oglPixelType,
                                            levels[level].data.data());
                        }
                    }
                }

                if (RenderDeviceOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to upload texture data";
                    return false;
                }
            }

            return true;
        }

        bool TextureResourceOGL::setFilter(Texture::Filter newFilter)
        {
            if (!TextureResource::setFilter(newFilter))
            {
                return false;
            }

            if (!textureId)
            {
                Log(Log::Level::ERR) << "Texture not initialized";
                return false;
            }

            renderDeviceOGL->bindTexture(textureId, 0);

            Texture::Filter finalFilter = (filter == Texture::Filter::DEFAULT) ? renderDeviceOGL->getTextureFilter() : filter;

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
                default:
                    return false;
            }

            if (RenderDeviceOGL::checkOpenGLError())
            {
                Log(Log::Level::ERR) << "Failed to set texture filter";
                return false;
            }

            return true;
        }

        bool TextureResourceOGL::setAddressX(Texture::Address newAddressX)
        {
            if (!TextureResource::setAddressX(newAddressX))
            {
                return false;
            }

            if (!textureId)
            {
                Log(Log::Level::ERR) << "Texture not initialized";
                return false;
            }

            renderDeviceOGL->bindTexture(textureId, 0);

            switch (addressX)
            {
                case Texture::Address::CLAMP:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    break;
                case Texture::Address::REPEAT:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    break;
                case Texture::Address::MIRROR_REPEAT:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
                    break;
                default:
                    return false;
            }

            if (RenderDeviceOGL::checkOpenGLError())
            {
                Log(Log::Level::ERR) << "Failed to set texture wrap mode";
                return false;
            }

            return true;
        }

        bool TextureResourceOGL::setAddressY(Texture::Address newAddressY)
        {
            if (!TextureResource::setAddressY(newAddressY))
            {
                return false;
            }

            if (!textureId)
            {
                Log(Log::Level::ERR) << "Texture not initialized";
                return false;
            }

            renderDeviceOGL->bindTexture(textureId, 0);

            switch (addressY)
            {
                case Texture::Address::CLAMP:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    break;
                case Texture::Address::REPEAT:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                    break;
                case Texture::Address::MIRROR_REPEAT:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
                    break;
                default:
                    return false;
            }

            if (RenderDeviceOGL::checkOpenGLError())
            {
                Log(Log::Level::ERR) << "Failed to set texture wrap mode";
                return false;
            }

            return true;
        }

        bool TextureResourceOGL::setMaxAnisotropy(uint32_t newMaxAnisotropy)
        {
            if (!TextureResource::setMaxAnisotropy(newMaxAnisotropy))
            {
                return false;
            }

            if (!textureId)
            {
                Log(Log::Level::ERR) << "Texture not initialized";
                return false;
            }

            renderDeviceOGL->bindTexture(textureId, 0);

            uint32_t finalMaxAnisotropy = (maxAnisotropy == 0) ? renderDeviceOGL->getMaxAnisotropy() : maxAnisotropy;

            if (finalMaxAnisotropy > 1 && renderDeviceOGL->isAnisotropicFilteringSupported())
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<GLint>(finalMaxAnisotropy));

                if (RenderDeviceOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to set texture max anisotrophy";
                    return false;
                }
            }

            return true;
        }

        bool TextureResourceOGL::setClearColorBuffer(bool clear)
        {
            if (!TextureResource::setClearColorBuffer(clear))
            {
                return false;
            }

            if (clearColorBuffer)
            {
                clearMask |= GL_COLOR_BUFFER_BIT;
            }
            else
            {
                clearMask &= ~static_cast<GLbitfield>(GL_COLOR_BUFFER_BIT);
            }

            return true;
        }

        bool TextureResourceOGL::setClearDepthBuffer(bool clear)
        {
            if (!TextureResource::setClearDepthBuffer(clear))
            {
                return false;
            }

            if (clearDepthBuffer)
            {
                clearMask |= GL_DEPTH_BUFFER_BIT;
            }
            else
            {
                clearMask &= ~static_cast<GLbitfield>(GL_DEPTH_BUFFER_BIT);
            }

            return true;
        }

        bool TextureResourceOGL::setClearColor(Color color)
        {
            if (!TextureResource::setClearColor(color))
            {
                return false;
            }

            frameBufferClearColor[0] = clearColor.normR();
            frameBufferClearColor[1] = clearColor.normG();
            frameBufferClearColor[2] = clearColor.normB();
            frameBufferClearColor[3] = clearColor.normA();

            return true;
        }

        bool TextureResourceOGL::createTexture()
        {
            if (depthBufferId)
            {
                renderDeviceOGL->deleteRenderBuffer(depthBufferId);
                depthBufferId = 0;
            }

            if (frameBufferId)
            {
                renderDeviceOGL->deleteFrameBuffer(frameBufferId);
                frameBufferId = 0;
            }

            if (textureId)
            {
                renderDeviceOGL->deleteTexture(textureId);
                textureId = 0;
            }

            glGenTextures(1, &textureId);

            if (RenderDeviceOGL::checkOpenGLError())
            {
                Log(Log::Level::ERR) << "Failed to create texture";
                return false;
            }

            renderDeviceOGL->bindTexture(textureId, 0);

            width = static_cast<GLsizei>(size.width);
            height = static_cast<GLsizei>(size.height);

            oglInternalPixelFormat = getOGLInternalPixelFormat(pixelFormat, renderDeviceOGL->getAPIMajorVersion());

            if (oglInternalPixelFormat == GL_NONE)
            {
                Log(Log::Level::ERR) << "Invalid pixel format";
                return false;
            }

            oglPixelFormat = getOGLPixelFormat(pixelFormat);

            if (oglPixelFormat == GL_NONE)
            {
                Log(Log::Level::ERR) << "Invalid pixel format";
                return false;
            }

            oglPixelType = getOGLPixelType(pixelFormat);

            if (oglPixelType == GL_NONE)
            {
                Log(Log::Level::ERR) << "Invalid pixel format";
                return false;
            }

            if ((flags & Texture::RENDER_TARGET) && renderDeviceOGL->isRenderTargetsSupported())
            {
                glGenFramebuffersProc(1, &frameBufferId);

                if (RenderDeviceOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to create frame buffer";
                    return false;
                }

                renderDeviceOGL->bindFrameBuffer(frameBufferId);

                if (glCheckFramebufferStatusProc(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    renderDeviceOGL->bindTexture(textureId, 0);

                    glTexImage2D(GL_TEXTURE_2D, 0, oglInternalPixelFormat,
                                 width, height, 0,
                                 oglPixelFormat, oglPixelType, nullptr);

                    // TODO: blit multisample render buffer to texture
                    glFramebufferTexture2DProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

                    if (flags & Texture::DEPTH_BUFFER)
                    {
                        glGenRenderbuffersProc(1, &depthBufferId);
                        glBindRenderbufferProc(GL_RENDERBUFFER, depthBufferId);

                        if (sampleCount > 1 && renderDeviceOGL->isMultisamplingSupported())
                        {
                            glRenderbufferStorageMultisampleProc(GL_RENDERBUFFER,
                                                                 static_cast<GLsizei>(sampleCount),
                                                                 GL_DEPTH_COMPONENT,
                                                                 width, height);
                        }
                        else
                        {
                            glRenderbufferStorageProc(GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                                                      width, height);
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

            return true;
        }

        bool TextureResourceOGL::setTextureParameters()
        {
            renderDeviceOGL->bindTexture(textureId, 0);

            Texture::Filter finalFilter = (filter == Texture::Filter::DEFAULT) ? renderDeviceOGL->getTextureFilter() : filter;

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
                default:
                    return false;
            }

            if (RenderDeviceOGL::checkOpenGLError())
            {
                Log(Log::Level::ERR) << "Failed to set texture filter";
                return false;
            }

            switch (addressX)
            {
                case Texture::Address::CLAMP:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
                    break;
                case Texture::Address::REPEAT:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    break;
                case Texture::Address::MIRROR_REPEAT:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
                    break;
                default:
                    return false;
            }

            if (RenderDeviceOGL::checkOpenGLError())
            {
                Log(Log::Level::ERR) << "Failed to set texture wrap mode";
                return false;
            }

            switch (addressY)
            {
                case Texture::Address::CLAMP:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
                    break;
                case Texture::Address::REPEAT:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                    break;
                case Texture::Address::MIRROR_REPEAT:
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
                    break;
                default:
                    return false;
            }

            if (RenderDeviceOGL::checkOpenGLError())
            {
                Log(Log::Level::ERR) << "Failed to set texture wrap mode";
                return false;
            }

            uint32_t finalMaxAnisotropy = (maxAnisotropy == 0) ? renderDeviceOGL->getMaxAnisotropy() : maxAnisotropy;

            if (finalMaxAnisotropy > 1 && renderDeviceOGL->isAnisotropicFilteringSupported())
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<GLint>(finalMaxAnisotropy));

                if (RenderDeviceOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to set texture max anisotrophy";
                    return false;
                }
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
