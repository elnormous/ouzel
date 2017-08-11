// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include "utils/Noncopyable.hpp"
#include "scene/SpriteFrame.hpp"
#include "scene/ParticleDefinition.hpp"
#include "gui/BMFont.hpp"
#include "gui/TTFont.hpp"
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

        void preloadSpriteFrames(const std::string& filename, bool mipmaps = true,
                                 uint32_t spritesX = 1, uint32_t spritesY = 1,
                                 const Vector2& pivot = Vector2(0.5f, 0.5f));
        const std::vector<scene::SpriteFrame>& getSpriteFrames(const std::string& filename, bool mipmaps = true,
                                                               uint32_t spritesX = 1, uint32_t spritesY = 1,
                                                               const Vector2& pivot = Vector2(0.5f, 0.5f)) const;
        void setSpriteFrames(const std::string& filename, const std::vector<scene::SpriteFrame>& frames);
        void releaseSpriteFrames();

        void preloadParticleDefinition(const std::string& filename);
        const scene::ParticleDefinition& getParticleDefinition(const std::string& filename) const;
        void setParticleDefinition(const std::string& filename, const scene::ParticleDefinition& particleDefinition);
        void releaseParticleDefinitions();

        void preloadFTFont(std::string filename, uint16_t pt, bool mipmaps = true);
        void preloadBMFont(const std::string& filename, bool mipmaps = true);
        const std::shared_ptr<BMFont>& getBMFont(const std::string& filename, bool mipmaps = true) const;
        const std::shared_ptr<TTFont>& getTTFont(const std::string& filename, uint16_t pt, bool mipmaps = true) const;
        void setBMFont(const std::string& filename, const std::shared_ptr<BMFont>& bmFont);
        void releaseTTFonts();
        void releaseBMFonts();

        void preloadSoundData(const std::string& filename);
        const std::shared_ptr<audio::SoundData>& getSoundData(const std::string& filename) const;
        void setSoundData(const std::string& filename, const std::shared_ptr<audio::SoundData>& newSoundData);
        void releaseSoundData();

    protected:
        mutable std::map<std::string, std::shared_ptr<graphics::Texture>> textures;
        mutable std::map<std::string, std::shared_ptr<graphics::Shader>> shaders;
        mutable std::map<std::string, scene::ParticleDefinition> particleDefinitions;
        mutable std::map<std::string, std::shared_ptr<graphics::BlendState>> blendStates;
        mutable std::map<std::string, std::vector<scene::SpriteFrame>> spriteFrames;
        mutable std::map<std::string, std::shared_ptr<BMFont>> bmFonts;
        mutable std::map<std::string, std::shared_ptr<TTFont>> ttFonts;
        mutable std::map<std::string, std::shared_ptr<audio::SoundData>> soundData;
    };
}
