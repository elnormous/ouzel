// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include "Widget.h"

namespace ouzel
{
    class Texture;
    class MeshBuffer;
    class Shader;
    
    class Label: public Widget
    {
    public:
        Label(std::string const& font, std::string const& text, Vector2 const& textAnchor = Vector2(0.5f, 0.5f));
        virtual ~Label();
        
        virtual void draw() override;
        
    protected:
        TexturePtr _texture;
        MeshBufferPtr _meshBuffer;
        ShaderPtr _shader;
        
        uint32_t _uniModelViewProj;
    };
}
