#ifndef FLUXION_EDITOR_REFLECTIONPANEL_HPP
#define FLUXION_EDITOR_REFLECTIONPANEL_HPP

#include <memory>
#include <string>
#include "../Panel.hpp"
#include "../../engine/scene/Component.hpp"

namespace fluxion::editor
{
    // Live demo of the reflection + serialization pipeline:
    //   * lists every component type registered in TypeRegistry
    //   * lets the user pick one and edit a temporary instance through
    //     ImGuiInspector
    //   * round-trips that instance through JsonWriter / JsonReader, with
    //     an editable JSON text area for visual confirmation
    //   * exercises storage::AtomicFile with a "Save / Load test scene"
    //     pair targeting "demo_scene.flux" next to the editor binary
    //
    // Once phase 5 lands the live scene tree + selection, the temporary
    // instance here will be replaced by "the currently selected
    // component on the currently selected actor" — same Inspector code,
    // different source object.
    class ReflectionPanel final : public Panel
    {
    public:
        ReflectionPanel();
        ~ReflectionPanel() override;

        void draw() override;

    private:
        std::string selectedType_;
        std::string instantiatedType_;
        std::unique_ptr<scene::Component> instance_;

        std::string jsonBuffer_{"(click 'Serialize' to populate)"};
        std::string jsonStatus_;

        std::string sceneFileStatus_{"(idle)"};
    };
}

#endif // FLUXION_EDITOR_REFLECTIONPANEL_HPP
