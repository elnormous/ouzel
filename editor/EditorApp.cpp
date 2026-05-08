#include "EditorApp.hpp"

#include <cstring>
#include "imgui.h"
#include "../engine/core/Engine.hpp"
#include "../engine/core/Window.hpp"
#include "../engine/graphics/Graphics.hpp"
#include "../engine/reflection/JsonInspector.hpp"
#include "../engine/reflection/TypeRegistry.hpp"
#include "../engine/scene/Actor.hpp"
#include "../engine/scene/Camera.hpp"
#include "../engine/scene/Component.hpp"
#include "../engine/scene/Layer.hpp"
#include "../engine/scene/Scene.hpp"
#include "../engine/scene/SceneManager.hpp"
#include "../engine/scene/SceneSerializer.hpp"
#include "../engine/storage/Path.hpp"
#include "SceneIO.hpp"
#include "panels/InfoPanel.hpp"
#include "panels/InspectorPanel.hpp"
#include "panels/PlayControlPanel.hpp"
#include "panels/SceneTreePanel.hpp"
#include "panels/ViewportPanel.hpp"

namespace fluxion::editor
{
    EditorApp::EditorApp(): imgui_{*engine}
    {
        engine->setScreenSaverEnabled(false);
        // Override the engine-baked FLUXION_APPLICATION_NAME ("sample") at
        // runtime; we share a single libfluxion with the samples binary.
        engine->getWindow().setTitle("Fluxion Editor");

        buildTestScene();

        // Hand the working scene to the SceneManager so the engine's
        // render loop runs. Without an active scene Scene::draw isn't
        // called, Graphics::present is never queued, and the render
        // device's preSwapHook (where ImGui draws) never fires —
        // which is what produced the all-white window.
        engine->getSceneManager().setScene(workingScene_);

        // Self-tests run once at startup. Their results are passed to
        // the InfoPanel so the user sees them on first frame; failures
        // are also logged for CI to scrape.
        const auto reflectionResult = runReflectionSelfTest();
        const auto sceneResult = runSceneRoundTripSelfTest();
        const auto playResult = runPlayControlSelfTest();
        log(Log::Level::info) << "Editor reflection self-test: " << reflectionResult;
        log(Log::Level::info) << "Editor scene round-trip self-test: " << sceneResult;
        log(Log::Level::info) << "Editor play-control self-test: " << playResult;

        const auto driverName =
            std::string{graphics::getDriverName(engine->getGraphics().getDevice()->getDriver())};

        panels_.push_back(std::make_unique<InfoPanel>(driverName,
                                                       reflectionResult + " | play=" + playResult,
                                                       sceneResult));

        // Viewport panel owns its render target + camera. Built first
        // so the SceneTreePanel can be told which camera-actor to hide
        // from the hierarchy. Stored as a non-owning raw pointer for
        // later access — lifetime is tied to panels_.
        // The panel takes refs to the working scene + selection so it
        // can run picking and update selection from inside its own
        // ImGui::Image rect; editorLayer_ is passed as the exclude
        // filter so the back-buffer-clear camera never appears.
        auto viewportPanel = std::make_unique<ViewportPanel>(
            *engine, imgui_, workingLayer_,
            workingScene_, selection_, &editorLayer_);
        viewportPanel_ = viewportPanel.get();
        panels_.push_back(std::move(viewportPanel));

        panels_.push_back(std::make_unique<SceneTreePanel>(&workingScene_, &selection_,
                                                           &commandStack_,
                                                           &editorLayer_,
                                                           viewportPanel_->getCameraActor()));
        panels_.push_back(std::make_unique<InspectorPanel>(&selection_));
        panels_.push_back(std::make_unique<PlayControlPanel>(&workingScene_));

        imgui_.setDrawCallback([this]() {
            drawMainMenu();
            // Top-level keyboard shortcuts: Ctrl+Z / Ctrl+Y / Ctrl+
            // Shift+Z drive undo/redo regardless of which panel has
            // focus, so the user can hit them after any mutation.
            const ImGuiIO& io = ImGui::GetIO();
            const bool ctrl = io.KeyCtrl;
            const bool shift = io.KeyShift;
            if (ctrl && !shift && ImGui::IsKeyPressed(ImGuiKey_Z, false))
                commandStack_.undo();
            if ((ctrl && shift && ImGui::IsKeyPressed(ImGuiKey_Z, false)) ||
                (ctrl && !shift && ImGui::IsKeyPressed(ImGuiKey_Y, false)))
                commandStack_.redo();

            for (const auto& p : panels_)
                if (p && p->isVisible())
                    p->draw();
            drawFileDialog();
        });
        // Picking + AABB overlay live entirely inside ViewportPanel
        // now: clicks come from ImGui::IsItemClicked on the image,
        // the AABB outlines render on the panel's window draw list.
        // The back-buffer area outside any panel is just cleared by
        // editorCamera_ — no editor decoration there.
    }

    void EditorApp::drawEditMenu()
    {
        if (!ImGui::BeginMenu("Edit")) return;
        const auto undoLabel = commandStack_.undoLabel();
        const auto redoLabel = commandStack_.redoLabel();
        char buf[160];
        std::snprintf(buf, sizeof(buf), "Undo%s%s",
                      undoLabel.empty() ? "" : " ",
                      undoLabel.c_str());
        if (ImGui::MenuItem(buf, "Ctrl+Z",
                            /*selected*/ false,
                            /*enabled*/ commandStack_.canUndo()))
            commandStack_.undo();
        std::snprintf(buf, sizeof(buf), "Redo%s%s",
                      redoLabel.empty() ? "" : " ",
                      redoLabel.c_str());
        if (ImGui::MenuItem(buf, "Ctrl+Y",
                            /*selected*/ false,
                            /*enabled*/ commandStack_.canRedo()))
            commandStack_.redo();
        ImGui::EndMenu();
    }

    void EditorApp::drawMainMenu()
    {
        if (!ImGui::BeginMainMenuBar()) return;

        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Scene"))
                doNewScene();

            if (ImGui::MenuItem("Open Scene..."))
            {
                fileDialogMode_ = FileDialogMode::Open;
                std::memset(fileDialogBuf_, 0, sizeof(fileDialogBuf_));
                if (!currentScenePath_.empty())
                    std::snprintf(fileDialogBuf_, sizeof(fileDialogBuf_),
                                  "%s", currentScenePath_.c_str());
            }

            // "Save" only enabled once we know where the scene came
            // from — otherwise we route through Save As to ask the
            // user where to write it.
            const bool haveSavePath = !currentScenePath_.empty();
            if (ImGui::MenuItem("Save Scene", "Ctrl+S",
                                /*selected*/ false,
                                /*enabled*/ haveSavePath))
                doSaveScene();

            if (ImGui::MenuItem("Save Scene As..."))
            {
                fileDialogMode_ = FileDialogMode::Save;
                std::memset(fileDialogBuf_, 0, sizeof(fileDialogBuf_));
                if (!currentScenePath_.empty())
                    std::snprintf(fileDialogBuf_, sizeof(fileDialogBuf_),
                                  "%s", currentScenePath_.c_str());
            }
            ImGui::EndMenu();
        }

        drawEditMenu();

        // Right-side info: current scene path + a transient status
        // message after save/load. Helps the user see at a glance
        // which file they're editing without having to dig into the
        // file dialog.
        ImGui::SameLine(ImGui::GetWindowWidth() - 480.0F);
        if (!currentScenePath_.empty())
            ImGui::TextDisabled("[%s]", currentScenePath_.c_str());
        else
            ImGui::TextDisabled("[unsaved]");
        if (statusFramesRemaining_ > 0)
        {
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0.6F, 0.9F, 0.6F, 1.0F),
                               "%s", statusMessage_.c_str());
            --statusFramesRemaining_;
        }

        ImGui::EndMainMenuBar();
    }

    void EditorApp::drawFileDialog()
    {
        if (fileDialogMode_ == FileDialogMode::None) return;

        const char* popupId = fileDialogMode_ == FileDialogMode::Save
            ? "Save Scene As##editor_file_dialog"
            : "Open Scene##editor_file_dialog";

        // Open the popup the very first frame we entered Save / Open
        // mode. IsPopupOpen filters subsequent frames so we don't
        // re-open every tick (which would steal focus from inputs).
        if (!ImGui::IsPopupOpen(popupId))
            ImGui::OpenPopup(popupId);

        if (ImGui::BeginPopupModal(popupId, nullptr,
                                   ImGuiWindowFlags_AlwaysAutoResize))
        {
            const bool isSave = fileDialogMode_ == FileDialogMode::Save;
            ImGui::Text(isSave ? "Path to save the scene to:"
                               : "Path to load the scene from:");

            ImGui::SetNextItemWidth(380.0F);
            const bool entered = ImGui::InputText(
                "##editor_path",
                fileDialogBuf_, sizeof(fileDialogBuf_),
                ImGuiInputTextFlags_EnterReturnsTrue);

            const bool confirm = entered ||
                ImGui::Button(isSave ? "Save" : "Open");
            ImGui::SameLine();
            const bool cancel = ImGui::Button("Cancel");

            if (confirm && fileDialogBuf_[0] != '\0')
            {
                currentScenePath_ = fileDialogBuf_;
                if (isSave) doSaveScene();
                else        doOpenScene();
                fileDialogMode_ = FileDialogMode::None;
                ImGui::CloseCurrentPopup();
            }
            else if (cancel)
            {
                fileDialogMode_ = FileDialogMode::None;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    void EditorApp::doNewScene()
    {
        // Drop user-layer content; selection clears via validateAgainst
        // on the next SceneTreePanel draw. The editor layer (camera /
        // future gizmos) is intentionally untouched — those are
        // editor infrastructure, not part of the user's scene.
        workingLayer_.removeAllChildren();
        selection_.clear();
        // Undo history reflected the previous scene's mutations,
        // which no longer match — drop it.
        commandStack_.clear();
        currentScenePath_.clear();
        statusMessage_ = "New scene";
        statusFramesRemaining_ = 120;
    }

    void EditorApp::doSaveScene()
    {
        if (currentScenePath_.empty()) return;
        try
        {
            sceneIO::saveUserScene(workingLayer_,
                                   storage::Path{currentScenePath_});
            statusMessage_ = "Saved";
            log(Log::Level::info)
                << "Editor: saved scene to " << currentScenePath_;
        }
        catch (const std::exception& e)
        {
            statusMessage_ = std::string{"Save failed: "} + e.what();
            log(Log::Level::error)
                << "Editor: save failed: " << e.what();
        }
        statusFramesRemaining_ = 180;
    }

    void EditorApp::doOpenScene()
    {
        if (currentScenePath_.empty()) return;
        try
        {
            // Drop the current selection BEFORE the load runs — the
            // selected actor pointer (if any) becomes invalid after
            // layerFromJson clears the layer.
            selection_.clear();
            // The command stack was capturing pre-load state (uuids
            // and pointers that won't survive the wholesale layer
            // replacement); drop it together with the old scene.
            commandStack_.clear();
            sceneIO::loadUserScene(workingLayer_,
                                   storage::Path{currentScenePath_});
            statusMessage_ = "Loaded";
            log(Log::Level::info)
                << "Editor: loaded scene from " << currentScenePath_;
        }
        catch (const std::exception& e)
        {
            statusMessage_ = std::string{"Load failed: "} + e.what();
            log(Log::Level::error)
                << "Editor: load failed: " << e.what();
        }
        statusFramesRemaining_ = 180;
    }

    void EditorApp::buildTestScene()
    {
        // Tiny placeholder scene for the PlayControlPanel to manage:
        // two actors with deterministic names + positions. Phase 5 will
        // replace this with a real loaded-from-disk scene; for now it
        // gives the Edit/Play/Stop round-trip something concrete to act
        // on so the demo is observable.
        workingScene_.addLayer(workingLayer_);

        auto a1 = std::make_unique<scene::Actor>();
        a1->setName("Player");
        a1->setPosition(math::Vector<float, 3>{0.0F, 0.0F, 0.0F});
        workingLayer_.addChild(std::move(a1));

        auto a2 = std::make_unique<scene::Actor>();
        a2->setName("Enemy");
        a2->setPosition(math::Vector<float, 3>{10.0F, 0.0F, 0.0F});
        workingLayer_.addChild(std::move(a2));

        // Editor camera lives on a separate layer that the editor's
        // panels and picking explicitly filter out. The engine still
        // traverses both layers for rendering (so the camera clears the
        // back buffer and the user content draws on top), but the
        // SceneTreePanel never lists this layer and the AABB overlay
        // never decorates its actors.
        //
        // The orthographic content-size + showAll scale mode give the
        // editor a stable [-30..+30] x [-30..+30] world-space view
        // regardless of window aspect, so the picking AABB overlay
        // (drawn against this camera's projection) lines up with what
        // the user sees on screen and the test actors at world (0,0,0)
        // and (10,0,0) project into the visible region.
        editorCamera_.setClearColorBuffer(true);
        editorCamera_.setClearColor(math::Color{32, 32, 40, 255});
        editorCamera_.setScaleMode(scene::Camera::ScaleMode::showAll);
        editorCamera_.setTargetContentSize(math::Size<float, 2>{60.0F, 60.0F});
        editorCameraActor_.setName("Editor Camera");
        editorCameraActor_.addComponent(editorCamera_);
        editorLayer_.addChild(editorCameraActor_);
        workingScene_.addLayer(editorLayer_);
    }

    EditorApp::~EditorApp() = default;

    void EditorApp::registerPanels()
    {
        // Phase 5+ panel registrations will land here:
        //   panels_.push_back(std::make_unique<SceneTreePanel>(...));
        //   panels_.push_back(std::make_unique<InspectorPanel>(...));
        //   panels_.push_back(std::make_unique<AssetBrowserPanel>(...));
        //   panels_.push_back(std::make_unique<LogPanel>(...));
    }

    std::string EditorApp::runReflectionSelfTest()
    {
        auto cam = reflection::TypeRegistry::instance().create("scene.Camera");
        if (!cam) return "FAIL: scene.Camera not registered";
        auto* camera = static_cast<scene::Camera*>(cam.get());

        constexpr float testFov = 0.7F;
        constexpr float testNear = 12.5F;
        constexpr float testFar = 250.0F;
        camera->setFov(testFov);
        camera->setNearPlane(testNear);
        camera->setFarPlane(testFar);
        camera->setClearColor(math::Color{255, 64, 128, 200});

        json::Value out;
        reflection::serializeComponent(*camera, out);

        auto cam2 = reflection::TypeRegistry::instance().create("scene.Camera");
        auto* camera2 = static_cast<scene::Camera*>(cam2.get());
        reflection::deserializeComponent(*camera2, out);

        const bool ok = (camera2->getFov() == testFov)
                     && (camera2->getNearPlane() == testNear)
                     && (camera2->getFarPlane() == testFar)
                     && (camera2->getClearColor().v == camera->getClearColor().v);
        return ok ? "PASS (scene.Camera)" : "FAIL (round-trip mismatch)";
    }

    std::string EditorApp::runSceneRoundTripSelfTest()
    {
        scene::Scene src;
        scene::Layer srcLayer;
        src.addLayer(srcLayer);

        auto a1 = std::make_unique<scene::Actor>();
        a1->setName("Hero");
        a1->setPosition(math::Vector<float, 3>{1.0F, 2.0F, 3.0F});
        const auto heroId = a1->getId();
        srcLayer.addChild(std::move(a1));

        auto a2 = std::make_unique<scene::Actor>();
        a2->setName("Sidekick");
        a2->setOpacity(0.5F);
        auto child = std::make_unique<scene::Actor>();
        child->setName("Pet");
        const auto petId = child->getId();
        a2->addChild(std::move(child));
        srcLayer.addChild(std::move(a2));

        const auto json = scene::serialization::sceneToJson(src);

        scene::Scene dst;
        scene::Layer dstLayer;
        dst.addLayer(dstLayer);
        try { scene::serialization::sceneFromJson(dst, json); }
        catch (const std::exception& e) { return std::string{"FAIL parse: "} + e.what(); }

        const auto& dstChildren = dstLayer.getChildren();
        if (dstChildren.size() != 2) return "FAIL actor count != 2";
        if (dstChildren[0]->getName() != "Hero") return "FAIL name(0)";
        if (dstChildren[0]->getId() != heroId) return "FAIL id(Hero)";
        if (dstChildren[1]->getName() != "Sidekick") return "FAIL name(1)";
        if (dstChildren[1]->getChildren().size() != 1) return "FAIL pet missing";
        if (dstChildren[1]->getChildren()[0]->getId() != petId) return "FAIL id(Pet)";
        if (dstChildren[0]->getPosition().v[0] != 1.0F) return "FAIL position";
        if (dstChildren[1]->getOpacity() != 0.5F) return "FAIL opacity";
        return "PASS (2 actors + 1 nested, ids preserved)";
    }

    std::string EditorApp::runPlayControlSelfTest()
    {
        // Verifies the snapshot/restore semantics that Play/Stop relies
        // on, without involving the engine update loop. Walks the same
        // code path the PlayControlPanel does:
        //   1. snapshot the current workingScene_
        //   2. mutate an actor's position
        //   3. restore from the snapshot
        //   4. assert the position is back to the original
        const auto& kids = workingLayer_.getChildren();
        if (kids.size() < 2) return "FAIL: test scene not built";
        const std::size_t originalCount = kids.size();
        const auto originalPos = kids[0]->getPosition();

        json::Value snapshot;
        try { snapshot = scene::serialization::sceneToJson(workingScene_); }
        catch (const std::exception& e) { return std::string{"FAIL snapshot: "} + e.what(); }

        kids[0]->setPosition(math::Vector<float, 3>{999.0F, -42.0F, 17.0F});

        try { scene::serialization::sceneFromJson(workingScene_, snapshot); }
        catch (const std::exception& e) { return std::string{"FAIL restore: "} + e.what(); }

        // After the phase 4 fix: layerFromJson clears existing children
        // before populating, so the actor count returns to its pre-
        // snapshot value (no doubling). The restored Player at index 0
        // carries the original position.
        const auto& after = workingLayer_.getChildren();
        if (after.size() != originalCount)
            return "FAIL: actor count drifted across restore";
        const auto& restoredPos = after[0]->getPosition();
        if (restoredPos.v[0] != originalPos.v[0] ||
            restoredPos.v[1] != originalPos.v[1] ||
            restoredPos.v[2] != originalPos.v[2])
            return "FAIL: restored position does not match original";

        return "PASS (snapshot/restore round-trip)";
    }
}
