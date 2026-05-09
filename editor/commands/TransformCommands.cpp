#include "TransformCommands.hpp"

#include "../SceneLookup.hpp"
#include "../../engine/scene/Actor.hpp"
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

    // ---------- Position --------------------------------------------

    SetPositionCommand::SetPositionCommand(scene::Layer* layer,
                                           const Uuid& actorId,
                                           math::Vector<float, 3> oldValue,
                                           math::Vector<float, 3> newValue):
        layer_{layer}, actorId_{actorId},
        oldValue_{oldValue}, newValue_{newValue} {}

    void SetPositionCommand::redo()
    {
        if (auto* a = resolve(layer_, actorId_)) a->setPosition(newValue_);
    }
    void SetPositionCommand::undo()
    {
        if (auto* a = resolve(layer_, actorId_)) a->setPosition(oldValue_);
    }
    bool SetPositionCommand::tryMerge(const Command& other)
    {
        const auto* o = dynamic_cast<const SetPositionCommand*>(&other);
        if (!o) return false;
        if (o->layer_ != layer_ || !(o->actorId_ == actorId_)) return false;
        // Extend our newValue_ to cover the merged-in command's
        // target. oldValue_ stays at the oldest captured value, so
        // a single undo reverts the entire continuous run.
        newValue_ = o->newValue_;
        return true;
    }

    // ---------- Rotation --------------------------------------------

    SetRotationCommand::SetRotationCommand(scene::Layer* layer,
                                           const Uuid& actorId,
                                           math::Quaternion<float> oldValue,
                                           math::Quaternion<float> newValue):
        layer_{layer}, actorId_{actorId},
        oldValue_{oldValue}, newValue_{newValue} {}

    void SetRotationCommand::redo()
    {
        if (auto* a = resolve(layer_, actorId_)) a->setRotation(newValue_);
    }
    void SetRotationCommand::undo()
    {
        if (auto* a = resolve(layer_, actorId_)) a->setRotation(oldValue_);
    }
    bool SetRotationCommand::tryMerge(const Command& other)
    {
        const auto* o = dynamic_cast<const SetRotationCommand*>(&other);
        if (!o) return false;
        if (o->layer_ != layer_ || !(o->actorId_ == actorId_)) return false;
        newValue_ = o->newValue_;
        return true;
    }

    // ---------- Scale -----------------------------------------------

    SetScaleCommand::SetScaleCommand(scene::Layer* layer,
                                     const Uuid& actorId,
                                     math::Vector<float, 3> oldValue,
                                     math::Vector<float, 3> newValue):
        layer_{layer}, actorId_{actorId},
        oldValue_{oldValue}, newValue_{newValue} {}

    void SetScaleCommand::redo()
    {
        if (auto* a = resolve(layer_, actorId_)) a->setScale(newValue_);
    }
    void SetScaleCommand::undo()
    {
        if (auto* a = resolve(layer_, actorId_)) a->setScale(oldValue_);
    }
    bool SetScaleCommand::tryMerge(const Command& other)
    {
        const auto* o = dynamic_cast<const SetScaleCommand*>(&other);
        if (!o) return false;
        if (o->layer_ != layer_ || !(o->actorId_ == actorId_)) return false;
        newValue_ = o->newValue_;
        return true;
    }

    // ---------- Name ------------------------------------------------

    SetNameCommand::SetNameCommand(scene::Layer* layer,
                                   const Uuid& actorId,
                                   std::string oldValue,
                                   std::string newValue):
        layer_{layer}, actorId_{actorId},
        oldValue_{std::move(oldValue)}, newValue_{std::move(newValue)} {}

    void SetNameCommand::redo()
    {
        if (auto* a = resolve(layer_, actorId_)) a->setName(newValue_);
    }
    void SetNameCommand::undo()
    {
        if (auto* a = resolve(layer_, actorId_)) a->setName(oldValue_);
    }
    bool SetNameCommand::tryMerge(const Command& other)
    {
        const auto* o = dynamic_cast<const SetNameCommand*>(&other);
        if (!o) return false;
        if (o->layer_ != layer_ || !(o->actorId_ == actorId_)) return false;
        newValue_ = o->newValue_;
        return true;
    }
}
