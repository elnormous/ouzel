#include "PlayControlPanel.hpp"

#include "imgui.h"
#include "../../engine/core/Engine.hpp"
#include "../../engine/core/Window.hpp"
#include "../../engine/scene/Scene.hpp"
#include "../../engine/scene/SceneSerializer.hpp"

namespace fluxion::editor
{
    PlayControlPanel::PlayControlPanel(scene::Scene* targetScene):
        Panel{"Play control", true},
        scene_{targetScene}
    {
        // Editor entry state: simulation paused so the user can manipulate
        // the scene through the inspector without time advancing. The
        // Engine global is guaranteed live by the time any panel is
        // constructed (EditorApp's ImGui Context ctor depends on it).
        engine->setTimeScale(0.0F);
        updateWindowTitle();
    }

    void PlayControlPanel::updateWindowTitle()
    {
        // Title prefix tells the user at a glance which mode they're
        // in even when the Time panel is hidden — handy when the
        // editor window is alt-tabbed in or screenshotted. Format
        // matches what Unity does for parity.
        const char* prefix = "";
        switch (state_)
        {
            case State::Editing: prefix = "";          break;
            case State::Playing: prefix = "[PLAYING] "; break;
            case State::Paused:  prefix = "[PAUSED] ";  break;
        }
        engine->getWindow().setTitle(std::string{prefix} + "Fluxion Editor");
    }

    void PlayControlPanel::draw()
    {
        ImGui::SetNextWindowPos(ImVec2(20, 460), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(420, 200), ImGuiCond_FirstUseEver);
        ImGui::Begin(displayName_.c_str());

        const char* stateLabel = "Editing";
        ImVec4 stateColor{0.6F, 0.8F, 1.0F, 1.0F};
        switch (state_)
        {
            case State::Editing: stateLabel = "Editing";
                                 stateColor = ImVec4(0.6F, 0.8F, 1.0F, 1.0F); break;
            case State::Playing: stateLabel = "Playing";
                                 stateColor = ImVec4(0.4F, 1.0F, 0.4F, 1.0F); break;
            case State::Paused:  stateLabel = "Paused";
                                 stateColor = ImVec4(1.0F, 0.8F, 0.4F, 1.0F); break;
        }
        ImGui::TextColored(stateColor, "State: %s", stateLabel);
        ImGui::Text("timeScale = %.3f   pendingSteps = %d   snapshot=%s",
                    engine->getTimeScale(), engine->getPendingSteps(),
                    hasSnapshot_ ? "yes" : "no");

        ImGui::Separator();

        // Button enabling depends on state. ImGui's BeginDisabled keeps
        // the buttons visible (so the layout doesn't jump) but greys
        // them out until the transition is meaningful.
        ImGui::BeginDisabled(state_ == State::Playing || state_ == State::Paused);
        if (ImGui::Button("Play")) enterPlay();
        ImGui::EndDisabled();
        ImGui::SameLine();

        ImGui::BeginDisabled(state_ != State::Playing);
        if (ImGui::Button("Pause")) enterPause();
        ImGui::EndDisabled();
        ImGui::SameLine();

        ImGui::BeginDisabled(state_ != State::Paused);
        if (ImGui::Button("Resume")) enterResume();
        ImGui::EndDisabled();
        ImGui::SameLine();

        ImGui::BeginDisabled(state_ != State::Paused);
        if (ImGui::Button("Step")) stepOne();
        ImGui::EndDisabled();
        ImGui::SameLine();

        ImGui::BeginDisabled(state_ == State::Editing);
        if (ImGui::Button("Stop")) enterStop();
        ImGui::EndDisabled();

        ImGui::Separator();

        // Time scale slider applies live during Play. Disabled in Editing
        // (where timeScale is fixed at 0) and Paused (the slider would be
        // misleading since the actual scale is 0 until Resume).
        ImGui::BeginDisabled(state_ != State::Playing);
        if (ImGui::SliderFloat("Time scale", &playbackSpeed_, 0.0F, 4.0F, "%.2fx"))
            engine->setTimeScale(playbackSpeed_);
        ImGui::EndDisabled();

        if (!statusMessage_.empty())
        {
            ImGui::Separator();
            ImGui::TextWrapped("%s", statusMessage_.c_str());
        }

        ImGui::End();
    }

    void PlayControlPanel::enterPlay()
    {
        if (state_ != State::Editing) return;       // no-op from Playing/Paused
        if (!scene_)
        {
            statusMessage_ = "No scene attached; nothing to play.";
            return;
        }

        try
        {
            snapshot_ = scene::serialization::sceneToJson(*scene_);
            hasSnapshot_ = true;
        }
        catch (const std::exception& e)
        {
            statusMessage_ = std::string{"Snapshot failed: "} + e.what();
            hasSnapshot_ = false;
            return;
        }

        state_ = State::Playing;
        engine->setTimeScale(playbackSpeed_);
        statusMessage_ = "Play started; scene snapshotted.";
        updateWindowTitle();
    }

    void PlayControlPanel::enterPause()
    {
        if (state_ != State::Playing) return;       // no-op from Editing/Paused
        state_ = State::Paused;
        engine->setTimeScale(0.0F);
        statusMessage_ = "Paused. Use Step to advance one frame, Resume to continue.";
        updateWindowTitle();
    }

    void PlayControlPanel::enterResume()
    {
        if (state_ != State::Paused) return;        // no-op from Editing/Playing
        state_ = State::Playing;
        engine->setTimeScale(playbackSpeed_);
        statusMessage_ = "Resumed.";
        updateWindowTitle();
    }

    void PlayControlPanel::stepOne()
    {
        if (state_ != State::Paused) return;        // step only in Paused
        // requestStep() bypasses timeScale for exactly one update tick,
        // delivering real-time delta. The state stays Paused so the
        // simulation freezes again on the next tick.
        engine->requestStep(1);
        statusMessage_ = "Step requested (one update tick at real-time delta).";
    }

    void PlayControlPanel::enterStop()
    {
        if (state_ == State::Editing) return;       // already stopped
        if (hasSnapshot_ && scene_)
        {
            try
            {
                scene::serialization::sceneFromJson(*scene_, snapshot_);
                statusMessage_ = "Stop: snapshot restored.";
            }
            catch (const std::exception& e)
            {
                statusMessage_ = std::string{"Restore failed: "} + e.what();
            }
        }
        else
        {
            statusMessage_ = "Stop: no snapshot to restore.";
        }
        state_ = State::Editing;
        engine->setTimeScale(0.0F);
        updateWindowTitle();
    }
}
