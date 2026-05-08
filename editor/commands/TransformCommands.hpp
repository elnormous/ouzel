#ifndef FLUXION_EDITOR_COMMANDS_TRANSFORMCOMMANDS_HPP
#define FLUXION_EDITOR_COMMANDS_TRANSFORMCOMMANDS_HPP

#include <string>
#include "Command.hpp"
#include "../../engine/math/Quaternion.hpp"
#include "../../engine/math/Vector.hpp"
#include "../../engine/utils/Uuid.hpp"

namespace fluxion::scene
{
    class Layer;
}

namespace fluxion::editor::commands
{
    // Transform commands all share the same shape: an actor uuid +
    // a layer* used for lookup, a "before" value captured at gesture
    // start, and an "after" value captured at gesture end. tryMerge
    // collapses runs of same-actor commands so a single drag stays
    // one undo step regardless of how many commits the input layer
    // chooses to fire — even though the editor today pushes once
    // per drag, the merge support keeps Inspector field edits
    // (which can fire per-frame) sane too once 9b wires those in.

    class SetPositionCommand final : public Command
    {
    public:
        SetPositionCommand(scene::Layer* layer, const Uuid& actorId,
                           math::Vector<float, 3> oldValue,
                           math::Vector<float, 3> newValue);

        void redo() override;
        void undo() override;
        [[nodiscard]] std::string label() const override { return "Move Actor"; }
        [[nodiscard]] bool tryMerge(const Command& other) override;

    private:
        scene::Layer* layer_;
        Uuid actorId_;
        math::Vector<float, 3> oldValue_;
        math::Vector<float, 3> newValue_;
    };

    class SetRotationCommand final : public Command
    {
    public:
        SetRotationCommand(scene::Layer* layer, const Uuid& actorId,
                           math::Quaternion<float> oldValue,
                           math::Quaternion<float> newValue);

        void redo() override;
        void undo() override;
        [[nodiscard]] std::string label() const override { return "Rotate Actor"; }
        [[nodiscard]] bool tryMerge(const Command& other) override;

    private:
        scene::Layer* layer_;
        Uuid actorId_;
        math::Quaternion<float> oldValue_;
        math::Quaternion<float> newValue_;
    };

    class SetScaleCommand final : public Command
    {
    public:
        SetScaleCommand(scene::Layer* layer, const Uuid& actorId,
                        math::Vector<float, 3> oldValue,
                        math::Vector<float, 3> newValue);

        void redo() override;
        void undo() override;
        [[nodiscard]] std::string label() const override { return "Scale Actor"; }
        [[nodiscard]] bool tryMerge(const Command& other) override;

    private:
        scene::Layer* layer_;
        Uuid actorId_;
        math::Vector<float, 3> oldValue_;
        math::Vector<float, 3> newValue_;
    };

    // Actor's display name (the std::string returned by getName).
    // Inspector pushes one of these on InputText commit
    // (IsItemDeactivatedAfterEdit) with the value captured the
    // moment the field became active.
    class SetNameCommand final : public Command
    {
    public:
        SetNameCommand(scene::Layer* layer, const Uuid& actorId,
                       std::string oldValue, std::string newValue);

        void redo() override;
        void undo() override;
        [[nodiscard]] std::string label() const override { return "Rename Actor"; }
        [[nodiscard]] bool tryMerge(const Command& other) override;

    private:
        scene::Layer* layer_;
        Uuid actorId_;
        std::string oldValue_;
        std::string newValue_;
    };
}

#endif // FLUXION_EDITOR_COMMANDS_TRANSFORMCOMMANDS_HPP
