// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferOGL.h"
#include "Engine.h"
#include "RendererOGL.h"
#include "Utils.h"

namespace ouzel
{
    namespace video
    {
        MeshBufferOGL::MeshBufferOGL()
        {

        }

        MeshBufferOGL::~MeshBufferOGL()
        {
            destroy();
        }

        void MeshBufferOGL::destroy()
        {
            if (_vertexArrayId) glDeleteVertexArrays(1, &_vertexArrayId);
            if (_vertexBufferId) glDeleteBuffers(1, &_vertexBufferId);
            if (_indexBufferId) glDeleteBuffers(1, &_indexBufferId);
        }

        bool MeshBufferOGL::initFromData(const void* indices, uint32_t indexSize, uint32_t indexCount, bool dynamicIndexBuffer, const void* vertices, uint32_t vertexAttributes, uint32_t vertexCount, bool dynamicVertexBuffer)
        {
            if (!MeshBuffer::initFromData(indices, indexSize, indexCount, dynamicIndexBuffer, vertices, vertexAttributes, vertexCount, dynamicVertexBuffer))
            {
                return false;
            }

            destroy();

            glGenBuffers(1, &_indexBufferId);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * indexCount, indices,
                         _dynamicIndexBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
            {
                return false;
            }

            if (!updateIndexFormat())
            {
                return false;
            }

            glGenVertexArrays(1, &_vertexArrayId);
            glBindVertexArray(_vertexArrayId);

            glGenBuffers(1, &_vertexBufferId);
            glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
            glBufferData(GL_ARRAY_BUFFER, _vertexSize * vertexCount, vertices,
                         _dynamicVertexBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
            {
                return false;
            }

            if (!updateVertexAttributes())
            {
                return false;
            }

            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            return true;
        }


        bool MeshBufferOGL::setIndexSize(uint32_t indexSize)
        {
            if (!MeshBuffer::setIndexSize(indexSize))
            {
                return false;
            }

            return updateIndexFormat();
        }

        bool MeshBufferOGL::setVertexAttributes(uint32_t vertexAttributes)
        {
            if (!MeshBuffer::setVertexAttributes(vertexAttributes))
            {
                return false;
            }

            return updateVertexAttributes();
        }

        bool MeshBufferOGL::uploadIndices(const void* indices, uint32_t indexCount)
        {
            if (!MeshBuffer::uploadIndices(indices, indexCount))
            {
                return false;
            }

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indexSize * indexCount, indices,
                         _dynamicIndexBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
            {
                return false;
            }

            return true;
        }

        bool MeshBufferOGL::uploadVertices(const void* vertices, uint32_t vertexCount)
        {
            if (!MeshBuffer::uploadVertices(vertices, vertexCount))
            {
                return false;
            }

            glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
            glBufferData(GL_ARRAY_BUFFER, _vertexSize * vertexCount, vertices,
                         _dynamicVertexBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
            {
                return false;
            }

            return true;
        }

        bool MeshBufferOGL::updateIndexFormat()
        {
            switch (_indexSize)
            {
                case 1: _indexFormat = GL_UNSIGNED_BYTE; break;
                case 2: _indexFormat = GL_UNSIGNED_SHORT; break;
                case 4: _indexFormat = GL_UNSIGNED_INT; break;
                default: log("Invalid index size"); return false;
            }

            return true;
        }

        bool MeshBufferOGL::updateVertexAttributes()
        {
            glBindVertexArray(_vertexArrayId);

            GLuint index = 0;
            GLuint offset = 0;

            if (_vertexAttributes & VERTEX_POSITION)
            {
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, static_cast<GLint>(_vertexSize), reinterpret_cast<const GLvoid*>(offset));
                offset += 3 * sizeof(float);
                index++;
            }

            if (_vertexAttributes & VERTEX_COLOR)
            {
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index, 4, GL_UNSIGNED_BYTE, GL_TRUE, static_cast<GLint>(_vertexSize), reinterpret_cast<const GLvoid*>(offset));
                offset += 4 * sizeof(uint8_t);
                index++;
            }

            if (_vertexAttributes & VERTEX_NORMAL)
            {
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, static_cast<GLint>(_vertexSize), reinterpret_cast<const GLvoid*>(offset));
                offset += 3 * sizeof(float);
                index++;
            }

            if (_vertexAttributes & VERTEX_TEXCOORD0)
            {
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, static_cast<GLint>(_vertexSize), reinterpret_cast<const GLvoid*>(offset));
                offset += 2 * sizeof(float);
                index++;
            }

            if (_vertexAttributes & VERTEX_TEXCOORD1)
            {
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, static_cast<GLint>(_vertexSize), reinterpret_cast<const GLvoid*>(offset));
                offset += 2 * sizeof(float);
                index++;
            }

            if (offset != _vertexSize)
            {
                log("Invalid vertex size");
                return false;
            }

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
            {
                return false;
            }

            return true;
        }

    } // namespace video
} // namespace ouzel
