// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/CompileConfig.h"

#if OUZEL_SUPPORTS_OPENGL

#include "TextureInterfaceOGL.h"
#include "core/Engine.h"
#include "RendererOGL.h"
#include "utils/Log.h"

namespace ouzel
{
    namespace graphics
    {
        static GLint getOGLInternalPixelFormat(PixelFormat pixelFormat)
        {
#if OUZEL_SUPPORTS_OPENGLES
            uint32_t openGLVersion = sharedEngine->getRenderer()->getAPIMajorVersion();

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

        TextureInterfaceOGL::TextureInterfaceOGL()
        {
        }

        TextureInterfaceOGL::~TextureInterfaceOGL()
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

        bool TextureInterfaceOGL::init(const Size2& newSize,
                                       uint32_t newFlags,
                                       uint32_t newMipmaps,
                                       uint32_t newSampleCount,
                                       PixelFormat newPixelFormat)
        {
            if (!TextureInterface::init(newSize,
                                        newFlags,
                                        newMipmaps,
                                        newSampleCount,
                                        newPixelFormat))
            {
                return false;
            }

            if (!textureId)
            {
                glGenTextures(1, &textureId);

                if (RendererOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to create texture";
                    return false;
                }
            }

            if (!updateTexture())
            {
                return false;
            }

            RendererOGL* rendererOGL = static_cast<RendererOGL*>(sharedEngine->getRenderer());
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

            if (RendererOGL::checkOpenGLError())
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
            }

            if (RendererOGL::checkOpenGLError())
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
            }

            if (RendererOGL::checkOpenGLError())
            {
                Log(Log::Level::ERR) << "Failed to set texture wrap mode";
                return false;
            }

            uint32_t finalMaxAnisotropy = (maxAnisotropy == 0) ? rendererOGL->getMaxAnisotropy() : maxAnisotropy;

            if (finalMaxAnisotropy > 1 && rendererOGL->isAnisotropicFilteringSupported())
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<GLint>(finalMaxAnisotropy));

                if (RendererOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to set texture max anisotrophy";
                    return false;
                }
            }

            clearMask = 0;
            if (clearColorBuffer) clearMask |= GL_COLOR_BUFFER_BIT;
            if (clearDepthBuffer) clearMask |= GL_DEPTH_BUFFER_BIT;

            frameBufferClearColor[0] = clearColor.normR();
            frameBufferClearColor[1] = clearColor.normG();
            frameBufferClearColor[2] = clearColor.normB();
            frameBufferClearColor[3] = clearColor.normA();

            return true;
        }

        bool TextureInterfaceOGL::init(const std::vector<uint8_t>& newData,
                                       const Size2& newSize,
                                       uint32_t newFlags,
                                       uint32_t newMipmaps,
                                       PixelFormat newPixelFormat)
        {
            if (!TextureInterface::init(newData,
                                        newSize,
                                        newFlags,
                                        newMipmaps,
                                        newPixelFormat))
            {
                return false;
            }

            if (!textureId)
            {
                glGenTextures(1, &textureId);

                if (RendererOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to create texture";
                    return false;
                }
            }

            if (!updateTexture())
            {
                return false;
            }

            RendererOGL* rendererOGL = static_cast<RendererOGL*>(sharedEngine->getRenderer());
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

            if (RendererOGL::checkOpenGLError())
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
            }

            if (RendererOGL::checkOpenGLError())
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
            }

            if (RendererOGL::checkOpenGLError())
            {
                Log(Log::Level::ERR) << "Failed to set texture wrap mode";
                return false;
            }

            uint32_t finalMaxAnisotropy = (maxAnisotropy == 0) ? rendererOGL->getMaxAnisotropy() : maxAnisotropy;

            if (finalMaxAnisotropy > 1 && rendererOGL->isAnisotropicFilteringSupported())
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<GLint>(finalMaxAnisotropy));

                if (RendererOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to set texture max anisotrophy";
                    return false;
                }
            }

            return true;
        }

        bool TextureInterfaceOGL::setSize(const Size2& newSize)
        {
            if (!TextureInterface::setSize(newSize))
            {
                return false;
            }

            if (!textureId)
            {
                Log(Log::Level::ERR) << "Texture not initialized";
                return false;
            }

            return updateTexture();
        }

        bool TextureInterfaceOGL::setData(const std::vector<uint8_t>& newData, const Size2& newSize)
        {
            if (!TextureInterface::setData(newData, newSize))
            {
                return false;
            }

            if (!textureId)
            {
                Log(Log::Level::ERR) << "Texture not initialized";
                return false;
            }

            return updateTexture();
        }

        bool TextureInterfaceOGL::setFilter(Texture::Filter newFilter)
        {
            if (!TextureInterface::setFilter(newFilter))
            {
                return false;
            }

            if (!textureId)
            {
                Log(Log::Level::ERR) << "Texture not initialized";
                return false;
            }

            RendererOGL* rendererOGL = static_cast<RendererOGL*>(sharedEngine->getRenderer());
            Texture::Filter finalFilter = (filter == Texture::Filter::DEFAULT) ? rendererOGL->getTextureFilter() : filter;

            RendererOGL::bindTexture(textureId, 0);

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

            if (RendererOGL::checkOpenGLError())
            {
                Log(Log::Level::ERR) << "Failed to set texture filter";
                return false;
            }

            return true;
        }

        bool TextureInterfaceOGL::setAddressX(Texture::Address newAddressX)
        {
            if (!TextureInterface::setAddressX(newAddressX))
            {
                return false;
            }

            if (!textureId)
            {
                Log(Log::Level::ERR) << "Texture not initialized";
                return false;
            }

            RendererOGL::bindTexture(textureId, 0);

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
            }

            if (RendererOGL::checkOpenGLError())
            {
                Log(Log::Level::ERR) << "Failed to set texture wrap mode";
                return false;
            }

            return true;
        }

        bool TextureInterfaceOGL::setAddressY(Texture::Address newAddressY)
        {
            if (!TextureInterface::setAddressY(newAddressY))
            {
                return false;
            }

            if (!textureId)
            {
                Log(Log::Level::ERR) << "Texture not initialized";
                return false;
            }

            RendererOGL::bindTexture(textureId, 0);

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
            }

            if (RendererOGL::checkOpenGLError())
            {
                Log(Log::Level::ERR) << "Failed to set texture wrap mode";
                return false;
            }

            return true;
        }

        bool TextureInterfaceOGL::setMaxAnisotropy(uint32_t newMaxAnisotropy)
        {
            if (!TextureInterface::setMaxAnisotropy(newMaxAnisotropy))
            {
                return false;
            }

            if (!textureId)
            {
                Log(Log::Level::ERR) << "Texture not initialized";
                return false;
            }

            RendererOGL::bindTexture(textureId, 0);

            RendererOGL* rendererOGL = static_cast<RendererOGL*>(sharedEngine->getRenderer());
            uint32_t finalMaxAnisotropy = (maxAnisotropy == 0) ? rendererOGL->getMaxAnisotropy() : maxAnisotropy;

            if (finalMaxAnisotropy > 1 && rendererOGL->isAnisotropicFilteringSupported())
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, static_cast<GLint>(finalMaxAnisotropy));

                if (RendererOGL::checkOpenGLError())
                {
                    Log(Log::Level::ERR) << "Failed to set texture max anisotrophy";
                    return false;
                }
            }

            return true;
        }

        bool TextureInterfaceOGL::setClearColorBuffer(bool clear)
        {
            if (!TextureInterface::setClearColorBuffer(clear))
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

        bool TextureInterfaceOGL::setClearDepthBuffer(bool clear)
        {
            if (!TextureInterface::setClearDepthBuffer(clear))
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

        bool TextureInterfaceOGL::setClearColor(Color color)
        {
            if (!TextureInterface::setClearColor(color))
            {
                return false;
            }

            frameBufferClearColor[0] = clearColor.normR();
            frameBufferClearColor[1] = clearColor.normG();
            frameBufferClearColor[2] = clearColor.normB();
            frameBufferClearColor[3] = clearColor.normA();

            return true;
        }

        bool TextureInterfaceOGL::updateTexture()
        {
            RendererOGL::bindTexture(textureId, 0);

            RendererOGL* rendererOGL = static_cast<RendererOGL*>(sharedEngine->getRenderer());

            GLsizei newWidth = static_cast<GLsizei>(size.v[0]);
            GLsizei newHeight = static_cast<GLsizei>(size.v[1]);

            GLint oglInternalPixelFormat = getOGLInternalPixelFormat(pixelFormat);

            if (oglInternalPixelFormat == GL_NONE)
            {
                Log(Log::Level::ERR) << "Invalid pixel format";
                return false;
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, static_cast<GLsizei>(levels.size()) - 1);

            if (newWidth != width ||
                newHeight != height)
            {
                width = newWidth;
                height = newHeight;

                if (!(flags & Texture::RENDER_TARGET))
                {
                    for (size_t level = 0; level < levels.size(); ++level)
                    {
                        if (!levels[level].data.empty())
                        {
                            glTexImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), oglInternalPixelFormat,
                                         static_cast<GLsizei>(levels[level].size.v[0]),
                                         static_cast<GLsizei>(levels[level].size.v[1]), 0,
                                         getOGLPixelFormat(pixelFormat), getOGLPixelType(pixelFormat),
                                         levels[level].data.data());
                        }
                    }
                }

                if ((flags & Texture::RENDER_TARGET) && rendererOGL->isRenderTargetsSupported())
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

                        glTexImage2D(GL_TEXTURE_2D, 0, oglInternalPixelFormat,
                                     static_cast<GLsizei>(size.v[0]),
                                     static_cast<GLsizei>(size.v[1]), 0,
                                     getOGLPixelFormat(pixelFormat), getOGLPixelType(pixelFormat), 0);

                        // TODO: blit multisample render buffer to texture
                        glFramebufferTexture2DProc(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);

                        if (flags & Texture::DEPTH_BUFFER)
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
            else if (!(flags & Texture::RENDER_TARGET))
            {
                for (size_t level = 0; level < levels.size(); ++level)
                {
                    if (!levels[level].data.empty())
                    {
                        glTexSubImage2D(GL_TEXTURE_2D, static_cast<GLint>(level), 0, 0,
                                        static_cast<GLsizei>(levels[level].size.v[0]),
                                        static_cast<GLsizei>(levels[level].size.v[1]),
                                        getOGLPixelFormat(pixelFormat), getOGLPixelType(pixelFormat),
                                        levels[level].data.data());

                        if (RendererOGL::checkOpenGLError())
                        {
                            Log(Log::Level::ERR) << "Failed to upload texture data";
                            return false;
                        }
                    }
                }
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
