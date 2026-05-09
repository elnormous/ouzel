#include "../../core/Setup.h"

#include "ImGuiBackend.hpp"
#include "../../core/Engine.hpp"
#include "../../graphics/Graphics.hpp"
#include "../../graphics/RenderDevice.hpp"

namespace fluxion::gui::imgui
{
#if FLUXION_COMPILE_DIRECT3D11
    std::unique_ptr<Backend> createBackendD3D11(core::Engine& eng);
#endif
#if FLUXION_COMPILE_OPENGL
    std::unique_ptr<Backend> createBackendOGL(core::Engine& eng);
#endif
#if FLUXION_COMPILE_METAL
    std::unique_ptr<Backend> createBackendMetal(core::Engine& eng);
#endif

    std::unique_ptr<Backend> createBackend(core::Engine& eng)
    {
        auto* device = eng.getGraphics().getDevice();
        if (!device) return {};

        switch (device->getDriver())
        {
#if FLUXION_COMPILE_DIRECT3D11
            case graphics::Driver::direct3D11: return createBackendD3D11(eng);
#endif
#if FLUXION_COMPILE_OPENGL
            case graphics::Driver::openGl: return createBackendOGL(eng);
#endif
#if FLUXION_COMPILE_METAL
            case graphics::Driver::metal: return createBackendMetal(eng);
#endif
            default: return {};
        }
    }
}
