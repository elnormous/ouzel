// Ouzel by Elviss Strazdins

#include "ParticleSystemLoader.hpp"
#include "Bundle.hpp"
#include "Cache.hpp"
#include "../scene/ParticleSystem.hpp"
#include "../formats/Json.hpp"

namespace ouzel::assets
{
    ParticleSystemLoader::ParticleSystemLoader():
        Loader{Type::particleSystem}
    {
    }

    bool ParticleSystemLoader::loadAsset(Cache& cache,
                                         Bundle& bundle,
                                         const std::string& name,
                                         const std::vector<std::byte>& data,
                                         bool mipmaps)
    {
        scene::ParticleSystemData particleSystemData;

        const auto d = json::parse(data);

        if (!d.hasMember("textureFileName") ||
            !d.hasMember("configName"))
            return false;

        particleSystemData.name = d["configName"].as<std::string>();

        if (d.hasMember("blendFuncSource")) particleSystemData.blendFuncSource = d["blendFuncSource"].as<std::uint32_t>();
        if (d.hasMember("blendFuncDestination")) particleSystemData.blendFuncDestination = d["blendFuncDestination"].as<std::uint32_t>();

        if (d.hasMember("emitterType"))
        {
            switch (d["emitterType"].as<std::uint32_t>())
            {
                case 0: particleSystemData.emitterType = scene::ParticleSystemData::EmitterType::gravity; break;
                case 1: particleSystemData.emitterType = scene::ParticleSystemData::EmitterType::radius; break;
                default: throw std::runtime_error("Unsupported emitter type");
            }
        }

        if (d.hasMember("maxParticles")) particleSystemData.maxParticles = d["maxParticles"].as<std::uint32_t>();

        if (d.hasMember("duration")) particleSystemData.duration = d["duration"].as<float>();
        if (d.hasMember("particleLifespan")) particleSystemData.particleLifespan = d["particleLifespan"].as<float>();
        if (d.hasMember("particleLifespanVariance")) particleSystemData.particleLifespanVariance = d["particleLifespanVariance"].as<float>();

        if (d.hasMember("speed")) particleSystemData.speed = d["speed"].as<float>();
        if (d.hasMember("speedVariance")) particleSystemData.speedVariance = d["speedVariance"].as<float>();

        if (d.hasMember("absolutePosition")) particleSystemData.absolutePosition = d["absolutePosition"].as<bool>();

        if (d.hasMember("yCoordFlipped")) particleSystemData.yCoordFlipped = (d["yCoordFlipped"].as<std::uint32_t>() == 1);

        if (d.hasMember("sourcePositionx")) particleSystemData.sourcePosition.v[0] = d["sourcePositionx"].as<float>();
        if (d.hasMember("sourcePositiony")) particleSystemData.sourcePosition.v[1] = d["sourcePositiony"].as<float>();
        if (d.hasMember("sourcePositionVariancex")) particleSystemData.sourcePositionVariance.v[0] = d["sourcePositionVariancex"].as<float>();
        if (d.hasMember("sourcePositionVariancey")) particleSystemData.sourcePositionVariance.v[1] = d["sourcePositionVariancey"].as<float>();

        if (d.hasMember("startParticleSize")) particleSystemData.startParticleSize = d["startParticleSize"].as<float>();
        if (d.hasMember("startParticleSizeVariance")) particleSystemData.startParticleSizeVariance = d["startParticleSizeVariance"].as<float>();
        if (d.hasMember("finishParticleSize")) particleSystemData.finishParticleSize = d["finishParticleSize"].as<float>();
        if (d.hasMember("finishParticleSizeVariance")) particleSystemData.finishParticleSizeVariance = d["finishParticleSizeVariance"].as<float>();
        if (d.hasMember("angle")) particleSystemData.angle = d["angle"].as<float>();
        if (d.hasMember("angleVariance")) particleSystemData.angleVariance = d["angleVariance"].as<float>();
        if (d.hasMember("rotationStart")) particleSystemData.startRotation = d["rotationStart"].as<float>();
        if (d.hasMember("rotationStartVariance")) particleSystemData.startRotationVariance = d["rotationStartVariance"].as<float>();
        if (d.hasMember("rotationEnd")) particleSystemData.finishRotation = d["rotationEnd"].as<float>();
        if (d.hasMember("rotationEndVariance")) particleSystemData.finishRotationVariance = d["rotationEndVariance"].as<float>();
        if (d.hasMember("rotatePerSecond")) particleSystemData.rotatePerSecond = d["rotatePerSecond"].as<float>();
        if (d.hasMember("rotatePerSecondVariance")) particleSystemData.rotatePerSecondVariance = d["rotatePerSecondVariance"].as<float>();
        if (d.hasMember("minRadius")) particleSystemData.minRadius = d["minRadius"].as<float>();
        if (d.hasMember("minRadiusVariance")) particleSystemData.minRadiusVariance = d["minRadiusVariance"].as<float>();
        if (d.hasMember("maxRadius")) particleSystemData.maxRadius = d["maxRadius"].as<float>();
        if (d.hasMember("maxRadiusVariance")) particleSystemData.maxRadiusVariance = d["maxRadiusVariance"].as<float>();

        if (d.hasMember("radialAcceleration")) particleSystemData.radialAcceleration = d["radialAcceleration"].as<float>();
        if (d.hasMember("radialAccelVariance")) particleSystemData.radialAccelVariance = d["radialAccelVariance"].as<float>();
        if (d.hasMember("tangentialAcceleration")) particleSystemData.tangentialAcceleration = d["tangentialAcceleration"].as<float>();
        if (d.hasMember("tangentialAccelVariance")) particleSystemData.tangentialAccelVariance = d["tangentialAccelVariance"].as<float>();

        if (d.hasMember("rotationIsDir")) particleSystemData.rotationIsDir = d["rotationIsDir"].as<bool>();

        if (d.hasMember("gravityx")) particleSystemData.gravity.v[0] = d["gravityx"].as<float>();
        if (d.hasMember("gravityy")) particleSystemData.gravity.v[1] = d["gravityy"].as<float>();

        if (d.hasMember("startColorRed")) particleSystemData.startColorRed = d["startColorRed"].as<float>();
        if (d.hasMember("startColorGreen")) particleSystemData.startColorGreen = d["startColorGreen"].as<float>();
        if (d.hasMember("startColorBlue")) particleSystemData.startColorBlue = d["startColorBlue"].as<float>();
        if (d.hasMember("startColorAlpha")) particleSystemData.startColorAlpha = d["startColorAlpha"].as<float>();

        if (d.hasMember("startColorVarianceRed")) particleSystemData.startColorRedVariance = d["startColorVarianceRed"].as<float>();
        if (d.hasMember("startColorVarianceGreen")) particleSystemData.startColorGreenVariance = d["startColorVarianceGreen"].as<float>();
        if (d.hasMember("startColorVarianceBlue")) particleSystemData.startColorBlueVariance = d["startColorVarianceBlue"].as<float>();
        if (d.hasMember("startColorVarianceAlpha")) particleSystemData.startColorAlphaVariance = d["startColorVarianceAlpha"].as<float>();

        if (d.hasMember("finishColorRed")) particleSystemData.finishColorRed = d["finishColorRed"].as<float>();
        if (d.hasMember("finishColorGreen")) particleSystemData.finishColorGreen = d["finishColorGreen"].as<float>();
        if (d.hasMember("finishColorBlue")) particleSystemData.finishColorBlue = d["finishColorBlue"].as<float>();
        if (d.hasMember("finishColorAlpha")) particleSystemData.finishColorAlpha = d["finishColorAlpha"].as<float>();

        if (d.hasMember("finishColorVarianceRed")) particleSystemData.finishColorRedVariance = d["finishColorVarianceRed"].as<float>();
        if (d.hasMember("finishColorVarianceGreen")) particleSystemData.finishColorGreenVariance = d["finishColorVarianceGreen"].as<float>();
        if (d.hasMember("finishColorVarianceBlue")) particleSystemData.finishColorBlueVariance = d["finishColorVarianceBlue"].as<float>();
        if (d.hasMember("finishColorVarianceAlpha")) particleSystemData.finishColorAlphaVariance = d["finishColorVarianceAlpha"].as<float>();

        if (d.hasMember("textureFileName"))
        {
            const auto textureFileName = d["textureFileName"].as<std::string>();
            particleSystemData.texture = cache.getTexture(textureFileName);

            if (!particleSystemData.texture)
            {
                bundle.loadAsset(Type::image, textureFileName, textureFileName, mipmaps);
                particleSystemData.texture = cache.getTexture(textureFileName);
            }
        }

        particleSystemData.emissionRate = static_cast<float>(particleSystemData.maxParticles) / particleSystemData.particleLifespan;

        bundle.setParticleSystemData(name, particleSystemData);

        return true;
    }
}
