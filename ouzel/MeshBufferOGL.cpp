// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferOGL.h"
#include "RendererOGL.h"

namespace ouzel
{
    MeshBufferOGL::MeshBufferOGL(Renderer* renderer):
        MeshBuffer(renderer)
    {
        
    }
    
    MeshBufferOGL::~MeshBufferOGL()
    {
        if (_vertexArrayId) glDeleteVertexArrays(1, &_vertexArrayId);
        if (_vertexBufferId) glDeleteBuffers(1, &_vertexBufferId);
        if (_indexBufferId) glDeleteBuffers(1, &_indexBufferId);
    }
    
    bool MeshBufferOGL::initFromData(const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices)
    {
        glGenVertexArrays(1, &_vertexArrayId);
        glBindVertexArray(_vertexArrayId);
        
        glGenBuffers(1, &_vertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
        
        if (static_cast<RendererOGL*>(_renderer)->checkOpenGLErrors())
        {
            return false;
        }
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const GLvoid*>(0));
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), reinterpret_cast<const GLvoid*>(16));
        
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<const GLvoid*>(20));
        
        glGenBuffers(1, &_indexBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t), indices.data(), GL_STATIC_DRAW);
        
        if (static_cast<RendererOGL*>(_renderer)->checkOpenGLErrors())
        {
            return false;
        }
        
        _indexCount = static_cast<GLsizei>(indices.size());
        
        return true;
    }
}
