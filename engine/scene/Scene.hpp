// Ouzel by Elviss Strazdins

#ifndef OUZEL_SCENE_SCENE_HPP
#define OUZEL_SCENE_SCENE_HPP

#include <vector>
#include <unordered_map>
#include <cstdint>
#include "../events/EventHandler.hpp"
#include "../math/Vector.hpp"

namespace ouzel::scene
{
    class SceneManager;
    class Layer;

    class Scene
    {
        friend SceneManager;
    public:
        Scene();
        virtual ~Scene();

        Scene(const Scene&) = delete;
        Scene& operator=(const Scene&) = delete;

        Scene(Scene&&) = delete;
        Scene& operator=(Scene&&) = delete;

        virtual void draw();

        void addLayer(Layer& layer);

        template <class T> void addLayer(std::unique_ptr<T> layer)
        {
            addLayer(layer.get());
            ownedLayers.push_back(std::move(layer));
        }

        bool removeLayer(const Layer& layer);

        void removeAllLayers();
        bool hasLayer(const Layer& layer) const;
        auto& getLayers() const noexcept { return layers; }

        virtual void calculateProjection();

        std::pair<Actor*, math::Vector<float, 3>> pickActor(const math::Vector<float, 2>& position,
                                                            const bool renderTargets = false) const;
        std::vector<std::pair<Actor*, math::Vector<float, 3>>> pickActors(const math::Vector<float, 2>& position,
                                                                          const bool renderTargets = false) const;
        std::vector<Actor*> pickActors(const std::vector<math::Vector<float, 2>>& edges,
                                       const bool renderTargets = false) const;

    protected:
        virtual void enter();
        virtual void leave();

        bool handleMouse(const MouseEvent& event);
        bool handleTouch(const TouchEvent& event);

        void pointerEnterActor(std::uint64_t pointerId, Actor* actor,
                               const math::Vector<float, 2>& position);
        void pointerLeaveActor(std::uint64_t pointerId, Actor* actor,
                               const math::Vector<float, 2>& position);
        void pointerDownOnActor(std::uint64_t pointerId, Actor* actor,
                                const math::Vector<float, 2>& position,
                                const math::Vector<float, 3>& localPosition);
        void pointerUpOnActor(std::uint64_t pointerId, Actor* actor,
                              const math::Vector<float, 2>& position);
        void pointerDragActor(std::uint64_t pointerId, Actor* actor,
                              const math::Vector<float, 2>& position,
                              const math::Vector<float, 2>& difference,
                              const math::Vector<float, 3>& localPosition);

        SceneManager* sceneManger = nullptr;

        std::vector<Layer*> layers;
        std::vector<std::unique_ptr<Layer>> ownedLayers;
        EventHandler eventHandler;

        std::unordered_map<std::uint64_t, std::pair<Actor*, math::Vector<float, 3>>> pointerDownOnActors;

        bool entered = false;
    };
}

#endif // OUZEL_SCENE_SCENE_HPP
