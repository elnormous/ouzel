#ifndef FLUXION_GUI_IMGUI_IMGUICONTEXT_HPP
#define FLUXION_GUI_IMGUI_IMGUICONTEXT_HPP

#include <cstdint>
#include <functional>
#include <memory>

namespace fluxion::core
{
    class Engine;
}

namespace fluxion::graphics
{
    class Texture;
}

namespace fluxion::gui::imgui
{
    class Backend;

    class Context final
    {
    public:
        using DrawCallback = std::function<void()>;

        explicit Context(core::Engine& engine);
        ~Context();

        Context(const Context&) = delete;
        Context& operator=(const Context&) = delete;
        Context(Context&&) = delete;
        Context& operator=(Context&&) = delete;

        // The callback is invoked on the render thread, between ImGui::NewFrame
        // and ImGui::Render. All ImGui:: drawing calls must happen inside it.
        void setDrawCallback(DrawCallback cb);

        // Master switch. When disabled, the backend skips the entire ImGui
        // frame (no NewFrame, no draw calls, no Render): zero overlay
        // rendering and zero per-frame ImGui CPU/GPU work. Input is still
        // captured into the buffer (cheap), so re-enabling resumes seamlessly.
        // Safe to call from any thread.
        void setEnabled(bool enabled) noexcept;
        bool isEnabled() const noexcept;

        // Whether ImGui currently wants to consume keyboard / mouse input.
        // Useful for samples to suppress in-game input while a UI element is focused.
        bool wantsKeyboard() const noexcept;
        bool wantsMouse() const noexcept;

        // Returns a handle suitable for ImGui::Image's ImTextureID.
        // See ImGuiBackend::textureNativeHandle for the per-backend
        // shape and the "0 means not yet ready" convention.
        std::uint64_t textureNativeHandle(
            const graphics::Texture& texture) const noexcept;

    private:
        std::unique_ptr<Backend> backend;
    };
}

#endif // FLUXION_GUI_IMGUI_IMGUICONTEXT_HPP
