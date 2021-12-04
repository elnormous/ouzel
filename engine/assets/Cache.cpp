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
#include "SpriteSheetLoader.hpp"
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
        addLoader(Asset::Type::font, loadBmf);
        addLoader(Asset::Type::skinnedMesh, loadCollada);
        addLoader(Asset::Type::cue, loadCue);
        addLoader(Asset::Type::skinnedMesh, loadGltf);
        addLoader(Asset::Type::image, loadImage);
        addLoader(Asset::Type::material, loadMtl);
        addLoader(Asset::Type::staticMesh, loadObj);
        addLoader(Asset::Type::particleSystem, loadParticleSystem);
        addLoader(Asset::Type::sprite, loadSprite);
        addLoader(Asset::Type::sprite, loadSpriteSheet);
        addLoader(Asset::Type::font, loadTtf);
        addLoader(Asset::Type::sound, loadVorbis);
        addLoader(Asset::Type::sound, loadWave);
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

    void Cache::addLoader(const Asset::Type assetType, const Loader loader)
    {
        loaders.push_back(std::make_pair(assetType, loader));
    }

    void Cache::removeLoader(const Loader loader)
    {
        for (auto i = loaders.begin(); i != loaders.end();)
            if (i->second == loader)
                i = loaders.erase(i);
            else
                ++i;
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
