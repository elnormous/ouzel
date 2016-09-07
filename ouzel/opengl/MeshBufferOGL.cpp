// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferOGL.h"
#include "core/CompileConfig.h"
#include "core/Engine.h"
#include "RendererOGL.h"
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

            if (vertexBufferId)
            {
                RendererOGL::deleteResource(vertexBufferId, RendererOGL::ResourceType::Buffer);
            }

            if (indexBufferId)
            {
                RendererOGL::deleteResource(indexBufferId, RendererOGL::ResourceType::Buffer);
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

            if (vertexBufferId)
            {
                RendererOGL::deleteResource(vertexBufferId, RendererOGL::ResourceType::Buffer);
                vertexBufferId = 0;
            }

            if (indexBufferId)
            {
                RendererOGL::deleteResource(indexBufferId, RendererOGL::ResourceType::Buffer);
                indexBufferId = 0;
            }
        }
    } // namespace graphics
} // namespace ouzel
