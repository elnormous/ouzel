// Copyright (C) 2016 Elviss Strazdins
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

        void preloadTexture(const std::string& filename, bool dynamic = false, bool mipmaps = true);
        video::TexturePtr getTexture(const std::string& filename, bool dynamic = false, bool mipmaps = true) const;
        void setTexture(const std::string& filename, const video::TexturePtr& texture);
        virtual void releaseTextures();

        video::ShaderPtr getShader(const std::string& shaderName) const;
        void setShader(const std::string& shaderName, const video::ShaderPtr& shader);

        void preloadParticleDefinition(const std::string& filename);
        scene::ParticleDefinitionPtr getParticleDefinition(const std::string& filename) const;

    protected:
        Cache();

        scene::ParticleDefinitionPtr loadParticleDefinition(const std::string& filename) const;

        mutable std::unordered_map<std::string, video::TexturePtr> _textures;
        mutable std::unordered_map<std::string, video::ShaderPtr> _shaders;
        mutable std::unordered_map<std::string, scene::ParticleDefinitionPtr> _particleDefinitions;
    };
}
