// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <unordered_map>
#include "Types.h"
#include "Noncopyable.h"

namespace ouzel
{
    class Cache: public Noncopyable
    {
        friend Engine;
    public:
        virtual ~Cache();
        
        void preloadTexture(const std::string& filename, bool dynamic = false);
        TexturePtr getTexture(const std::string& filename);
        void setTexture(const std::string& filename, TexturePtr const& texture);
        virtual void releaseTextures();
        
        ShaderPtr getShader(const std::string& shaderName) const;
        void setShader(const std::string& shaderName, ShaderPtr const& shader);
        
    protected:
        Cache();
        
        std::unordered_map<std::string, TexturePtr> _textures;
        std::unordered_map<std::string, ShaderPtr> _shaders;
    };
}