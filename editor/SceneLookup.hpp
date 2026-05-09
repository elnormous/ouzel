#ifndef FLUXION_EDITOR_SCENELOOKUP_HPP
#define FLUXION_EDITOR_SCENELOOKUP_HPP

#include "../engine/utils/Uuid.hpp"

namespace fluxion::scene
{
    class Actor;
    class ActorContainer;
}

namespace fluxion::editor
{
    // Recursive O(N) search for an actor by Uuid. Used by undo/redo
    // commands so they store stable identifiers instead of raw
    // pointers — pointers don't survive a delete-then-undo round
    // trip (the new actor gets a different heap address) but the
    // Uuid does, since the deserialiser restores the original id.
    //
    // Returns nullptr when the id isn't found in the subtree, which
    // commands treat as "target gone, no-op cleanly". Suitable for
    // editor-scale scenes (<= a few thousand actors); if the cost
    // ever shows up in a profile, the editor can switch to a
    // Uuid -> Actor* map maintained as actors enter/leave the scene.
    [[nodiscard]] scene::Actor* findActorById(scene::ActorContainer& root,
                                              const Uuid& id);
}

#endif // FLUXION_EDITOR_SCENELOOKUP_HPP
