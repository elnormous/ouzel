// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferOGL.h"
#include "CompileConfig.h"
#include "Engine.h"
#include "RendererOGL.h"
#include "Utils.h"

namespace ouzel
{
    namespace graphics
    {
        MeshBufferOGL::MeshBufferOGL()
        {

        }

        MeshBufferOGL::~MeshBufferOGL()
        {
            free();
        }

        void MeshBufferOGL::free()
        {
            if (vertexArrayId)
            {
#if defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
                glDeleteVertexArraysOES(1, &vertexArrayId);
#else
                glDeleteVertexArrays(1, &vertexArrayId);
#endif
                vertexArrayId = 0;
            }

            if (vertexBufferId)
            {
                glDeleteBuffers(1, &vertexBufferId);
                vertexBufferId = 0;
            }

            if (indexBufferId)
            {
                glDeleteBuffers(1, &indexBufferId);
                indexBufferId = 0;
            }
        }

        bool MeshBufferOGL::init()
        {
            glGenBuffers(1, &indexBufferId);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);

#if defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
            glGenVertexArraysOES(1, &vertexArrayId);
            glBindVertexArrayOES(vertexArrayId);
#else
            glGenVertexArrays(1, &vertexArrayId);
            glBindVertexArray(vertexArrayId);
#endif
            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors(false))
            {
                vertexArrayId = 0;
                log("Failed to create vertex array");
            }

            glGenBuffers(1, &vertexBufferId);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
            {
                return false;
            }

            return true;
        }

        bool MeshBufferOGL::initFromData(const void* newIndices, uint32_t newIndexSize,
                                         uint32_t newIndexCount, bool newDynamicIndexBuffer,
                                         const void* newVertices, uint32_t newVertexAttributes,
                                         uint32_t newVertexCount, bool newDynamicVertexBuffer)
        {
            if (!MeshBuffer::initFromData(newIndices, newIndexSize, newIndexCount, newDynamicIndexBuffer, newVertices, newVertexAttributes, newVertexCount, newDynamicVertexBuffer))
            {
                return false;
            }

            free();

            glGenBuffers(1, &indexBufferId);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * indexCount, newIndices,
                         dynamicIndexBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
            {
                return false;
            }

            if (!updateIndexFormat())
            {
                return false;
            }

#if defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
            glGenVertexArraysOES(1, &vertexArrayId);
            glBindVertexArrayOES(vertexArrayId);
#else
            glGenVertexArrays(1, &vertexArrayId);
            glBindVertexArray(vertexArrayId);
#endif
            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors(false))
            {
                vertexArrayId = 0;
            }

            glGenBuffers(1, &vertexBufferId);
            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
            glBufferData(GL_ARRAY_BUFFER, vertexSize * vertexCount, newVertices,
                         dynamicVertexBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
            {
                return false;
            }

            if (!updateVertexAttributes())
            {
                return false;
            }

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

            if (vertexArrayId)
            {
#if defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
                glBindVertexArrayOES(vertexArrayId);
#else
                glBindVertexArray(vertexArrayId);
#endif

                glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);

                updateVertexAttributes();
            }

            return true;
        }

        bool MeshBufferOGL::uploadIndices(const void* indices, uint32_t indexCount)
        {
            if (!MeshBuffer::uploadIndices(indices, indexCount))
            {
                return false;
            }

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferId);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * indexCount, indices,
                         dynamicIndexBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

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

            glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
            glBufferData(GL_ARRAY_BUFFER, vertexSize * vertexCount, vertices,
                         dynamicVertexBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

            if (std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer())->checkOpenGLErrors())
            {
                return false;
            }

            return true;
        }

        bool MeshBufferOGL::updateIndexFormat()
        {
            switch (indexSize)
            {
                case 1: indexFormat = GL_UNSIGNED_BYTE; break;
                case 2: indexFormat = GL_UNSIGNED_SHORT; break;
                case 4: indexFormat = GL_UNSIGNED_INT; break;
                default: log("Invalid index size"); return false;
            }

            return true;
        }

        bool MeshBufferOGL::bindVertexBuffer()
        {
            if (vertexArrayId)
            {
#if defined(OUZEL_PLATFORM_IOS) || defined(OUZEL_PLATFORM_TVOS)
                glBindVertexArrayOES(vertexArrayId);
#else
                glBindVertexArray(vertexArrayId);
#endif
            }
            else
            {
                glBindBuffer(GL_ARRAY_BUFFER, vertexBufferId);
                if (!updateVertexAttributes())
                {
                    return false;
                }
            }

            return true;
        }

        bool MeshBufferOGL::updateVertexAttributes()
        {
            GLuint index = 0;
            GLuint offset = 0;

            if (vertexAttributes & VERTEX_POSITION)
            {
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, static_cast<GLint>(vertexSize), reinterpret_cast<const GLvoid*>(offset));
                offset += 3 * sizeof(float);
                ++index;
            }

            if (vertexAttributes & VERTEX_COLOR)
            {
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index, 4, GL_UNSIGNED_BYTE, GL_TRUE, static_cast<GLint>(vertexSize), reinterpret_cast<const GLvoid*>(offset));
                offset += 4 * sizeof(uint8_t);
                ++index;
            }

            if (vertexAttributes & VERTEX_NORMAL)
            {
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index, 3, GL_FLOAT, GL_FALSE, static_cast<GLint>(vertexSize), reinterpret_cast<const GLvoid*>(offset));
                offset += 3 * sizeof(float);
                ++index;
            }

            if (vertexAttributes & VERTEX_TEXCOORD0)
            {
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, static_cast<GLint>(vertexSize), reinterpret_cast<const GLvoid*>(offset));
                offset += 2 * sizeof(float);
                ++index;
            }

            if (vertexAttributes & VERTEX_TEXCOORD1)
            {
                glEnableVertexAttribArray(index);
                glVertexAttribPointer(index, 2, GL_FLOAT, GL_FALSE, static_cast<GLint>(vertexSize), reinterpret_cast<const GLvoid*>(offset));
                offset += 2 * sizeof(float);
                ++index;
            }

            for (GLuint unusedIndex = index; unusedIndex < 5; ++unusedIndex)
            {
                glDisableVertexAttribArray(unusedIndex);
            }

            if (offset != vertexSize)
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
    } // namespace graphics
} // namespace ouzel
