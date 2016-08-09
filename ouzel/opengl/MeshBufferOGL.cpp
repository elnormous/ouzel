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
            indexBufferDirty(false), vertexBufferDirty(false)
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
            std::lock_guard<std::mutex> lock(dataMutex);

            MeshBuffer::free();

            indexData.clear();
            vertexData.clear();

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

        bool MeshBufferOGL::init(bool newDynamicIndexBuffer, bool newDynamicVertexBuffer)
        {
            free();

            std::lock_guard<std::mutex> lock(dataMutex);
            
            if (!MeshBuffer::init(newDynamicIndexBuffer, newDynamicVertexBuffer))
            {
                return false;
            }

            indexBufferDirty = true;
            vertexBufferDirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool MeshBufferOGL::initFromBuffer(const void* newIndices, uint32_t newIndexSize,
                                           uint32_t newIndexCount, bool newDynamicIndexBuffer,
                                           const void* newVertices, uint32_t newVertexAttributes,
                                           uint32_t newVertexCount, bool newDynamicVertexBuffer)
        {
            free();
            
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::initFromBuffer(newIndices, newIndexSize, newIndexCount, newDynamicIndexBuffer, newVertices, newVertexAttributes, newVertexCount, newDynamicVertexBuffer))
            {
                return false;
            }

            if (newIndices && indexSize && indexCount)
            {
                indexData.assign(static_cast<const uint8_t*>(newIndices),
                                 static_cast<const uint8_t*>(newIndices) + indexSize * indexCount);
            }

            if (newVertices && vertexSize && vertexCount)
            {
                vertexData.assign(static_cast<const uint8_t*>(newVertices),
                                  static_cast<const uint8_t*>(newVertices) + vertexSize * vertexCount);
            }


            indexBufferDirty = true;
            vertexBufferDirty = true;
            
            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool MeshBufferOGL::setIndexSize(uint32_t indexSize)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::setIndexSize(indexSize))
            {
                return false;
            }

            indexBufferDirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool MeshBufferOGL::setVertexAttributes(uint32_t vertexAttributes)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::setVertexAttributes(vertexAttributes))
            {
                return false;
            }

            vertexBufferDirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool MeshBufferOGL::uploadIndices(const void* newIndices, uint32_t newIndexCount)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::uploadIndices(newIndices, newIndexCount))
            {
                return false;
            }

            indexData.assign(static_cast<const uint8_t*>(newIndices),
                             static_cast<const uint8_t*>(newIndices) + indexSize * indexCount);

            indexBufferDirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

            return true;
        }

        bool MeshBufferOGL::uploadVertices(const void* newVertices, uint32_t newVertexCount)
        {
            std::lock_guard<std::mutex> lock(dataMutex);

            if (!MeshBuffer::uploadVertices(newVertices, newVertexCount))
            {
                return false;
            }

            vertexData.assign(static_cast<const uint8_t*>(newVertices),
                              static_cast<const uint8_t*>(newVertices) + vertexSize * vertexCount);

            vertexBufferDirty = true;

            sharedEngine->getRenderer()->scheduleUpdate(shared_from_this());

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
                if (!vertexBufferId)
                {
                    log("Vertex buffer not initialized");
                    return false;
                }

                if (!RendererOGL::bindArrayBuffer(vertexBufferId))
                {
                    return false;
                }

                for (GLuint index = 0; index < 5; ++index)
                {
                    if (index < vertexAttribs.size())
                    {
                        glEnableVertexAttribArray(index);
                        glVertexAttribPointer(index,
                                              vertexAttribs[index].size,
                                              vertexAttribs[index].type,
                                              vertexAttribs[index].normalized,
                                              vertexAttribs[index].stride,
                                              vertexAttribs[index].pointer);
                    }
                    else
                    {
                        glDisableVertexAttribArray(index);
                    }
                }

                if (RendererOGL::checkOpenGLError())
                {
                    log("Failed to update vertex attributes");
                    return false;
                }
            }

            return true;
        }

        bool MeshBufferOGL::update()
        {
            if (indexBufferDirty || vertexBufferDirty)
            {
                std::vector<uint8_t> localIndexData;
                std::vector<uint8_t> localVertexData;

                {
                    std::lock_guard<std::mutex> lock(dataMutex);

                    if (indexBufferDirty)
                    {
                        localIndexData = indexData;
                        switch (indexSize)
                        {
                            case 1: indexFormat = GL_UNSIGNED_BYTE; break;
                            case 2: indexFormat = GL_UNSIGNED_SHORT; break;
                            case 4: indexFormat = GL_UNSIGNED_INT; break;
                            default: log("Invalid size"); return false;
                        }
                    }

                    if (vertexBufferDirty)
                    {
                        localVertexData = vertexData;

                        vertexAttribs.clear();

                        GLuint offset = 0;

                        if (vertexAttributes & VERTEX_POSITION)
                        {
                            vertexAttribs.push_back({
                                3, GL_FLOAT, GL_FALSE,
                                static_cast<GLint>(vertexSize),
                                reinterpret_cast<const GLvoid*>(offset)
                            });
                            offset += 3 * sizeof(float);
                        }
                        if (vertexAttributes & VERTEX_COLOR)
                        {
                            vertexAttribs.push_back({
                                4, GL_UNSIGNED_BYTE, GL_TRUE,
                                static_cast<GLint>(vertexSize),
                                reinterpret_cast<const GLvoid*>(offset)
                            });
                            offset += 4 * sizeof(uint8_t);
                        }
                        if (vertexAttributes & VERTEX_NORMAL)
                        {
                            vertexAttribs.push_back({
                                3, GL_FLOAT, GL_FALSE,
                                static_cast<GLint>(vertexSize),
                                reinterpret_cast<const GLvoid*>(offset)
                            });
                            offset += 3 * sizeof(float);
                        }
                        if (vertexAttributes & VERTEX_TEXCOORD0)
                        {
                            vertexAttribs.push_back({
                                2, GL_FLOAT, GL_FALSE,
                                static_cast<GLint>(vertexSize),
                                reinterpret_cast<const GLvoid*>(offset)
                            });
                            offset += 2 * sizeof(float);
                        }
                        if (vertexAttributes & VERTEX_TEXCOORD1)
                        {
                            vertexAttribs.push_back({
                                2, GL_FLOAT, GL_FALSE,
                                static_cast<GLint>(vertexSize),
                                reinterpret_cast<const GLvoid*>(offset)
                            });
                            offset += 2 * sizeof(float);
                        }
                        
                        if (offset != vertexSize)
                        {
                            log("Invalid vertex size");
                            return false;
                        }
                    }
                }

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
                    }

                    if (!localIndexData.empty())
                    {
                        RendererOGL::bindElementArrayBuffer(indexBufferId);
                        glBufferData(GL_ELEMENT_ARRAY_BUFFER, localIndexData.size(), localIndexData.data(),
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

                    if (vertexArrayId)
                    {
                        RendererOGL::bindVertexArray(vertexArrayId);
                        RendererOGL::bindArrayBuffer(vertexBufferId);

                        for (GLuint index = 0; index < 5; ++index)
                        {
                            if (index < vertexAttribs.size())
                            {
                                glEnableVertexAttribArray(index);
                                glVertexAttribPointer(index,
                                                      vertexAttribs[index].size,
                                                      vertexAttribs[index].type,
                                                      vertexAttribs[index].normalized,
                                                      vertexAttribs[index].stride,
                                                      vertexAttribs[index].pointer);
                            }
                            else
                            {
                                glDisableVertexAttribArray(index);
                            }
                        }

                        if (RendererOGL::checkOpenGLError())
                        {
                            log("Failed to update vertex attributes");
                            return false;
                        }
                    }

                    if (!localVertexData.empty())
                    {
                        RendererOGL::bindArrayBuffer(vertexBufferId);
                        glBufferData(GL_ARRAY_BUFFER, localVertexData.size(), localVertexData.data(),
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

                ready = true;
            }

            return true;
        }
    } // namespace graphics
} // namespace ouzel
