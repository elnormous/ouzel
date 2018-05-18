// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Setup.h"

#if OUZEL_COMPILE_OPENGL

#include "MeshBufferResourceOGL.hpp"
#include "RenderDeviceOGL.hpp"
#include "BufferResourceOGL.hpp"
#include "utils/Log.hpp"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferResourceOGL::MeshBufferResourceOGL(RenderDeviceOGL& initRenderDeviceOGL):
            renderDeviceOGL(initRenderDeviceOGL)
        {
        }

        MeshBufferResourceOGL::~MeshBufferResourceOGL()
        {
        }

        bool MeshBufferResourceOGL::init(uint32_t newIndexSize, BufferResource* newIndexBuffer,
                                         BufferResource* newVertexBuffer)
        {
            if (!MeshBufferResource::init(newIndexSize, newIndexBuffer, newVertexBuffer))
                return false;

            switch (indexSize)
            {
                case 2:
                    indexType = GL_UNSIGNED_SHORT;
                    bytesPerIndex = 2;
                    break;
                case 4:
                    indexType = GL_UNSIGNED_INT;
                    bytesPerIndex = 4;
                    break;
                default:
                    indexType = 0;
                    bytesPerIndex = 0;
                    Log(Log::Level::ERR) << "Invalid index size";
                    return false;
            }

            indexBufferOGL = static_cast<BufferResourceOGL*>(indexBuffer);
            vertexBufferOGL = static_cast<BufferResourceOGL*>(vertexBuffer);

            return true;
        }

        bool MeshBufferResourceOGL::setIndexSize(uint32_t newIndexSize)
        {
            if (MeshBufferResource::setIndexSize(newIndexSize))
                return false;

            switch (indexSize)
            {
                case 2:
                    indexType = GL_UNSIGNED_SHORT;
                    bytesPerIndex = 2;
                    break;
                case 4:
                    indexType = GL_UNSIGNED_INT;
                    bytesPerIndex = 4;
                    break;
                default:
                    indexType = 0;
                    bytesPerIndex = 0;
                    Log(Log::Level::ERR) << "Invalid index size";
                    return false;
            }

            return true;
        }

        bool MeshBufferResourceOGL::setIndexBuffer(BufferResource* newIndexBuffer)
        {
            if (MeshBufferResource::setIndexBuffer(newIndexBuffer))
                return false;

            indexBufferOGL = static_cast<BufferResourceOGL*>(indexBuffer);

            return true;
        }

        bool MeshBufferResourceOGL::setVertexBuffer(BufferResource* newVertexBuffer)
        {
            if (MeshBufferResource::setVertexBuffer(newVertexBuffer))
                return false;

            vertexBufferOGL = static_cast<BufferResourceOGL*>(vertexBuffer);

            return true;
        }
    } // namespace graphics
} // namespace ouzel

#endif
