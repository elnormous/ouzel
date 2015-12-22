// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "RendererOGL.h"
#include "TextureOGL.h"
#include "RenderTargetOGL.h"
#include "ShaderOGL.h"
#include "MeshBufferOGL.h"
#include "Engine.h"
#include "Scene.h"
#include "Camera.h"
#include "Utils.h"

static const char TEXTURE_PIXEL_SHADER[] =
    "#version 400\n"
    "uniform sampler2D tex;\n"
    "in vec4 ex_Color;\n"
    "in vec2 ex_TexCoord;\n"
    "out vec4 out_Color;\n"
    "void main(void)\n"
    "{\n"
    "    out_Color = texture(tex, ex_TexCoord) * ex_Color;\n"
    "}";

static const char TEXTURE_VERTEX_SHADER[] =
    "#version 400\n"
    "layout(location=0) in vec4 in_Position;\n"
    "layout(location=1) in vec4 in_Color;\n"
    "layout(location=2) in vec2 in_TexCoord;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 proj;\n"
    "out vec4 ex_Color;\n"
    "out vec2 ex_TexCoord;\n"
    "void main(void)\n"
    "{\n"
    "    gl_Position = proj * view * model * in_Position;\n"
    "    ex_Color = in_Color;\n"
    "    ex_TexCoord = in_TexCoord;\n"
    "}";

static const char COLOR_PIXEL_SHADER[] =
    "#version 400\n"
    "in vec4 ex_Color;\n"
    "out vec4 out_Color;\n"
    "void main(void)\n"
    "{\n"
    "    out_Color = ex_Color;\n"
    "}";

static const char COLOR_VERTEX_SHADER[] =
    "#version 400\n"
    "layout(location=0) in vec4 in_Position;\n"
    "layout(location=1) in vec4 in_Color;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 proj;\n"
    "out vec4 ex_Color;\n"
    "void main(void)\n"
    "{\n"
    "    gl_Position = proj * view * model * in_Position;\n"
    "    ex_Color = in_Color;\n"
    "}";

namespace ouzel
{
    RendererOGL::RendererOGL(const Size2& size, bool fullscreen, Engine* engine):
        Renderer(size, fullscreen, engine, Driver::OPENGL)
    {
        recalculateProjection();
    }
    
    bool RendererOGL::initOpenGL(uint32_t width, uint32_t height)
    {
        //glEnable(GL_DEPTH_TEST);
        glClearColor(_clearColor.getR(), _clearColor.getG(), _clearColor.getB(), _clearColor.getA());
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        //glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
        
        // precomputed alpha
        //glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
        //glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
        
        if (checkOpenGLErrors())
        {
            return false;
        }
        
        Shader* textureShader = loadShaderFromStrings(TEXTURE_PIXEL_SHADER, TEXTURE_VERTEX_SHADER);
        _shaders[SHADER_TEXTURE] = textureShader;
        
        Shader* colorShader = loadShaderFromStrings(COLOR_PIXEL_SHADER, COLOR_VERTEX_SHADER);
        _shaders[SHADER_COLOR] = colorShader;
        
        _ready = true;
        
        _engine->begin();
        
        return true;
    }
    
    bool RendererOGL::checkOpenGLErrors()
    {
        bool error = false;
        
        while (GLenum error = glGetError() != GL_NO_ERROR)
        {
            printf("OpenGL error: %d (%x)\n", error, error);
            
            error = true;
        }
        
        return error;
    }
    
    void RendererOGL::setClearColor(Color color)
    {
        Renderer::setClearColor(color);
        
        glClearColor(_clearColor.getR(), _clearColor.getG(), _clearColor.getB(), _clearColor.getA());
    }

    void RendererOGL::recalculateProjection()
    {
        Renderer::recalculateProjection();
        
        if (_ready)
        {
            glViewport(0, 0, _size.width, _size.height);
        }
    }
    
    void RendererOGL::clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        checkOpenGLErrors();
    }
    
    void RendererOGL::flush()
    {
        glFlush();
        checkOpenGLErrors();
    }
    
    Texture* RendererOGL::loadTextureFromFile(const std::string& filename)
    {
        TextureOGL* texture = new TextureOGL(this);
        
        if (!texture->initFromFile(filename))
        {
            delete texture;
            texture = nullptr;
        }
        
        return texture;
    }
    
    bool RendererOGL::activateTexture(Texture* texture, uint32_t layer)
    {
        if (!Renderer::activateTexture(texture, layer))
        {
            return false;
        }
        
        if (texture)
        {
            TextureOGL* textureOGL = static_cast<TextureOGL*>(texture);
            
            glActiveTexture(GL_TEXTURE0 + layer);
            glBindTexture(GL_TEXTURE_2D, textureOGL->getTextureId());
            
            if (checkOpenGLErrors())
            {
                return false;
            }
        }
        else
        {
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        
        return true;
    }
    
    Shader* RendererOGL::loadShaderFromFiles(const std::string& fragmentShader, const std::string& vertexShader)
    {
        ShaderOGL* shader = new ShaderOGL(fragmentShader, vertexShader, this);
        
        if (!shader->initFromFiles(fragmentShader, vertexShader))
        {
            delete shader;
            shader = nullptr;
        }
        
        return shader;
    }
    
    Shader* RendererOGL::loadShaderFromStrings(const std::string& fragmentShader, const std::string& vertexShader)
    {
        ShaderOGL* shader = new ShaderOGL(fragmentShader, vertexShader, this);
        
        if (!shader->initFromStrings(fragmentShader, vertexShader))
        {
            delete shader;
            shader = nullptr;
        }
        
        return shader;
    }
    
    bool RendererOGL::activateShader(Shader* shader)
    {
        if (!Renderer::activateShader(shader))
        {
            return false;
        }
        
        if (shader)
        {
            ShaderOGL* shaderOGL = static_cast<ShaderOGL*>(shader);
            
            glUseProgram(shaderOGL->getProgramId());
            
            if (checkOpenGLErrors())
            {
                return false;
            }
        }
        else
        {
            glUseProgram(0);
        }
        
        return true;
    }
    
    MeshBuffer* RendererOGL::createMeshBuffer(const std::vector<uint16_t>& indices, const std::vector<Vertex>& vertices)
    {
        MeshBufferOGL* meshBuffer = new MeshBufferOGL(this);
        
        if (!meshBuffer->initFromData(indices, vertices))
        {
            delete meshBuffer;
            meshBuffer = nullptr;
        }
        
        return meshBuffer;
    }
    
    bool RendererOGL::drawMeshBuffer(MeshBuffer* meshBuffer, const Matrix4& transform)
    {
        if (!Renderer::drawMeshBuffer(meshBuffer, transform))
        {
            return false;
        }
        
        MeshBufferOGL* meshBufferOGL = static_cast<MeshBufferOGL*>(meshBuffer);
        
        uint32_t uniProj = _activeShader->getVertexShaderConstantId("proj");
        _activeShader->setVertexShaderConstant(uniProj, &_engine->getRenderer()->getProjection(), 1);
        
        uint32_t uniView = _activeShader->getVertexShaderConstantId("view");
        _activeShader->setVertexShaderConstant(uniView, &_engine->getScene()->getCamera()->getTransform(), 1);
        
        uint32_t uniModel = _activeShader->getVertexShaderConstantId("model");
        _activeShader->setVertexShaderConstant(uniModel, &transform, 1);
        
        if (checkOpenGLErrors())
        {
            return false;
        }
        
        glBindVertexArray(meshBufferOGL->getVertexArrayId());
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshBufferOGL->getIndexBufferId());
        glDrawElements(GL_TRIANGLES, meshBufferOGL->getCount(), GL_UNSIGNED_SHORT, nullptr);
        
        if (checkOpenGLErrors())
        {
            return false;
        }
        
        return true;
    }
    
    void RendererOGL::drawLine(const Vector2& start, const Vector2& finish, const Color& color, const Matrix4& transform)
    {
        GLuint vertexArray = 0;
        GLuint vertexBuffer = 0;
        GLuint indexBuffer = 0;
        
        glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);
        
        GLfloat vertices[] = {
            start.x, start.y, -10.0f, 1.0f, color.getR(), color.getG(), color.getB(), color.getA(),
            finish.x, finish.y, -10.0f, 1.0f, color.getR(), color.getG(), color.getB(), color.getA()
        };
        
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(16));
        
        GLubyte indices[] = {0, 1};
        
        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        
        ShaderOGL* colorShader = static_cast<ShaderOGL*>(getShader(SHADER_COLOR));
        activateShader(colorShader);
        
        GLint uniProj = glGetUniformLocation(colorShader->getProgramId(), "proj");
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, _projection.m);
        
        GLint uniView = glGetUniformLocation(colorShader->getProgramId(), "view");
        
        Camera* camera = _engine->getScene()->getCamera();
        
        if (camera)
        {
            glUniformMatrix4fv(uniView, 1, GL_FALSE, camera->getTransform().m);
        }
        else
        {
            Matrix4 temp;
            glUniformMatrix4fv(uniView, 1, GL_FALSE, temp.m);
        }
        
        GLint uniModel = glGetUniformLocation(colorShader->getProgramId(), "model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, transform.m);
        
        glBindVertexArray(vertexArray);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glDrawElements(GL_LINE_STRIP, 2, GL_UNSIGNED_BYTE, nullptr);
        
        // delete buffers
        glDeleteVertexArrays(1, &vertexArray);
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &indexBuffer);
    }
    
    void RendererOGL::drawRectangle(const Rectangle& rectangle, const Color& color, const Matrix4& transform)
    {
        GLuint vertexArray = 0;
        GLuint vertexBuffer = 0;
        GLuint indexBuffer = 0;
        
        glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);
        
        GLfloat vertices[] = {
            rectangle.x, rectangle.y, -10.0f, 1.0f, color.getR(), color.getG(), color.getB(), color.getA(),
            rectangle.x + rectangle.width, rectangle.y, -10.0f, 1.0f, color.getR(), color.getG(), color.getB(), color.getA(),
            rectangle.x, rectangle.y + rectangle.height, -10.0f, 1.0f, color.getR(), color.getG(), color.getB(), color.getA(),
            rectangle.x + rectangle.width, rectangle.y + rectangle.height, -10.0f, 1.0f, color.getR(), color.getG(), color.getB(), color.getA()
        };
        
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(16));
        
        GLubyte indices[] = {0, 1, 3, 2, 0};
        
        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        
        ShaderOGL* colorShader = static_cast<ShaderOGL*>(getShader(SHADER_COLOR));
        activateShader(colorShader);
        
        GLint uniProj = glGetUniformLocation(colorShader->getProgramId(), "proj");
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, _projection.m);
        
        GLint uniView = glGetUniformLocation(colorShader->getProgramId(), "view");
        
        Camera* camera = _engine->getScene()->getCamera();
        
        if (camera)
        {
            glUniformMatrix4fv(uniView, 1, GL_FALSE, camera->getTransform().m);
        }
        else
        {
            Matrix4 temp;
            glUniformMatrix4fv(uniView, 1, GL_FALSE, temp.m);
        }
        
        GLint uniModel = glGetUniformLocation(colorShader->getProgramId(), "model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, transform.m);
        
        glBindVertexArray(vertexArray);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glDrawElements(GL_LINE_STRIP, 5, GL_UNSIGNED_BYTE, nullptr);
        
        // delete buffers
        glDeleteVertexArrays(1, &vertexArray);
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &indexBuffer);
    }
    
    void RendererOGL::drawQuad(const Rectangle& rectangle, const Color& color, const Matrix4& transform)
    {
        GLuint vertexArray = 0;
        GLuint vertexBuffer = 0;
        GLuint indexBuffer = 0;
        
        glGenVertexArrays(1, &vertexArray);
        glBindVertexArray(vertexArray);
        
        GLfloat vertices[] = {
            rectangle.x, rectangle.y, -10.0f, 1.0f, color.getR(), color.getG(), color.getB(), color.getA(), 0.0f, 1.0f,
            rectangle.x + rectangle.width, rectangle.y, -10.0f, 1.0f, color.getR(), color.getG(), color.getB(), color.getA(), 1.0f, 1.0f,
            rectangle.x, rectangle.y + rectangle.height, -10.0f, 1.0f, color.getR(), color.getG(), color.getB(), color.getA(), 0.0f, 0.0f,
            rectangle.x + rectangle.width, rectangle.y + rectangle.height, -10.0f, 1.0f, color.getR(), color.getG(), color.getB(), color.getA(), 1.0f, 0.0f
        };
        
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(0));
        
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(16));
        
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), reinterpret_cast<const GLvoid*>(32));
        
        GLubyte indices[] = {0, 1, 2, 1, 3, 2};
        
        glGenBuffers(1, &indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
        
        ShaderOGL* colorShader = static_cast<ShaderOGL*>(getShader(SHADER_TEXTURE));
        activateShader(colorShader);
        
        GLint uniProj = glGetUniformLocation(colorShader->getProgramId(), "proj");
        glUniformMatrix4fv(uniProj, 1, GL_FALSE, _projection.m);
        
        GLint uniView = glGetUniformLocation(colorShader->getProgramId(), "view");
        
        Camera* camera = _engine->getScene()->getCamera();
        
        if (camera)
        {
            glUniformMatrix4fv(uniView, 1, GL_FALSE, camera->getTransform().m);
        }
        else
        {
            Matrix4 temp;
            glUniformMatrix4fv(uniView, 1, GL_FALSE, temp.m);
        }
        
        GLint uniModel = glGetUniformLocation(colorShader->getProgramId(), "model");
        glUniformMatrix4fv(uniModel, 1, GL_FALSE, transform.m);
        
        glBindVertexArray(vertexArray);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, nullptr);
        
        // delete buffers
        glDeleteVertexArrays(1, &vertexArray);
        glDeleteBuffers(1, &vertexBuffer);
        glDeleteBuffers(1, &indexBuffer);
    }
}
