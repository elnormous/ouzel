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
        Label(std::string const& font, std::string const& text);
        virtual ~Label();
        
        virtual void draw() override;
        
    protected:
        std::shared_ptr<Texture> _texture;
        std::shared_ptr<MeshBuffer> _meshBuffer;
        std::shared_ptr<Shader> _shader;
        
        uint32_t _uniModelViewProj;
    };
}
