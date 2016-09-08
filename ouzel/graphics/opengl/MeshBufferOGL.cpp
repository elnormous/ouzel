// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferOGL.h"
#include "core/CompileConfig.h"
#include "core/Engine.h"
#include "RendererOGL.h"
#include "IndexBufferOGL.h"
#include "VertexBufferOGL.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferOGL::MeshBufferOGL()
        {
        }

        MeshBufferOGL::~MeshBufferOGL()
        {
            if (vertexArrayId)
            {
                RendererOGL::deleteResource(vertexArrayId, RendererOGL::ResourceType::VertexArray);
            }
        }

        void MeshBufferOGL::free()
        {
            MeshBuffer::free();

            if (vertexArrayId)
            {
                RendererOGL::deleteResource(vertexArrayId, RendererOGL::ResourceType::VertexArray);
                vertexArrayId = 0;
            }
        }

        bool MeshBufferOGL::bindBuffers()
        {
            if (vertexArrayId)
            {
                if (!RendererOGL::bindVertexArray(vertexArrayId))
                {
                    return false;
                }
            }
            else
            {
                std::shared_ptr<IndexBufferOGL> indexBufferOGL = std::static_pointer_cast<IndexBufferOGL>(indexBuffer);

                if (!indexBufferOGL || !indexBufferOGL->bindBuffer())
                {
                    return false;
                }

                std::shared_ptr<VertexBufferOGL> vertexBufferOGL = std::static_pointer_cast<VertexBufferOGL>(vertexBuffer);

                if (!vertexBufferOGL || !vertexBufferOGL->bindBuffer())
                {
                    return false;
                }
            }
            
            return true;
        }

        bool MeshBufferOGL::upload()
        {
            if (dirty)
            {
                if (!indexBuffer->upload())
                {
                    return false;
                }

                if (!vertexBuffer->upload())
                {
                    return false;
                }

                if (!vertexArrayId)
                {
#if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
                    glGenVertexArraysOES(1, &vertexArrayId);
#elif OUZEL_PLATFORM_ANDROID || OUZEL_PLATFORM_RASPBIAN
                    if (genVertexArraysOES) genVertexArraysOES(1, &vertexArrayId);
#elif OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_LINUX
                    glGenVertexArrays(1, &vertexArrayId);
#endif
                }

                if (RendererOGL::checkOpenGLError())
                {
                    log(LOG_LEVEL_WARNING, "Failed to create vertex array");
                }

                if (vertexArrayId)
                {
                    RendererOGL::bindVertexArray(vertexArrayId);

                    std::shared_ptr<IndexBufferOGL> indexBufferOGL = std::static_pointer_cast<IndexBufferOGL>(indexBuffer);

                    if (indexBufferOGL && !indexBufferOGL->bindBuffer())
                    {
                        return false;
                    }

                    std::shared_ptr<VertexBufferOGL> vertexBufferOGL = std::static_pointer_cast<VertexBufferOGL>(vertexBuffer);

                    if (vertexBufferOGL && !vertexBufferOGL->bindBuffer())
                    {
                        return false;
                    }
                }

                ready = indexBuffer && vertexBuffer;
                dirty = false;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
