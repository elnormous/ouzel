#ifndef FLUXION_GUI_IMGUI_IMGUIINSPECTOR_HPP
#define FLUXION_GUI_IMGUI_IMGUIINSPECTOR_HPP

#include "../../reflection/Inspector.hpp"

namespace fluxion::gui::imgui
{
    // Renders a Component's reflected properties using Dear ImGui widgets.
    // All methods must be called from the render thread, between
    // ImGui::NewFrame and ImGui::Render — the same constraint that already
    // applies to ImGui::* calls.
    //
    // Not `final`: the editor subclasses this to add asset-database-aware
    // rendering for AssetReference fields (see editor/AssetAwareInspector).
    // Other host applications can do the same when they want to extend
    // the inspector's vocabulary.
    class Inspector : public reflection::Inspector
    {
    public:
        Inspector() = default;

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

        void readOnlyText(const char* label, const char* text) override;

        void beginGroup(const char* label) override;
        void endGroup() override;
    };
}

#endif // FLUXION_GUI_IMGUI_IMGUIINSPECTOR_HPP
