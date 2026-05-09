#ifndef FLUXION_REFLECTION_INSPECTOR_HPP
#define FLUXION_REFLECTION_INSPECTOR_HPP

#include <cstddef>
#include <string>
#include "../math/Color.hpp"
#include "../math/Vector.hpp"

namespace fluxion::assets
{
    // Forward decl: kept opaque at the reflection layer so the
    // Inspector header stays free of asset-pipeline dependencies.
    // Concrete inspector implementations include Asset.hpp themselves
    // when they need to read or write an AssetReference.
    struct AssetReference;
}

namespace fluxion::reflection
{
    // UI-independent abstraction over a property editor. The same Inspector
    // method calls used by an editor panel (ImGuiInspector) are also the
    // hook point for a JSON serializer (later phase). A component declares
    // its inspectable properties by overriding Component::inspect and
    // calling these methods in order; the Inspector implementation decides
    // whether to render a widget, write JSON, etc.
    class Inspector
    {
    public:
        virtual ~Inspector() = default;

        Inspector(const Inspector&) = delete;
        Inspector& operator=(const Inspector&) = delete;
        Inspector(Inspector&&) = delete;
        Inspector& operator=(Inspector&&) = delete;

        // Scalar / primitive fields
        virtual void floatField(const char* label, float& value) = 0;
        virtual void floatRangeField(const char* label, float& value, float minValue, float maxValue) = 0;
        virtual void intField(const char* label, int& value) = 0;
        virtual void intRangeField(const char* label, int& value, int minValue, int maxValue) = 0;
        virtual void boolField(const char* label, bool& value) = 0;
        virtual void stringField(const char* label, std::string& value) = 0;

        // Math types
        virtual void colorField(const char* label, math::Color& value) = 0;
        virtual void vector2Field(const char* label, math::Vector<float, 2>& value) = 0;
        virtual void vector3Field(const char* label, math::Vector<float, 3>& value) = 0;
        virtual void vector4Field(const char* label, math::Vector<float, 4>& value) = 0;

        // Discrete choice from a fixed set of labels (options[0]..options[count-1]).
        // value is interpreted as an index into options.
        virtual void enumField(const char* label, int& value, const char* const* options, std::size_t count) = 0;

        // Reference to an AssetDatabase-tracked resource (texture,
        // sprite, sound, ...). Persisted as the asset's stable Uuid;
        // the editor renders a draggable receptor that resolves the
        // id back to a human-readable name. Default impl is a no-op
        // so legacy inspector subclasses that don't care still link
        // — concrete inspectors that DO care (JsonInspector,
        // ImGuiInspector, ...) override this.
        virtual void assetField(const char* /*label*/,
                                assets::AssetReference& /*value*/) {}

        // One-shot button. Returns true exactly on the frame the
        // user clicked it; the inspect() body should react inline
        // (call play(), reset(), ...). Persisting inspectors
        // (JsonInspector) and silent ones return false so the
        // action never fires during save/load — buttons are pure
        // UI affordance, not state.
        [[nodiscard]] virtual bool actionField(const char* /*label*/) { return false; }

        // Read-only display of derived state (e.g., "boundingBox")
        virtual void readOnlyText(const char* label, const char* text) = 0;

        // Visual grouping. Implementations may render a header, indent, etc.
        virtual void beginGroup(const char* label) = 0;
        virtual void endGroup() = 0;

    protected:
        Inspector() = default;
    };
}

#endif // FLUXION_REFLECTION_INSPECTOR_HPP
