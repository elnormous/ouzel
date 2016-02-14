// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include "Widget.h"
#include "BMFont.h"

namespace ouzel
{
    class Texture;
    class MeshBuffer;
    class Shader;
    
    class Label: public Widget
    {
    public:
        static std::shared_ptr<Label> create(const std::string& font, const std::string& text, const Vector2& textAnchor = Vector2(0.5f, 0.5f));
        
        Label();
        virtual ~Label();
        
        virtual bool init(const std::string& font, const std::string& text, const Vector2& textAnchor = Vector2(0.5f, 0.5f));
        
        virtual void draw() override;
        
        virtual void setText(const std::string& text);
        virtual const std::string& getText() const { return _text; }
        
        virtual const Color& getColor() const { return _color; }
        virtual void setColor(const Color& color);
        
    protected:
        TexturePtr _texture;
        MeshBufferPtr _meshBuffer;
        ShaderPtr _shader;
        
        uint32_t _uniModelViewProj;
        BMFont _font;
        Vector2 _textAnchor;
        std::string _text;
        
        Color _color = Color(255, 255, 255, 255);
    };
}
