#include "ComponentCommands.hpp"

#include "../SceneLookup.hpp"
#include "../../engine/scene/Actor.hpp"
#include "../../engine/scene/Component.hpp"
#include "../../engine/scene/Layer.hpp"

namespace fluxion::editor::commands
{
    namespace
    {
        scene::Actor* resolve(scene::Layer* layer, const Uuid& id)
        {
            if (!layer) return nullptr;
            return findActorById(*layer, id);
        }
    }

    // ---------- AddComponentCommand ----------------------------------

    AddComponentCommand::AddComponentCommand(scene::Layer* layer,
                                             const Uuid& actorId,
                                             std::unique_ptr<scene::Component> component):
        layer_{layer},
        actorId_{actorId},
        typeName_{component ? std::string{component->typeName()} : std::string{}},
        storedComponent_{std::move(component)},
        attachedRef_{nullptr}
    {}

    void AddComponentCommand::redo()
    {
        if (!storedComponent_) return;  // already attached
        auto* actor = resolve(layer_, actorId_);
        if (!actor) return;
        attachedRef_ = storedComponent_.get();
        actor->addComponent(std::move(storedComponent_));
    }

    void AddComponentCommand::undo()
    {
        if (storedComponent_) return;  // not attached
        auto* actor = resolve(layer_, actorId_);
        if (!actor || !attachedRef_) return;
        storedComponent_ = actor->releaseComponent(*attachedRef_);
        attachedRef_ = nullptr;
    }

    // ---------- RemoveComponentCommand -------------------------------

    RemoveComponentCommand::RemoveComponentCommand(scene::Layer* layer,
                                                   const Uuid& actorId,
                                                   scene::Component* component):
        layer_{layer},
        actorId_{actorId},
        typeName_{component ? std::string{component->typeName()} : std::string{}},
        storedComponent_{},
        attachedRef_{component}
    {}

    void RemoveComponentCommand::redo()
    {
        if (storedComponent_) return;  // already detached
        auto* actor = resolve(layer_, actorId_);
        if (!actor || !attachedRef_) return;
        storedComponent_ = actor->releaseComponent(*attachedRef_);
        // attachedRef_ now dangles in the sense that the actor
        // doesn't hold the component any more; storedComponent_
        // does, with the same address. Keep attachedRef_ pointing
        // at the same instance so undo / next-redo cycles stay
        // symmetric.
    }

    void RemoveComponentCommand::undo()
    {
        if (!storedComponent_) return;  // not currently detached
        auto* actor = resolve(layer_, actorId_);
        if (!actor) return;
        attachedRef_ = storedComponent_.get();
        actor->addComponent(std::move(storedComponent_));
    }
}
