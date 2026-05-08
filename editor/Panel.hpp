#ifndef FLUXION_EDITOR_PANEL_HPP
#define FLUXION_EDITOR_PANEL_HPP

#include <atomic>
#include <string>

namespace fluxion::editor
{
    // Base for every editor ImGui panel. Owns its visibility flag (atomic so
    // a future native menu bar or scripting layer can flip it from another
    // thread) and exposes a stable display name.
    //
    // The lifetime contract is:
    //   * Panels are constructed once by the EditorApp at startup.
    //   * draw() is invoked from the ImGui draw callback on the render
    //     thread, ONLY when isVisible() returned true at the start of the
    //     frame; the implementation issues a single ImGui::Begin/End pair.
    //   * Panels keep no references to engine resources that outlive the
    //     EditorApp; they may store a back-pointer to the EditorApp for
    //     access to shared editor state (selection, current scene, etc.)
    //     introduced in later phases.
    class Panel
    {
    public:
        virtual ~Panel() = default;

        Panel(const Panel&) = delete;
        Panel& operator=(const Panel&) = delete;
        Panel(Panel&&) = delete;
        Panel& operator=(Panel&&) = delete;

        [[nodiscard]] const std::string& displayName() const noexcept { return displayName_; }

        [[nodiscard]] bool isVisible() const noexcept
        {
            return visible_.load(std::memory_order_acquire);
        }
        void setVisible(bool v) noexcept
        {
            visible_.store(v, std::memory_order_release);
        }

        // Concrete panel issues its ImGui content here. Called only when
        // visible. Panel may flip its own visibility (e.g. when a future
        // close button is wired) by calling setVisible(false).
        virtual void draw() = 0;

    protected:
        explicit Panel(std::string name, bool initialVisible = true):
            displayName_{std::move(name)}, visible_{initialVisible} {}

        // Subclasses pass this to ImGui::Begin so the window title matches
        // the menu-bar entry. Kept protected (rather than re-fetched via
        // displayName() in every draw()) to keep the hot path zero-cost.
        std::string displayName_;

    private:
        std::atomic<bool> visible_;
    };
}

#endif // FLUXION_EDITOR_PANEL_HPP
