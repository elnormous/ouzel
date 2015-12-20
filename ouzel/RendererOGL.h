// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Renderer.h"

namespace ouzel
{
    class RendererOGL: public Renderer
    {
    public:
        RendererOGL(Engine* engine);
        
        bool initOpenGL(uint32_t width, uint32_t height);
        
        virtual void resize(uint32_t width, uint32_t height);
        
        virtual void clear();
        virtual void flush();
        
        virtual void activateTexture(Texture* texture, uint32_t layer);
        virtual void activateShader(Shader* shader);
        
        virtual void drawLine(const Vector2& start, const Vector2& finish, const Vector3& color, const Matrix4& transform = Matrix4());
        virtual void drawRectangle(const Rectangle& rectangle, const Vector3& color, const Matrix4& transform = Matrix4());
        virtual void drawQuad(const Rectangle& rectangle, const Vector3& color, const Matrix4& transform = Matrix4());
    };
}
