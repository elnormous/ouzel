#include "ImGuiContext.hpp"
#include "ImGuiBackend.hpp"
#include "../../utils/Log.hpp"

namespace fluxion::gui::imgui
{
    Context::Context(core::Engine& engine)
    {
        backend = createBackend(engine);
        if (!backend)
            log(Log::Level::warning) << "ImGui: no compatible backend for the active graphics driver; ImGui will be inactive";
    }

    Context::~Context() = default;

    void Context::setDrawCallback(DrawCallback cb)
    {
        if (backend) backend->setDrawCallback(std::move(cb));
    }

    void Context::setEnabled(bool enabled) noexcept
    {
        if (backend) backend->setEnabled(enabled);
    }

    bool Context::isEnabled() const noexcept
    {
        return backend ? backend->isEnabled() : false;
    }

    bool Context::wantsKeyboard() const noexcept
    {
        return backend ? backend->wantsKeyboard() : false;
    }

    bool Context::wantsMouse() const noexcept
    {
        return backend ? backend->wantsMouse() : false;
    }
}
