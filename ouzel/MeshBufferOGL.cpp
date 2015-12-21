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
        if (_colorBufferId) glDeleteBuffers(1, &_colorBufferId);
        if (_texcoordBufferId) glDeleteBuffers(1, &_texcoordBufferId);
        if (_indexBufferId) glDeleteBuffers(1, &_indexBufferId);
    }
    
    bool MeshBufferOGL::initFromData(const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices)
    {
        glGenVertexArrays(1, &_vertexArrayId);
        glBindVertexArray(_vertexArrayId);
        
        GLfloat* verts = new GLfloat[4 * vertices.size()];
        GLfloat* colors = new GLfloat[4 * vertices.size()];
        GLfloat* texCoords = new GLfloat[2 * vertices.size()];
        
        int i = 0;
        
        for (const Vertex& vertex : vertices)
        {
            verts[i * 4 + 0] = vertex.position.x;
            verts[i * 4 + 1] = vertex.position.y;
            verts[i * 4 + 2] = vertex.position.z;
            verts[i * 4 + 3] = vertex.position.w;
            
            colors[i * 4 + 0] = vertex.color.getR();
            colors[i * 4 + 1] = vertex.color.getG();
            colors[i * 4 + 2] = vertex.color.getB();
            colors[i * 4 + 3] = vertex.color.getA();
            
            texCoords[i * 2 + 0] = vertex.texCoord.x;
            texCoords[i * 2 + 1] = vertex.texCoord.y;
            
            ++i;
        }
        
        glGenBuffers(1, &_vertexBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferId);
        glBufferData(GL_ARRAY_BUFFER, 4 * vertices.size() * sizeof(GLfloat), verts, GL_STATIC_DRAW);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);
        
        glGenBuffers(1, &_colorBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, _colorBufferId);
        glBufferData(GL_ARRAY_BUFFER, 4 * vertices.size() * sizeof(GLfloat), colors, GL_STATIC_DRAW);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);
        
        glGenBuffers(1, &_texcoordBufferId);
        glBindBuffer(GL_ARRAY_BUFFER, _texcoordBufferId);
        glBufferData(GL_ARRAY_BUFFER, 2 * vertices.size() * sizeof(GLfloat), texCoords, GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(2);
        
        glGenBuffers(1, &_indexBufferId);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferId);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t), indices.data(), GL_STATIC_DRAW);
        
        _count = static_cast<GLsizei>(indices.size());
        
        delete [] verts;
        delete [] colors;
        delete [] texCoords;
        
        return true;
    }
}
