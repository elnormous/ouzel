// Ouzel by Elviss Strazdins

#include <cassert>
#include <algorithm>
#include "Layer.hpp"
#include "Actor.hpp"
#include "Camera.hpp"
#include "Scene.hpp"
#include "Component.hpp"
#include "../core/Engine.hpp"
#include "../graphics/Graphics.hpp"
#include "../math/Matrix.hpp"

namespace ouzel::scene
{
    Layer::Layer()
    {
        layer = this;
    }

    Layer::~Layer()
    {
        if (scene) scene->removeLayer(*this);
    }

    void Layer::draw()
    {
        for (const auto camera : cameras)
        {
            std::vector<Actor*> drawQueue;

            for (const auto actor : children)
                actor->visit(drawQueue, math::identityMatrix<float, 4>, false, camera, 0, false);

            engine->getGraphics().setRenderTarget(camera->getRenderTarget() ? camera->getRenderTarget()->getResource() : 0);
            engine->getGraphics().setViewport(camera->getRenderViewport());
            engine->getGraphics().setDepthStencilState(camera->getDepthStencilState() ? camera->getDepthStencilState()->getResource() : 0,
                                                        camera->getStencilReferenceValue());

            for (const auto actor : drawQueue)
                actor->draw(camera, camera->getWireframe());
        }
    }

    void Layer::addChild(Actor& actor)
    {
        ActorContainer::addChild(actor);

        actor.updateTransform(math::identityMatrix<float, 4>);
    }

    void Layer::addCamera(Camera& camera)
    {
        cameras.push_back(&camera);
    }

    void Layer::removeCamera(Camera& camera)
    {
        if (const auto i = std::find(cameras.begin(), cameras.end(), &camera); i != cameras.end())
            cameras.erase(i);
    }

    void Layer::addLight(Light& light)
    {
        lights.push_back(&light);
    }

    void Layer::removeLight(Light& light)
    {
        if (const auto i = std::find(lights.begin(), lights.end(), &light); i != lights.end())
            lights.erase(i);
    }

    std::pair<Actor*, math::Vector<float, 3>> Layer::pickActor(const math::Vector<float, 2>& position, bool renderTargets) const
    {
        for (auto i = cameras.rbegin(); i != cameras.rend(); ++i)
        {
            const auto camera = *i;

            if (renderTargets || !camera->getRenderTarget())
            {
                const auto worldPosition = math::Vector<float, 2>{camera->convertNormalizedToWorld(position)};
                const auto actors = findActors(worldPosition);
                if (!actors.empty()) return actors.front();
            }
        }

        return std::pair(nullptr, math::Vector<float, 3>{});
    }

    std::vector<std::pair<Actor*, math::Vector<float, 3>>> Layer::pickActors(const math::Vector<float, 2>& position, bool renderTargets) const
    {
        std::vector<std::pair<Actor*, math::Vector<float, 3>>> result;

        for (auto i = cameras.rbegin(); i != cameras.rend(); ++i)
        {
            const auto camera = *i;

            if (renderTargets || !camera->getRenderTarget())
            {
                const auto worldPosition = math::Vector<float, 2>{camera->convertNormalizedToWorld(position)};
                const auto actors = findActors(worldPosition);
                result.insert(result.end(), actors.begin(), actors.end());
            }
        }

        return result;
    }

    std::vector<Actor*> Layer::pickActors(const std::vector<math::Vector<float, 2>>& edges, bool renderTargets) const
    {
        std::vector<Actor*> result;

        for (auto i = cameras.rbegin(); i != cameras.rend(); ++i)
        {
            const auto camera = *i;

            if (renderTargets || !camera->getRenderTarget())
            {
                std::vector<math::Vector<float, 2>> worldEdges;
                worldEdges.reserve(edges.size());

                for (const auto& edge : edges)
                    worldEdges.emplace_back(camera->convertNormalizedToWorld(edge));

                const auto actors = findActors(worldEdges);
                result.insert(result.end(), actors.begin(), actors.end());
            }
        }

        return result;
    }

    void Layer::setOrder(std::int32_t newOrder)
    {
        order = newOrder;
    }

    void Layer::recalculateProjection()
    {
        for (const auto camera : cameras)
            camera->recalculateProjection();
    }

    void Layer::enter()
    {
        ActorContainer::enter();

        recalculateProjection();
    }

    void Layer::removeFromScene()
    {
        if (scene) scene->removeLayer(*this);
    }
}
