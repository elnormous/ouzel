#ifndef FLUXION_EDITOR_INFOPANEL_HPP
#define FLUXION_EDITOR_INFOPANEL_HPP

#include <string>
#include "../Panel.hpp"

namespace fluxion::editor
{
    // Top-of-window status panel: renderer name, display size, frame stats,
    // ImGui input-capture state, and the result of the startup self-tests
    // shipped with the editor (reflection round-trip, scene round-trip).
    // It is the editor's "everything is wired correctly" smoke test that
    // every contributor sees on first launch.
    class InfoPanel final : public Panel
    {
    public:
        InfoPanel(std::string driverName,
                  std::string reflectionSelfTest,
                  std::string sceneSelfTest);

        void draw() override;

    private:
        std::string driverName_;
        std::string reflectionSelfTest_;
        std::string sceneSelfTest_;
    };
}

#endif // FLUXION_EDITOR_INFOPANEL_HPP
