#ifndef FLUXION_GUI_IMGUI_IMGUIBACKEND_HPP
#define FLUXION_GUI_IMGUI_IMGUIBACKEND_HPP

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
    // Internal interface implemented per-renderer. Each Backend owns its own
    // ImGuiContext (Dear ImGui's), installs the platform/renderer ImGui_Impl_*
    // hooks, and provides the per-frame NewFrame/Render bracket invoked from
    // the engine RenderDevice's preSwapHook on the render thread.
    class Backend
    {
    public:
        using DrawCallback = std::function<void()>;

        virtual ~Backend() = default;

        virtual void setDrawCallback(DrawCallback cb) = 0;

        virtual void setEnabled(bool enabled) noexcept = 0;
        virtual bool isEnabled() const noexcept = 0;

        virtual bool wantsKeyboard() const noexcept = 0;
        virtual bool wantsMouse() const noexcept = 0;

        // Returns the renderer-native handle for `texture` packed into a
        // 64-bit value, suitable for passing to ImGui::Image as
        // ImTextureID. The exact contents differ per backend:
        //   * Direct3D 11 — ID3D11ShaderResourceView* (cast from pointer)
        //   * OpenGL      — GLuint texture name (zero-extended)
        //   * Metal       — id<MTLTexture> (__bridge cast from pointer)
        // Returns 0 when the GPU resource has not been created yet
        // (e.g., the engine has not rendered a frame), or when the
        // texture lacks the bindShader flag. Callers must skip
        // ImGui::Image when the handle is 0 to avoid undefined ImGui
        // behaviour.
        virtual std::uint64_t textureNativeHandle(
            const graphics::Texture& texture) const noexcept = 0;
    };

    // Factory: picks the appropriate backend based on the engine's active
    // graphics driver. Returns null if no compatible backend was compiled in.
    std::unique_ptr<Backend> createBackend(core::Engine& engine);
}

#endif // FLUXION_GUI_IMGUI_IMGUIBACKEND_HPP
