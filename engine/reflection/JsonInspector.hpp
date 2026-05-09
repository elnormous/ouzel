#ifndef FLUXION_REFLECTION_JSONINSPECTOR_HPP
#define FLUXION_REFLECTION_JSONINSPECTOR_HPP

#include <vector>
#include "Inspector.hpp"
#include "../formats/Json.hpp"

namespace fluxion::scene
{
    class Component;
}

namespace fluxion::reflection
{
    // Convenience: drive a component's inspect() with a JsonWriter / JsonReader
    // so callers don't have to know which Inspector concrete type to plug in.
    // Because inspect() is the single source of truth for a component's
    // editable + persisted state, calling these is functionally equivalent
    // to "save this component" / "load into this component".
    void serializeComponent(scene::Component& component, json::Value& out);
    void deserializeComponent(scene::Component& component, const json::Value& in);


    // JsonWriter walks a Component's inspect() callbacks and writes each
    // field into a json::Value object tree. Designed so the same inspect()
    // body that drives the editor UI also drives serialization — there is
    // no second list of "what to save" to keep in sync.
    //
    // Construct with a json::Value that is either default-initialised (the
    // writer turns it into a json Object) or already an Object. After the
    // call, the underlying value contains one key per inspected field,
    // plus nested objects for beginGroup/endGroup ranges.
    class JsonWriter final : public Inspector
    {
    public:
        explicit JsonWriter(json::Value& root);

        void floatField(const char* label, float& value) override;
        void floatRangeField(const char* label, float& value, float minValue, float maxValue) override;
        void intField(const char* label, int& value) override;
        void intRangeField(const char* label, int& value, int minValue, int maxValue) override;
        void boolField(const char* label, bool& value) override;
        void stringField(const char* label, std::string& value) override;

        void colorField(const char* label, math::Color& value) override;
        void vector2Field(const char* label, math::Vector<float, 2>& value) override;
        void vector3Field(const char* label, math::Vector<float, 3>& value) override;
        void vector4Field(const char* label, math::Vector<float, 4>& value) override;

        void enumField(const char* label, int& value, const char* const* options, std::size_t count) override;

        void assetField(const char* label, assets::AssetReference& value) override;

        // Read-only fields are derived state and intentionally not written.
        void readOnlyText(const char* /*label*/, const char* /*text*/) override {}

        void beginGroup(const char* label) override;
        void endGroup() override;

    private:
        json::Value& current() noexcept { return *stack_.back(); }
        std::vector<json::Value*> stack_;
    };

    // JsonReader is the symmetric counterpart. It looks up each field by
    // label in the json Object and writes the parsed value into the
    // component's reference. Missing fields and type mismatches are
    // tolerated: the corresponding output stays at its current value, so
    // a component can be loaded from an older schema and the new fields
    // keep their constructor defaults. This is the policy that lets us
    // evolve component schemas without breaking saved scenes.
    class JsonReader final : public Inspector
    {
    public:
        explicit JsonReader(const json::Value& root);

        void floatField(const char* label, float& value) override;
        void floatRangeField(const char* label, float& value, float minValue, float maxValue) override;
        void intField(const char* label, int& value) override;
        void intRangeField(const char* label, int& value, int minValue, int maxValue) override;
        void boolField(const char* label, bool& value) override;
        void stringField(const char* label, std::string& value) override;

        void colorField(const char* label, math::Color& value) override;
        void vector2Field(const char* label, math::Vector<float, 2>& value) override;
        void vector3Field(const char* label, math::Vector<float, 3>& value) override;
        void vector4Field(const char* label, math::Vector<float, 4>& value) override;

        void enumField(const char* label, int& value, const char* const* options, std::size_t count) override;

        void assetField(const char* label, assets::AssetReference& value) override;

        void readOnlyText(const char* /*label*/, const char* /*text*/) override {}

        void beginGroup(const char* label) override;
        void endGroup() override;

    private:
        // null-tolerant accessor: returns nullptr if the current group has
        // no member with this name, or if the member's type isn't the one
        // the caller will read out via `is<T>()`.
        const json::Value* find(const char* label) const noexcept;

        // Parallel stacks: the actual current Value, and a sentinel for
        // "the parent didn't have this group" so endGroup can pop without
        // reading invalid memory. nullptr in stack_ means "this scope is
        // missing from the JSON; treat all reads as defaults".
        std::vector<const json::Value*> stack_;
    };
}

#endif // FLUXION_REFLECTION_JSONINSPECTOR_HPP
