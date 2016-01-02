// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferOGL.h"
#include "Engine.h"
#include "RendererOGL.h"
#include "Utils.h"

namespace ouzel
{
    MeshBufferOGL::MeshBufferOGL()
    {
        
    }
    
    MeshBufferOGL::~MeshBufferOGL()
    {
        if (_vertexArrayId) glDeleteVertexArrays(1, &_vertexArrayId);
        if (_vertexBufferId) glDeleteBuffers(1, &_vertexBufferId);
        if (_indexBufferId) glDeleteBuffers(1, &_indexBufferId);
    }
    
    bool MeshBufferOGL::initFromData(const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices, bool dynamicIndexBuffer, bool dynamicVertexBuffer)
    {
        if (!MeshBuffer::initFromData(indices, vertices, dynamicIndexBuffer, dynamicVertexBuffer))
        {
            return false;
        }
        
        glGenBuffers(1, &_indexBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, vectorDataSize(indices), indices.data(),
                     _dynamicIndexBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        
        if (static_cast<RendererOGL*>(Renderer::getInstance())->checkOpenGLErrors())
        {
            return false;
        }
        
        glGenVertexArrays(1, &_vertexArrayId);
        glBindVertexArray(_vertexArrayId);
        
        glGenBuffers(1, &_vertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, vectorDataSize(vertices), vertices.data(),
                     _dynamicVertexBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        
        if (static_cast<RendererOGL*>(Renderer::getInstance())->checkOpenGLErrors())
        {
            return false;
        }
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const GLvoid*>(0));
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), reinterpret_cast<const GLvoid*>(12));
        
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const GLvoid*>(16));
        
        if (static_cast<RendererOGL*>(Renderer::getInstance())->checkOpenGLErrors())
        {
            return false;
        }
        
        _indexCount = static_cast<GLsizei>(indices.size());
        
        return true;
    }
    
    bool MeshBufferOGL::uploadIndices(const std::vector<uint16_t>& indices)
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, vectorDataSize(indices), indices.data(),
                     _dynamicIndexBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        
        if (static_cast<RendererOGL*>(Renderer::getInstance())->checkOpenGLErrors())
        {
            return false;
        }
        
        return true;
    }
    
    bool MeshBufferOGL::uploadVertices(const std::vector<Vertex>& vertices)
    {
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, vectorDataSize(vertices), vertices.data(),
                     _dynamicVertexBuffer ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        
        if (static_cast<RendererOGL*>(Renderer::getInstance())->checkOpenGLErrors())
        {
            return false;
        }
        
        return true;
    }
}
