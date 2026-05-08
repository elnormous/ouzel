#include "ActorCommands.hpp"

#include "../SceneLookup.hpp"
#include "../SelectionContext.hpp"
#include "../../engine/scene/Actor.hpp"
#include "../../engine/scene/Layer.hpp"

namespace fluxion::editor::commands
{
    namespace
    {
        // Resolve the parent container for an actor that's currently
        // attached. Returns the layer when parentActorId is null,
        // otherwise looks the parent actor up by id. Returns nullptr
        // when the parent isn't reachable any more (e.g., it was
        // deleted via a non-undoable path) — callers no-op in that
        // case.
        scene::ActorContainer* resolveParent(scene::Layer* layer,
                                             const Uuid& parentActorId)
        {
            if (!layer) return nullptr;
            if (parentActorId.isNull()) return layer;
            return findActorById(*layer, parentActorId);
        }
    }

    // ---------- AddActorCommand ----------------------------------

    AddActorCommand::AddActorCommand(scene::Layer* parentLayer,
                                     std::unique_ptr<scene::Actor> newActor,
                                     SelectionContext* selection):
        parentLayer_{parentLayer},
        parentActorId_{},
        newActorId_{newActor ? newActor->getId() : Uuid{}},
        storedActor_{std::move(newActor)},
        selection_{selection}
    {}

    AddActorCommand::AddActorCommand(scene::Layer* parentLayer,
                                     const Uuid& parentActorId,
                                     std::unique_ptr<scene::Actor> newActor,
                                     SelectionContext* selection):
        parentLayer_{parentLayer},
        parentActorId_{parentActorId},
        newActorId_{newActor ? newActor->getId() : Uuid{}},
        storedActor_{std::move(newActor)},
        selection_{selection}
    {}

    void AddActorCommand::redo()
    {
        if (!storedActor_) return;  // already in scene
        auto* parent = resolveParent(parentLayer_, parentActorId_);
        if (!parent) return;
        parent->addChild(std::move(storedActor_));
    }

    void AddActorCommand::undo()
    {
        if (storedActor_) return;  // already detached
        auto* parent = resolveParent(parentLayer_, parentActorId_);
        if (!parent) return;
        auto* victim = findActorById(*parent, newActorId_);
        if (!victim) return;
        // Drop selection eagerly when we're about to detach the
        // selected actor so the inspector / overlay don't read a
        // stale pointer between this undo() and the next frame's
        // SelectionContext::validateAgainst sweep.
        if (selection_ && selection_->getSelected() == victim)
            selection_->clear();
        storedActor_ = parent->releaseChild(*victim);
    }

    std::string AddActorCommand::label() const
    {
        return parentActorId_.isNull() ? "Add Actor" : "Add Child Actor";
    }

    // ---------- DeleteActorCommand -------------------------------

    DeleteActorCommand::DeleteActorCommand(scene::Layer* parentLayer,
                                           const Uuid& actorId,
                                           SelectionContext* selection):
        parentLayer_{parentLayer},
        actorId_{actorId},
        selection_{selection}
    {}

    void DeleteActorCommand::redo()
    {
        if (storedActor_) return;  // already detached
        if (!parentLayer_) return;

        // First-time redo: walk the layer to find both the actor and
        // its current parent. Capture the parent's id (or null when
        // the parent IS the layer) so subsequent redo/undo cycles
        // re-resolve cheaply.
        auto* victim = findActorById(*parentLayer_, actorId_);
        if (!victim) return;

        if (!parentResolved_)
        {
            auto* parentRaw = victim->getParent();
            if (auto* parentActor = dynamic_cast<scene::Actor*>(parentRaw))
                parentActorId_ = parentActor->getId();
            else
                parentActorId_ = Uuid{};  // null = layer
            parentResolved_ = true;
        }

        auto* parent = resolveParent(parentLayer_, parentActorId_);
        if (!parent) return;

        if (selection_ && selection_->getSelected() == victim)
            selection_->clear();
        storedActor_ = parent->releaseChild(*victim);
    }

    void DeleteActorCommand::undo()
    {
        if (!storedActor_) return;  // not currently detached
        auto* parent = resolveParent(parentLayer_, parentActorId_);
        if (!parent) return;
        parent->addChild(std::move(storedActor_));
    }

    std::string DeleteActorCommand::label() const
    {
        return "Delete Actor";
    }
}
