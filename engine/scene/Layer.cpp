#include <cassert>
#include <algorithm>
#include <utility>
#include "Layer.hpp"
#include "Scene.hpp"
#include "Component.hpp"
#include "SortingLayers.hpp"
#include "../core/Engine.hpp"
#include "../graphics/Graphics.hpp"
#include "../math/Matrix.hpp"

namespace fluxion::scene
{
    Layer::Layer()
    {
        layer = this;
    }

    Layer::~Layer()
    {
        if (scene) scene->removeLayer(*this);
    }

    namespace
    {
        // Sort key for an actor in the draw queue. The renderer's
        // sortingLayer + orderInLayer live on Component (any
        // component, but in practice only the renderer subclasses
        // care about z-order); we pick the first component that
        // sets non-default values, falling back to "Default" / 0
        // for actors with only-defaulted (or missing) renderers.
        // That keeps existing samples drawing in their old order
        // while new sprites can be reordered explicitly.
        std::pair<std::size_t, int> drawSortKey(const Actor& actor) noexcept
        {
            for (const auto* comp : actor.getComponents())
            {
                if (!comp) continue;
                const auto& layerName = comp->getSortingLayer();
                const auto orderInLayer = comp->getOrderInLayer();
                if (layerName != "Default" || orderInLayer != 0)
                    return {sortingLayers::indexOf(layerName), orderInLayer};
            }
            return {sortingLayers::indexOfDefault(), 0};
        }
    }

    void Layer::draw()
    {
        for (const auto camera : cameras)
        {
            std::vector<Actor*> drawQueue;

            for (const auto actor : children)
                actor->visit(drawQueue, math::identityMatrix<float, 4>, false, camera, 0, false);

            // Stable-sort by (sortingLayerIndex, orderInLayer) so
            // the visit-order tie-break is preserved for actors
            // that share a sort key. Stable_sort is O(n log n) and
            // the queue is typically small (per-camera per-frame).
            std::stable_sort(drawQueue.begin(), drawQueue.end(),
                             [](const Actor* a, const Actor* b) {
                                 return drawSortKey(*a) < drawSortKey(*b);
                             });

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

        return std::pair{nullptr, math::Vector<float, 3>{}};
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

    void Layer::setOrder(Order newOrder) noexcept
    {
        order = newOrder;
    }

    void Layer::calculateProjection()
    {
        for (const auto camera : cameras)
            camera->calculateProjection();
    }

    void Layer::enter()
    {
        ActorContainer::enter();

        calculateProjection();
    }

    void Layer::removeFromScene()
    {
        if (scene) scene->removeLayer(*this);
    }
}
