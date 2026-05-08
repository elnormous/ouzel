#include "ReflectionPanel.hpp"

#include <cstdio>
#include "imgui.h"
#include "../../engine/gui/imgui/ImGuiInspector.hpp"
#include "../../engine/reflection/JsonInspector.hpp"
#include "../../engine/reflection/TypeRegistry.hpp"
#include "../../engine/scene/Actor.hpp"
#include "../../engine/scene/Layer.hpp"
#include "../../engine/scene/Scene.hpp"
#include "../../engine/scene/SceneSerializer.hpp"
#include "../../engine/storage/Path.hpp"
#include "../../engine/utils/Uuid.hpp"

namespace fluxion::editor
{
    namespace
    {
        // ImGui::InputTextMultiline expects a writable char buffer; this
        // resize callback grows the std::string in place when the user
        // types past the current capacity.
        int resizeStringBuffer(ImGuiInputTextCallbackData* data)
        {
            if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
            {
                auto* s = static_cast<std::string*>(data->UserData);
                s->resize(static_cast<std::size_t>(data->BufTextLen));
                data->Buf = s->data();
            }
            return 0;
        }
    }

    ReflectionPanel::ReflectionPanel(): Panel{"Reflection + serialization", true} {}
    ReflectionPanel::~ReflectionPanel() = default;

    void ReflectionPanel::draw()
    {
        ImGui::SetNextWindowPos(ImVec2(20, 240), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(560, 520), ImGuiCond_FirstUseEver);
        ImGui::Begin(displayName_.c_str());

        const auto types = reflection::TypeRegistry::instance().listTypes();
        ImGui::Text("TypeRegistry has %zu component type(s):", types.size());
        ImGui::Indent();
        for (const auto& t : types) ImGui::BulletText("%s", t.c_str());
        ImGui::Unindent();
        ImGui::Separator();

        if (selectedType_.empty() && !types.empty()) selectedType_ = types.front();
        if (ImGui::BeginCombo("##type", selectedType_.c_str()))
        {
            for (const auto& t : types)
            {
                const bool sel = (t == selectedType_);
                if (ImGui::Selectable(t.c_str(), sel)) selectedType_ = t;
                if (sel) ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (instantiatedType_ != selectedType_)
        {
            instance_ = reflection::TypeRegistry::instance().create(selectedType_);
            instantiatedType_ = selectedType_;
        }

        if (!instance_)
        {
            ImGui::TextColored(ImVec4(1, 0.4F, 0.4F, 1),
                               "TypeRegistry::create returned null");
            ImGui::End();
            return;
        }

        ImGui::TextDisabled("Editing a temporary instance of %s", selectedType_.c_str());
        ImGui::Separator();
        gui::imgui::Inspector inspector;
        instance_->inspect(inspector);

        ImGui::Separator();
        ImGui::TextDisabled("Serialization round-trip");

        static Uuid demoId;
        if (ImGui::Button("Generate UUID")) demoId = Uuid::generate();
        ImGui::SameLine();
        ImGui::Text("uuid: %s", demoId.toString().c_str());

        if (ImGui::Button("Serialize"))
        {
            json::Value out;
            reflection::serializeComponent(*instance_, out);
            jsonBuffer_ = json::encode(out, /*whiteSpaces*/ true);
            jsonStatus_ = "Serialized.";
        }
        ImGui::SameLine();
        if (ImGui::Button("Load"))
        {
            try
            {
                const auto parsed = json::parse(jsonBuffer_);
                reflection::deserializeComponent(*instance_, parsed);
                jsonStatus_ = "Loaded.";
            }
            catch (const std::exception& e)
            {
                jsonStatus_ = std::string{"Parse error: "} + e.what();
            }
        }
        ImGui::SameLine();
        ImGui::TextDisabled("%s", jsonStatus_.c_str());

        if (jsonBuffer_.capacity() < 1024) jsonBuffer_.reserve(1024);
        ImGui::InputTextMultiline("##json",
                                  jsonBuffer_.data(), jsonBuffer_.capacity() + 1,
                                  ImVec2(-1, 140),
                                  ImGuiInputTextFlags_CallbackResize,
                                  resizeStringBuffer, &jsonBuffer_);

        // Disk-level demo: exercises storage::AtomicFile cross-platform.
        ImGui::Separator();
        ImGui::TextDisabled("Scene save / load (atomic file write)");
        static const storage::Path scenePath{"demo_scene.flux"};
        if (ImGui::Button("Save test scene"))
        {
            try
            {
                scene::Scene s;
                scene::Layer layer;
                s.addLayer(layer);
                auto a = std::make_unique<scene::Actor>();
                a->setName("DemoActor");
                a->setPosition(math::Vector<float, 3>{42.0F, -7.5F, 0.0F});
                layer.addChild(std::move(a));
                scene::serialization::saveToFile(s, scenePath);
                sceneFileStatus_ = std::string{"Saved to "} + std::string{scenePath};
            }
            catch (const std::exception& e)
            {
                sceneFileStatus_ = std::string{"Save failed: "} + e.what();
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Load test scene"))
        {
            try
            {
                scene::Scene s;
                scene::Layer layer;
                s.addLayer(layer);
                scene::serialization::loadFromFile(s, scenePath);
                const auto& kids = layer.getChildren();
                if (kids.empty())
                    sceneFileStatus_ = "Loaded but layer is empty";
                else
                {
                    const auto& p = kids.front()->getPosition();
                    char buf[160];
                    std::snprintf(buf, sizeof(buf),
                                  "Loaded %zu actor(s); first = '%s' at (%.1f, %.1f, %.1f)",
                                  kids.size(),
                                  kids.front()->getName().c_str(),
                                  p.v[0], p.v[1], p.v[2]);
                    sceneFileStatus_ = buf;
                }
            }
            catch (const std::exception& e)
            {
                sceneFileStatus_ = std::string{"Load failed: "} + e.what();
            }
        }
        ImGui::SameLine();
        ImGui::TextDisabled("%s", sceneFileStatus_.c_str());

        ImGui::End();
    }
}
