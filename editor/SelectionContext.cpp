#include "SelectionContext.hpp"

#include <algorithm>
#include <utility>
#include "../engine/scene/Actor.hpp"
#include "../engine/scene/Layer.hpp"
#include "../engine/scene/Scene.hpp"

namespace fluxion::editor
{
    namespace
    {
        // Recursive search: returns true iff `target` is a child or
        // descendant of `container`. Used by validateAgainst to verify
        // the selected actor is still anywhere in the scene tree.
        bool containsActor(const scene::ActorContainer& container,
                           const scene::Actor* target)
        {
            for (const auto* child : container.getChildren())
            {
                if (child == target) return true;
                if (containsActor(*child, target)) return true;
            }
            return false;
        }
    }

    void SelectionContext::setSelected(scene::Actor* actor)
    {
        if (selected_ == actor) return;
        selected_ = actor;
        notify();
    }

    void SelectionContext::validateAgainst(const scene::Scene& scene)
    {
        if (!selected_) return;
        for (const auto* layer : scene.getLayers())
            if (containsActor(*layer, selected_)) return;
        // Selected actor is no longer in the scene; clear and notify.
        selected_ = nullptr;
        notify();
    }

    SelectionContext::ListenerId SelectionContext::addListener(Listener listener)
    {
        const auto id = nextListenerId_++;
        listeners_.push_back(ListenerSlot{id, std::move(listener)});
        return id;
    }

    void SelectionContext::removeListener(ListenerId id)
    {
        const auto it = std::find_if(listeners_.begin(), listeners_.end(),
            [id](const ListenerSlot& s) { return s.id == id; });
        if (it != listeners_.end()) listeners_.erase(it);
    }

    void SelectionContext::notify()
    {
        // Iterate over a snapshot so a listener can safely add/remove
        // listeners without invalidating iterators on the live vector.
        const auto snapshot = listeners_;
        for (const auto& slot : snapshot)
            if (slot.fn) slot.fn(selected_);
    }
}
