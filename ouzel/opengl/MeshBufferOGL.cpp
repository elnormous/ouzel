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
        MeshBufferOGL::MeshBufferOGL():
            indexBufferDirty(true), vertexBufferDirty(true)
        {

        }

        MeshBufferOGL::~MeshBufferOGL()
        {
            std::shared_ptr<RendererOGL> rendererOGL = std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer());

            if (vertexArrayId)
            {
                rendererOGL->deleteResource(vertexArrayId, RendererOGL::ResourceType::VertexArray);
            }

            if (vertexBufferId)
            {
                rendererOGL->deleteResource(vertexBufferId, RendererOGL::ResourceType::Buffer);
            }

            if (indexBufferId)
            {
                rendererOGL->deleteResource(indexBufferId, RendererOGL::ResourceType::Buffer);
            }
        }

        void MeshBufferOGL::free()
        {
            MeshBuffer::free();

            std::shared_ptr<RendererOGL> rendererOGL = std::static_pointer_cast<RendererOGL>(sharedEngine->getRenderer());

            if (vertexArrayId)
            {
                rendererOGL->deleteResource(vertexArrayId, RendererOGL::ResourceType::VertexArray);
                vertexArrayId = 0;
            }

            if (vertexBufferId)
            {
                rendererOGL->deleteResource(vertexBufferId, RendererOGL::ResourceType::Buffer);
                vertexBufferId = 0;
            }

            if (indexBufferId)
            {
                rendererOGL->deleteResource(indexBufferId, RendererOGL::ResourceType::Buffer);
                indexBufferId = 0;
            }
        }

        bool MeshBufferOGL::init(bool newDynamicIndexBuffer, bool newDynamicVertexBuffer)
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            
            if (!MeshBuffer::init(newDynamicIndexBuffer, newDynamicVertexBuffer))
            {
                return false;
            }

            ready = true;

            return true;
        }

        bool MeshBufferOGL::initFromBuffer(const void* newIndices, uint32_t newIndexSize,
                                           uint32_t newIndexCount, bool newDynamicIndexBuffer,
                                           const void* newVertices, uint32_t newVertexAttributes,
                                           uint32_t newVertexCount, bool newDynamicVertexBuffer)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::initFromBuffer(newIndices, newIndexSize, newIndexCount, newDynamicIndexBuffer, newVertices, newVertexAttributes, newVertexCount, newDynamicVertexBuffer))
            {
                return false;
            }

            indexData.assign(static_cast<const uint8_t*>(newIndices),
                             static_cast<const uint8_t*>(newIndices) + indexSize * indexCount);


            vertexData.assign(static_cast<const uint8_t*>(newVertices),
                              static_cast<const uint8_t*>(newVertices) + vertexSize * vertexCount);

            ready = true;

            return true;
        }

        bool MeshBufferOGL::setIndexSize(uint32_t indexSize)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::setIndexSize(indexSize))
            {
                return false;
            }

            return updateIndexFormat();
        }

        bool MeshBufferOGL::setVertexAttributes(uint32_t vertexAttributes)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::setVertexAttributes(vertexAttributes))
            {
                return false;
            }

            vertexBufferDirty = true;

            return true;
        }

        bool MeshBufferOGL::uploadIndices(const void* indices, uint32_t indexCount)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::uploadIndices(indices, indexCount))
            {
                return false;
            }

            indexData.assign(static_cast<const uint8_t*>(indices),
                             static_cast<const uint8_t*>(indices) + indexSize * indexCount);

            indexBufferDirty = true;

            return true;
        }

        bool MeshBufferOGL::uploadVertices(const void* vertices, uint32_t vertexCount)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::uploadVertices(vertices, vertexCount))
            {
                return false;
            }

            vertexData.assign(static_cast<const uint8_t*>(vertices),
                              static_cast<const uint8_t*>(vertices) + vertexSize * vertexCount);

            vertexBufferDirty = true;

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
                if (!RendererOGL::bindVertexArray(vertexArrayId))
                {
                    return false;
                }
            }
            else
            {
                if (!RendererOGL::bindArrayBuffer(vertexBufferId))
                {
                    return false;
                }

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

            if (RendererOGL::checkOpenGLError())
            {
                log("Failed to update vertex attributes");
                return false;
            }

            return true;
        }

        bool MeshBufferOGL::update()
        {
            if (indexBufferDirty || vertexBufferDirty)
            {
                std::lock_guard<std::mutex> lock(dataMutex);

                if (indexBufferDirty)
                {
                    if (!indexBufferId)
                    {
                        glGenBuffers(1, &indexBufferId);

                        if (RendererOGL::checkOpenGLError())
                        {
                            log("Failed to create index buffer");
                            return false;
                        }
                        
                        if (!updateIndexFormat())
                        {
                            return false;
                        }
                    }

                    if (!indexData.empty())
                    {
                        RendererOGL::bindElementArrayBuffer(indexBufferId);
                        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize * indexCount, indexData.data(),
                                     dynamicIndexBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                        if (RendererOGL::checkOpenGLError())
                        {
                            log("Failed to upload index data");
                            return false;
                        }

                        // unbind so that it gets bind again right before glDrawElements
                        RendererOGL::unbindElementArrayBuffer(indexBufferId);
                    }

                    indexBufferDirty = false;
                }

                if (vertexBufferDirty)
                {
                    if (!vertexBufferId)
                    {
#if OUZEL_PLATFORM_IOS || OUZEL_PLATFORM_TVOS
                        glGenVertexArraysOES(1, &vertexArrayId);
#elif OUZEL_PLATFORM_ANDROID || OUZEL_PLATFORM_RASPBIAN
                        if (glGenVertexArraysOESEXT) glGenVertexArraysOESEXT(1, &vertexArrayId);
#else
                        glGenVertexArrays(1, &vertexArrayId);
#endif

                        if (RendererOGL::checkOpenGLError(false))
                        {
                            log("Failed to create vertex array");
                            vertexArrayId = 0;
                        }
                        else
                        {
                            RendererOGL::bindVertexArray(vertexArrayId);
                        }

                        glGenBuffers(1, &vertexBufferId);

                        if (RendererOGL::checkOpenGLError())
                        {
                            log("Failed to create vertex buffer");
                            return false;
                        }
                    }

                    if (!vertexData.empty())
                    {
                        if (vertexArrayId)
                        {
                            RendererOGL::bindVertexArray(vertexArrayId);
                            RendererOGL::bindArrayBuffer(vertexBufferId);

                            updateVertexAttributes();
                        }

                        RendererOGL::bindArrayBuffer(vertexBufferId);
                        glBufferData(GL_ARRAY_BUFFER, vertexSize * vertexCount, vertexData.data(),
                                     dynamicVertexBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

                        if (RendererOGL::checkOpenGLError())
                        {
                            log("Failed to create vertex data");
                            return false;
                        }

                        // unbind so that it gets bind again right before glDrawElements
                        RendererOGL::unbindArrayBuffer(vertexBufferId);
                    }

                    vertexBufferDirty = false;
                }
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
