#include "ImGuiInspector.hpp"
#include "imgui.h"

namespace fluxion::gui::imgui
{
    void Inspector::floatField(const char* label, float& value)
    {
        ImGui::DragFloat(label, &value, 0.01F);
    }

    void Inspector::floatRangeField(const char* label, float& value,
                                    float minValue, float maxValue)
    {
        ImGui::SliderFloat(label, &value, minValue, maxValue);
    }

    void Inspector::intField(const char* label, int& value)
    {
        ImGui::DragInt(label, &value);
    }

    void Inspector::intRangeField(const char* label, int& value,
                                  int minValue, int maxValue)
    {
        ImGui::SliderInt(label, &value, minValue, maxValue);
    }

    void Inspector::boolField(const char* label, bool& value)
    {
        ImGui::Checkbox(label, &value);
    }

    void Inspector::stringField(const char* label, std::string& value)
    {
        // ImGui::InputText needs a writable char buffer. Resize on demand
        // so the user can grow the string while editing; we keep at least
        // 256 bytes to avoid frequent reallocations on common edits.
        constexpr std::size_t minCapacity = 256;
        if (value.capacity() < minCapacity) value.reserve(minCapacity);

        struct Callback
        {
            static int resize(ImGuiInputTextCallbackData* data)
            {
                if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
                {
                    auto* str = static_cast<std::string*>(data->UserData);
                    str->resize(static_cast<std::size_t>(data->BufTextLen));
                    data->Buf = str->data();
                }
                return 0;
            }
        };

        ImGui::InputText(label, value.data(), value.capacity() + 1,
                         ImGuiInputTextFlags_CallbackResize, Callback::resize, &value);
    }

    void Inspector::colorField(const char* label, math::Color& value)
    {
        float rgba[4]{
            value.v[0] / 255.0F,
            value.v[1] / 255.0F,
            value.v[2] / 255.0F,
            value.v[3] / 255.0F
        };
        if (ImGui::ColorEdit4(label, rgba))
        {
            value.v[0] = static_cast<std::uint8_t>(rgba[0] * 255.0F);
            value.v[1] = static_cast<std::uint8_t>(rgba[1] * 255.0F);
            value.v[2] = static_cast<std::uint8_t>(rgba[2] * 255.0F);
            value.v[3] = static_cast<std::uint8_t>(rgba[3] * 255.0F);
        }
    }

    void Inspector::vector2Field(const char* label, math::Vector<float, 2>& value)
    {
        ImGui::DragFloat2(label, value.v, 0.1F);
    }

    void Inspector::vector3Field(const char* label, math::Vector<float, 3>& value)
    {
        ImGui::DragFloat3(label, value.v, 0.1F);
    }

    void Inspector::vector4Field(const char* label, math::Vector<float, 4>& value)
    {
        ImGui::DragFloat4(label, value.v, 0.1F);
    }

    void Inspector::enumField(const char* label, int& value,
                              const char* const* options, std::size_t count)
    {
        const int n = static_cast<int>(count);
        const char* current = (value >= 0 && value < n) ? options[value] : "<invalid>";
        if (ImGui::BeginCombo(label, current))
        {
            for (int i = 0; i < n; ++i)
            {
                const bool selected = (i == value);
                if (ImGui::Selectable(options[i], selected)) value = i;
                if (selected) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }

    void Inspector::readOnlyText(const char* label, const char* text)
    {
        ImGui::LabelText(label, "%s", text);
    }

    void Inspector::beginGroup(const char* label)
    {
        ImGui::SeparatorText(label);
        ImGui::Indent();
    }

    void Inspector::endGroup()
    {
        ImGui::Unindent();
    }
}
