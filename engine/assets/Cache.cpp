// Ouzel by Elviss Strazdins

#include <algorithm>
#include "Cache.hpp"
#include "BmfLoader.hpp"
#include "ColladaLoader.hpp"
#include "CueLoader.hpp"
#include "GltfLoader.hpp"
#include "ImageLoader.hpp"
#include "MtlLoader.hpp"
#include "ObjLoader.hpp"
#include "ParticleSystemLoader.hpp"
#include "SpriteLoader.hpp"
#include "TtfLoader.hpp"
#include "VorbisLoader.hpp"
#include "WaveLoader.hpp"
#include "../graphics/Graphics.hpp"
#include "../gui/BMFont.hpp"
#include "../gui/TTFont.hpp"

namespace ouzel::assets
{
    Cache::Cache()
    {
        addLoader(std::make_unique<BmfLoader>());
        addLoader(std::make_unique<ColladaLoader>());
        addLoader(std::make_unique<CueLoader>());
        addLoader(std::make_unique<GltfLoader>());
        addLoader(std::make_unique<ImageLoader>());
        addLoader(std::make_unique<MtlLoader>());
        addLoader(std::make_unique<ObjLoader>());
        addLoader(std::make_unique<ParticleSystemLoader>());
        addLoader(std::make_unique<SpriteLoader>());
        addLoader(std::make_unique<TtfLoader>());
        addLoader(std::make_unique<VorbisLoader>());
        addLoader(std::make_unique<WaveLoader>());
    }

    void Cache::addBundle(const Bundle* bundle)
    {
        if (std::find(bundles.begin(), bundles.end(), bundle) == bundles.end())
            bundles.push_back(bundle);
    }

    void Cache::removeBundle(const Bundle* bundle)
    {
        if (const auto i = std::find(bundles.begin(), bundles.end(), bundle); i != bundles.end())
            bundles.erase(i);
    }

    void Cache::addLoader(std::unique_ptr<Loader> loader)
    {
        if (const auto i = std::find(loaders.begin(), loaders.end(), loader); i == loaders.end())
            loaders.push_back(std::move(loader));
    }

    void Cache::removeLoader(const Loader* loader)
    {
        const auto i = std::find_if(loaders.begin(), loaders.end(), [loader](const auto& ownedLoader) noexcept {
            return loader == ownedLoader.get();
        });
        if (i != loaders.end())
            loaders.erase(i);
    }

    std::shared_ptr<graphics::Texture> Cache::getTexture(std::string_view name) const
    {
        for (const auto bundle : bundles)
            if (const auto texture = bundle->getTexture(name))
                return texture;

        return nullptr;
    }

    const graphics::Shader* Cache::getShader(std::string_view name) const
    {
        for (const auto bundle : bundles)
            if (const auto shader = bundle->getShader(name))
                return shader;

        return nullptr;
    }

    const graphics::BlendState* Cache::getBlendState(std::string_view name) const
    {
        for (const auto bundle : bundles)
            if (const auto blendState = bundle->getBlendState(name))
                return blendState;

        return nullptr;
    }

    const graphics::DepthStencilState* Cache::getDepthStencilState(std::string_view name) const
    {
        for (const auto bundle : bundles)
            if (const auto depthStencilState = bundle->getDepthStencilState(name))
                return depthStencilState;

        return nullptr;
    }

    const scene::SpriteData* Cache::getSpriteData(std::string_view name) const
    {
        for (const auto bundle : bundles)
            if (const auto spriteData = bundle->getSpriteData(name))
                return spriteData;

        return nullptr;
    }

    const scene::ParticleSystemData* Cache::getParticleSystemData(std::string_view name) const
    {
        for (const auto bundle : bundles)
            if (const auto particleSystemData = bundle->getParticleSystemData(name))
                return particleSystemData;

        return nullptr;
    }

    const gui::Font* Cache::getFont(std::string_view name) const
    {
        for (const auto bundle : bundles)
            if (const auto font = bundle->getFont(name))
                return font;

        return nullptr;
    }

    const audio::Cue* Cache::getCue(std::string_view name) const
    {
        for (const auto bundle : bundles)
            if (const auto cue = bundle->getCue(name))
                return cue;

        return nullptr;
    }

    const audio::Sound* Cache::getSound(std::string_view name) const
    {
        for (const auto bundle : bundles)
            if (const auto sound = bundle->getSound(name))
                return sound;

        return nullptr;
    }

    const graphics::Material* Cache::getMaterial(std::string_view name) const
    {
        for (const auto bundle : bundles)
            if (const auto material = bundle->getMaterial(name))
                return material;

        return nullptr;
    }

    const scene::SkinnedMeshData* Cache::getSkinnedMeshData(std::string_view name) const
    {
        for (const auto bundle : bundles)
            if (const auto meshData = bundle->getSkinnedMeshData(name))
                return meshData;

        return nullptr;
    }

    const scene::StaticMeshData* Cache::getStaticMeshData(std::string_view name) const
    {
        for (const auto bundle : bundles)
            if (const auto meshData = bundle->getStaticMeshData(name))
                return meshData;

        return nullptr;
    }
}
