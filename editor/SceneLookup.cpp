#include "SceneLookup.hpp"

#include "../engine/scene/Actor.hpp"

namespace fluxion::editor
{
    scene::Actor* findActorById(scene::ActorContainer& root, const Uuid& id)
    {
        for (auto* child : root.getChildren())
        {
            if (!child) continue;
            if (child->getId() == id) return child;
            if (auto* nested = findActorById(*child, id)) return nested;
        }
        return nullptr;
    }
}
