// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#if OUZEL_SUPPORTS_OPENGLES
#define GL_GLEXT_PROTOTYPES 1
#include "GLES/gl.h"
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#include "GLES3/gl3.h"
#else
#define GL_GLEXT_PROTOTYPES 1
#include "GL/glcorearb.h"
#include "GL/glext.h"
#endif

#include "graphics/BufferResource.hpp"

namespace ouzel
{
    namespace graphics
    {
        class RenderDeviceOGL;

        class BufferResourceOGL: public BufferResource
        {
        public:
            explicit BufferResourceOGL(RenderDeviceOGL& initRenderDeviceOGL);
            virtual ~BufferResourceOGL();

            virtual bool init(Buffer::Usage newUsage, uint32_t newFlags,
                              const std::vector<uint8_t>& newData,
                              uint32_t newSize) override;

            virtual bool reload() override;

            virtual bool setData(const std::vector<uint8_t>& newData) override;

            inline GLuint getBufferId() const { return bufferId; }
            inline GLuint getBufferType() const { return bufferType; }

        private:
            bool createBuffer();

            RenderDeviceOGL& renderDeviceOGL;

            GLuint bufferId = 0;
            GLsizeiptr bufferSize = 0;

            GLuint bufferType = 0;
        };
    } // namespace graphics
} // namespace ouzel

#endif
