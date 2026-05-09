#ifndef FLUXION_EDITOR_COMMANDS_COMPONENTCOMMANDS_HPP
#define FLUXION_EDITOR_COMMANDS_COMPONENTCOMMANDS_HPP

#include <memory>
#include <string>
#include "Command.hpp"
#include "../../engine/utils/Uuid.hpp"

namespace fluxion::scene
{
    class Component;
    class Layer;
}

namespace fluxion::editor::commands
{
    // Adds a freshly-built Component to an actor (looked up by uuid
    // in the user layer). The Component's serialised state isn't
    // captured for the round trip — only the type identifier is.
    // On undo, the live Component instance is removed from the
    // actor and held inside the command (preserving any user-tweaked
    // properties); redo re-attaches it. Multiple round trips
    // therefore preserve all in-Inspector edits made between the
    // initial add and the first undo.
    class AddComponentCommand final : public Command
    {
    public:
        // The new component is constructed via TypeRegistry::create
        // by the caller and handed off here. The command owns it
        // until redo() attaches it.
        AddComponentCommand(scene::Layer* layer,
                            const Uuid& actorId,
                            std::unique_ptr<scene::Component> component);

        void redo() override;
        void undo() override;
        [[nodiscard]] std::string label() const override { return "Add Component"; }

    private:
        scene::Layer* layer_;
        Uuid actorId_;
        // Type for the user-facing label and as the key the
        // Inspector / TypeRegistry would use to recreate this
        // component if we ever need a deep clone (not used today —
        // we reattach the same instance on redo / undo).
        std::string typeName_;
        // alternates owned (when detached) / null (when attached
        // to actor).
        std::unique_ptr<scene::Component> storedComponent_;
        // Raw pointer captured the first time redo() runs. Stable
        // even when storedComponent_ is null (the Actor owns the
        // Component, no copies are made).
        scene::Component* attachedRef_ = nullptr;
    };

    class RemoveComponentCommand final : public Command
    {
    public:
        // The component must currently be attached to the actor; we
        // capture its pointer for re-attach on undo and detach on
        // first redo.
        RemoveComponentCommand(scene::Layer* layer,
                               const Uuid& actorId,
                               scene::Component* component);

        void redo() override;
        void undo() override;
        [[nodiscard]] std::string label() const override { return "Remove Component"; }

    private:
        scene::Layer* layer_;
        Uuid actorId_;
        std::string typeName_;
        std::unique_ptr<scene::Component> storedComponent_;
        scene::Component* attachedRef_;
    };
}

#endif // FLUXION_EDITOR_COMMANDS_COMPONENTCOMMANDS_HPP
