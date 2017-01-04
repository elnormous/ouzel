// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferOGL.h"
#include "core/CompileConfig.h"
#include "core/Engine.h"
#include "RendererOGL.h"
#include "IndexBufferOGL.h"
#include "VertexBufferOGL.h"
#include "utils/Log.h"

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
                std::shared_ptr<IndexBufferOGL> indexBufferOGL = std::static_pointer_cast<IndexBufferOGL>(uploadData.indexBuffer);

                if (!indexBufferOGL || !indexBufferOGL->bindBuffer())
                {
                    return false;
                }

                std::shared_ptr<VertexBufferOGL> vertexBufferOGL = std::static_pointer_cast<VertexBufferOGL>(uploadData.vertexBuffer);

                if (!vertexBufferOGL || !vertexBufferOGL->bindBuffer())
                {
                    return false;
                }
            }

            return true;
        }

        bool MeshBufferOGL::upload()
        {
            if (!MeshBuffer::upload())
            {
                return false;
            }

            if (!vertexArrayId)
            {
#if OUZEL_OPENGL_INTERFACE_EAGL
                glGenVertexArraysOES(1, &vertexArrayId);
#elif OUZEL_OPENGL_INTERFACE_EGL
                if (genVertexArraysOES) genVertexArraysOES(1, &vertexArrayId);
#elif OUZEL_PLATFORM_MACOS || OUZEL_PLATFORM_LINUX
                glGenVertexArrays(1, &vertexArrayId);
#endif
            }

            if (RendererOGL::checkOpenGLError())
            {
                Log(Log::Level::WARN) << "Failed to create vertex array";
            }

            std::shared_ptr<IndexBufferOGL> indexBufferOGL = std::static_pointer_cast<IndexBufferOGL>(uploadData.indexBuffer);

            if (indexBufferOGL && indexBufferOGL->dirty && !indexBufferOGL->upload())
            {
                return false;
            }

            std::shared_ptr<VertexBufferOGL> vertexBufferOGL = std::static_pointer_cast<VertexBufferOGL>(uploadData.vertexBuffer);

            if (vertexBufferOGL && vertexBufferOGL->dirty && !vertexBufferOGL->upload())
            {
                return false;
            }

            if (vertexArrayId)
            {
                RendererOGL::bindVertexArray(vertexArrayId);

                if (indexBufferOGL && !indexBufferOGL->bindBuffer())
                {
                    return false;
                }

                if (vertexBufferOGL && !vertexBufferOGL->bindBuffer())
                {
                    return false;
                }
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
