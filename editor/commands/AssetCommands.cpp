#include "AssetCommands.hpp"

#include <cstring>
#include "../../engine/reflection/Inspector.hpp"
#include "../../engine/scene/Actor.hpp"
#include "../../engine/scene/Component.hpp"
#include "../../engine/scene/Layer.hpp"
#include "../SceneLookup.hpp"

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
}
