#include "AssetCommands.hpp"

#include <cstring>
#include <utility>
#include "../../engine/reflection/Inspector.hpp"
#include "../../engine/scene/Actor.hpp"
#include "../../engine/scene/Component.hpp"
#include "../../engine/scene/Layer.hpp"
#include "../../engine/scene/SpriteRenderer.hpp"
#include "../SceneLookup.hpp"
#include "../SelectionContext.hpp"

namespace fluxion::editor::commands
{
    namespace
    {
        // Walks a Component's inspect() and overwrites the single
        // AssetReference field whose label matches `target_`. Every
        // other inspect callback is a deliberate no-op so the writer
        // touches nothing it isn't supposed to.
        //
        // Why this dance? AssetReferences live inside components and
        // are exposed through inspect(); the engine deliberately has
        // no field-pointer mechanism, so the only way to write a
        // single field by name is to drive inspect() with a tailored
        // Inspector subclass. The same shape will work for any other
        // "set one named field" command in the future.
        class SingleAssetFieldWriter final : public reflection::Inspector
        {
        public:
            SingleAssetFieldWriter(const char* target,
                                   assets::AssetReference value) noexcept:
                target_{target}, value_{value} {}

            void floatField(const char*, float&) override {}
            void floatRangeField(const char*, float&, float, float) override {}
            void intField(const char*, int&) override {}
            void intRangeField(const char*, int&, int, int) override {}
            void boolField(const char*, bool&) override {}
            void stringField(const char*, std::string&) override {}

            void colorField(const char*, math::Color&) override {}
            void vector2Field(const char*, math::Vector<float, 2>&) override {}
            void vector3Field(const char*, math::Vector<float, 3>&) override {}
            void vector4Field(const char*, math::Vector<float, 4>&) override {}

            void enumField(const char*, int&, const char* const*, std::size_t) override {}

            void assetField(const char* label, assets::AssetReference& current) override
            {
                if (std::strcmp(label, target_) == 0) current = value_;
            }

            void readOnlyText(const char*, const char*) override {}
            void beginGroup(const char*) override {}
            void endGroup() override {}

        private:
            const char* target_;
            assets::AssetReference value_;
        };
    }

    SetAssetCommand::SetAssetCommand(scene::Layer* userLayer,
                                     Uuid actorId,
                                     std::size_t componentIndex,
                                     std::string fieldLabel,
                                     assets::AssetReference oldValue,
                                     assets::AssetReference newValue) noexcept:
        userLayer_{userLayer},
        actorId_{actorId},
        componentIndex_{componentIndex},
        fieldLabel_{std::move(fieldLabel)},
        oldValue_{oldValue},
        newValue_{newValue}
    {
    }

    void SetAssetCommand::applyValue(const assets::AssetReference& value) const
    {
        if (!userLayer_) return;
        auto* actor = findActorById(*userLayer_, actorId_);
        if (!actor) return;
        const auto& comps = actor->getComponents();
        if (componentIndex_ >= comps.size()) return;
        auto* comp = comps[componentIndex_];
        if (!comp) return;

        SingleAssetFieldWriter writer{fieldLabel_.c_str(), value};
        comp->inspect(writer);
    }

    void SetAssetCommand::redo() { applyValue(newValue_); }
    void SetAssetCommand::undo() { applyValue(oldValue_); }

    std::string SetAssetCommand::label() const
    {
        return "Set " + fieldLabel_;
    }

    // ---- CreateActorWithSpriteCommand -------------------------------

    CreateActorWithSpriteCommand::CreateActorWithSpriteCommand(
        scene::Layer* layer,
        const assets::AssetReference& assetRef,
        const math::Vector<float, 3>& worldPosition,
        std::string actorName,
        SelectionContext* selection):
        layer_{layer},
        selection_{selection}
    {
        // Build the actor up-front. The ctor runs once when the user
        // drops; redo() then alternates between handing it to the
        // layer and reclaiming it on undo, mirroring AddActorCommand.
        stored_ = std::make_unique<scene::Actor>();
        stored_->setName(std::move(actorName));
        stored_->setPosition(worldPosition);

        auto renderer = std::make_unique<scene::SpriteRenderer>();
        renderer->setSpriteReference(assetRef);
        stored_->addComponent(std::move(renderer));

        actorId_ = stored_->getId();
    }

    CreateActorWithSpriteCommand::~CreateActorWithSpriteCommand() = default;

    void CreateActorWithSpriteCommand::redo()
    {
        if (!stored_) return;        // already in scene
        if (!layer_) return;
        layer_->addChild(std::move(stored_));
        if (selection_)
        {
            // Land the new actor as the user's selection so the
            // Inspector immediately exposes its SpriteRenderer for
            // tweaks (sorting layer, offset, ...).
            if (auto* a = findActorById(*layer_, actorId_))
                selection_->setSelected(a);
        }
    }

    void CreateActorWithSpriteCommand::undo()
    {
        if (stored_) return;          // already detached
        if (!layer_) return;
        auto* victim = findActorById(*layer_, actorId_);
        if (!victim) return;
        if (selection_ && selection_->getSelected() == victim)
            selection_->clear();
        stored_ = layer_->releaseChild(*victim);
    }

    std::string CreateActorWithSpriteCommand::label() const
    {
        return "Create Sprite Actor";
    }
}
