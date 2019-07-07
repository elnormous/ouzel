// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <algorithm>
#include "Cache.hpp"
#include "graphics/Renderer.hpp"
#include "gui/BMFont.hpp"
#include "gui/TTFont.hpp"

namespace ouzel
{
    namespace assets
    {
        void Cache::addBundle(Bundle* bundle)
        {
            auto i = std::find(bundles.begin(), bundles.end(), bundle);
            if (i == bundles.end())
                bundles.push_back(bundle);
        }

        void Cache::removeBundle(Bundle* bundle)
        {
            auto i = std::find(bundles.begin(), bundles.end(), bundle);
            if (i != bundles.end())
                bundles.erase(i);
        }

        void Cache::addLoader(Loader* loader)
        {
            auto i = std::find(loaders.begin(), loaders.end(), loader);
            if (i == loaders.end())
                loaders.push_back(loader);
        }

        void Cache::removeLoader(Loader* loader)
        {
            auto i = std::find(loaders.begin(), loaders.end(), loader);
            if (i != loaders.end())
                loaders.erase(i);
        }

        std::shared_ptr<graphics::Texture> Cache::getTexture(const std::string& name) const
        {
            for (Bundle* bundle : bundles)
                if (auto texture = bundle->getTexture(name))
                    return texture;

            return nullptr;
        }

        std::shared_ptr<graphics::Shader> Cache::getShader(const std::string& name) const
        {
            for (Bundle* bundle : bundles)
                if (auto shader = bundle->getShader(name))
                    return shader;

            return nullptr;
        }

        std::shared_ptr<graphics::BlendState> Cache::getBlendState(const std::string& name) const
        {
            for (Bundle* bundle : bundles)
                if (auto blendState = bundle->getBlendState(name))
                    return blendState;

            return nullptr;
        }

        std::shared_ptr<graphics::DepthStencilState> Cache::getDepthStencilState(const std::string& name) const
        {
            for (Bundle* bundle : bundles)
                if (auto depthStencilState = bundle->getDepthStencilState(name))
                    return depthStencilState;

            return nullptr;
        }

        const scene::SpriteData* Cache::getSpriteData(const std::string& name) const
        {
            for (Bundle* bundle : bundles)
                if (auto spriteData = bundle->getSpriteData(name))
                    return spriteData;

            return nullptr;
        }

        const scene::ParticleSystemData* Cache::getParticleSystemData(const std::string& name) const
        {
            for (Bundle* bundle : bundles)
                if (auto particleSystemData = bundle->getParticleSystemData(name))
                    return particleSystemData;

            return nullptr;
        }

        std::shared_ptr<gui::Font> Cache::getFont(const std::string& name) const
        {
            for (Bundle* bundle : bundles)
                if (auto font = bundle->getFont(name))
                    return font;

            return nullptr;
        }

        std::shared_ptr<audio::Bank> Cache::getBank(const std::string& name) const
        {
            for (Bundle* bundle : bundles)
                if (auto bank = bundle->getBank(name))
                    return bank;

            return nullptr;
        }

        std::shared_ptr<audio::Sound> Cache::getSound(const std::string& name) const
        {
            for (Bundle* bundle : bundles)
                if (auto sound = bundle->getSound(name))
                    return sound;

            return nullptr;
        }

        std::shared_ptr<graphics::Material> Cache::getMaterial(const std::string& name) const
        {
            for (Bundle* bundle : bundles)
                if (auto material = bundle->getMaterial(name))
                    return material;

            return nullptr;
        }

        const scene::SkinnedMeshData* Cache::getSkinnedMeshData(const std::string& name) const
        {
            for (Bundle* bundle : bundles)
                if (const scene::SkinnedMeshData* meshData = bundle->getSkinnedMeshData(name))
                    return meshData;

            return nullptr;
        }

        const scene::StaticMeshData* Cache::getStaticMeshData(const std::string& name) const
        {
            for (Bundle* bundle : bundles)
                if (const scene::StaticMeshData* meshData = bundle->getStaticMeshData(name))
                    return meshData;

            return nullptr;
        }
    } // namespace assets
} // namespace ouzel
