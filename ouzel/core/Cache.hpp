// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <string>
#include <map>
#include "utils/Noncopyable.hpp"
#include "scene/SpriteDefinition.hpp"
#include "scene/ParticleDefinition.hpp"
#include "gui/Font.hpp"
#include "graphics/BlendState.hpp"
#include "graphics/Shader.hpp"
#include "graphics/Texture.hpp"
#include "audio/SoundData.hpp"

namespace ouzel
{
    class Cache: public Noncopyable
    {
    public:
        Cache();

        void clear();

        void preloadTexture(const std::string& filename, bool dynamic = false, bool mipmaps = true);
        const std::shared_ptr<graphics::Texture>& getTexture(const std::string& filename, bool dynamic = false, bool mipmaps = true) const;
        void setTexture(const std::string& filename, const std::shared_ptr<graphics::Texture>& texture);
        void releaseTextures();

        const std::shared_ptr<graphics::Shader>& getShader(const std::string& shaderName) const;
        void setShader(const std::string& shaderName, const std::shared_ptr<graphics::Shader>& shader);
        void releaseShaders();

        const std::shared_ptr<graphics::BlendState>& getBlendState(const std::string& blendStateName) const;
        void setBlendState(const std::string& blendStateName, const std::shared_ptr<graphics::BlendState>& blendState);
        void releaseBlendStates();

        void preloadSpriteDefinition(const std::string& filename, bool mipmaps = true,
                                     uint32_t spritesX = 1, uint32_t spritesY = 1,
                                     const Vector2& pivot = Vector2(0.5f, 0.5f));
        const scene::SpriteDefinition& getSpriteDefinition(const std::string& filename, bool mipmaps = true,
                                                           uint32_t spritesX = 1, uint32_t spritesY = 1,
                                                           const Vector2& pivot = Vector2(0.5f, 0.5f)) const;
        void setSpriteDefinition(const std::string& filename, const scene::SpriteDefinition& spriteDefinition);
        void releaseSpriteDefinitions();

        void preloadParticleDefinition(const std::string& filename);
        const scene::ParticleDefinition& getParticleDefinition(const std::string& filename) const;
        void setParticleDefinition(const std::string& filename, const scene::ParticleDefinition& particleDefinition);
        void releaseParticleDefinitions();

        void preloadFont(const std::string& filename, float fontSize = 1.0f, bool mipmaps = true);
        const std::shared_ptr<Font>& getFont(const std::string& filename, float fontSize = 1.0f, bool mipmaps = true) const;
        void setFont(const std::string& filename, const std::shared_ptr<Font>& font);
        void releaseFonts();

        void preloadSoundData(const std::string& filename);
        const std::shared_ptr<audio::SoundData>& getSoundData(const std::string& filename) const;
        void setSoundData(const std::string& filename, const std::shared_ptr<audio::SoundData>& newSoundData);
        void releaseSoundData();

    protected:
        mutable std::map<std::string, std::shared_ptr<graphics::Texture>> textures;
        mutable std::map<std::string, std::shared_ptr<graphics::Shader>> shaders;
        mutable std::map<std::string, scene::ParticleDefinition> particleDefinitions;
        mutable std::map<std::string, std::shared_ptr<graphics::BlendState>> blendStates;
        mutable std::map<std::string, scene::SpriteDefinition> spriteDefinitions;
        mutable std::map<std::string, std::shared_ptr<Font>> fonts;
        mutable std::map<std::string, std::shared_ptr<audio::SoundData>> soundData;
    };
}
