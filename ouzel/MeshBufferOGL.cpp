// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "MeshBufferOGL.h"

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
        
        GLfloat* verts = new GLfloat[10 * vertices.size()];
        
        int i = 0;
        
        for (const Vertex& vertex : vertices)
        {
            verts[i * 10 + 0] = vertex.position.x;
            verts[i * 10 + 1] = vertex.position.y;
            verts[i * 10 + 2] = vertex.position.z;
            verts[i * 10 + 3] = vertex.position.w;
            
            verts[i * 10 + 4] = vertex.color.getR();
            verts[i * 10 + 5] = vertex.color.getG();
            verts[i * 10 + 6] = vertex.color.getB();
            verts[i * 10 + 7] = vertex.color.getA();
            
            verts[i * 10 + 8] = vertex.texCoord.x;
            verts[i * 10 + 9] = vertex.texCoord.y;
            
            ++i;
        }
        
        glGenBuffers(1, &_vertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, 10 * vertices.size() * sizeof(GLfloat), verts, GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(16));
        
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(32));
        
        glGenBuffers(1, &_indexBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t), indices.data(), GL_STATIC_DRAW);
        
        _count = static_cast<GLsizei>(indices.size());
        
        delete [] verts;
        
        return true;
    }
}
