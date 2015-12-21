// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Renderer.h"

namespace ouzel
{
    class RendererOGL: public Renderer
    {
    public:
        RendererOGL(const Size2& size, bool fullscreen, Engine* engine);
        
        bool initOpenGL(uint32_t width, uint32_t height);
        
        virtual void recalculateProjection() override;
        
        virtual void clear() override;
        virtual void flush() override;
        
        virtual Texture* loadTextureFromFile(const std::string& filename) override;
        virtual void activateTexture(Texture* texture, uint32_t layer) override;
        
        virtual Shader* loadShaderFromFiles(const std::string& fragmentShader, const std::string& vertexShader) override;
        virtual Shader* loadShaderFromStrings(const std::string& fragmentShader, const std::string& vertexShader) override;
        virtual void activateShader(Shader* shader) override;
        
        virtual void drawLine(const Vector2& start, const Vector2& finish, const Color& color, const Matrix4& transform = Matrix4()) override;
        virtual void drawRectangle(const Rectangle& rectangle, const Color& color, const Matrix4& transform = Matrix4()) override;
        virtual void drawQuad(const Rectangle& rectangle, const Color& color, const Matrix4& transform = Matrix4()) override;
    };
}
