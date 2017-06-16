// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include "utils/Noncopyable.h"
#include "scene/SpriteFrame.h"
#include "scene/ParticleDefinition.h"
#include "gui/BMFont.h"
#include "gui/FTFont.h"
#include "graphics/BlendState.h"
#include "graphics/Shader.h"
#include "graphics/Texture.h"

namespace ouzel
{
    class Cache: public Noncopyable
    {
    public:
        Cache();

        void clear();

        void preloadTexture(const std::string& filename, bool dynamic = false, bool mipmaps = true);
        const std::shared_ptr<graphics::Texture>& getTexture(const std::string& filename, bool dynamic = false, bool mipmaps = true) const;
		const std::shared_ptr<graphics::Texture>& getTextureFromData(
			const std::string& name,
			const std::vector<uint8_t>& data = std::vector<uint8_t>(),
			Size2 inSize = Size2(),
			bool dynamic = false,
			bool mipmaps = true) const;
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
        void releaseParticleDefinitions();
		
		void preloadFTFont(std::string filename, int pt);
        void preloadBMFont(const std::string& filename);
        const BMFont& getBMFont(const std::string& filename) const;
		const FTFont& getFTFont(const std::string& filename, int pt) const;
		void releaseFTFonts();
        void releaseBMFonts();

    protected:
        mutable std::map<std::string, std::shared_ptr<graphics::Texture>> textures;
        mutable std::map<std::string, std::shared_ptr<graphics::Shader>> shaders;
        mutable std::map<std::string, scene::ParticleDefinition> particleDefinitions;
        mutable std::map<std::string, std::shared_ptr<graphics::BlendState>> blendStates;
        mutable std::map<std::string, std::vector<scene::SpriteFrame>> spriteFrames;
        mutable std::map<std::string, BMFont> bmFonts;
		mutable std::map<std::string, FTFont> ftFonts;

    };
}
