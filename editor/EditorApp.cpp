#include "EditorApp.hpp"

#include <array>
#include <cstring>
#include <filesystem>
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
#include "../engine/scene/Light.hpp"
#include "../engine/scene/ParticleSystem.hpp"
#include "../engine/scene/Scene.hpp"
#include "../engine/scene/SceneManager.hpp"
#include "../engine/scene/SceneSerializer.hpp"
#include "../engine/scene/ShapeRenderer.hpp"
#include "../engine/scene/SkinnedMeshRenderer.hpp"
#include "../engine/scene/SpriteRenderer.hpp"
#include "../engine/scene/StaticMeshRenderer.hpp"
#include "../engine/scene/TextRenderer.hpp"
#include "../engine/assets/Asset.hpp"
#include "../engine/storage/Path.hpp"
#include "asset/AssetImportPipeline.hpp"
#include "SceneIO.hpp"
#include "panels/AssetBrowserPanel.hpp"
#include "panels/InfoPanel.hpp"
#include "panels/InspectorPanel.hpp"
#include "panels/PlayControlPanel.hpp"
#include "panels/SceneTreePanel.hpp"
#include "panels/SpriteAnimationPanel.hpp"
#include "panels/SpriteEditorPanel.hpp"
#include "panels/ViewportPanel.hpp"

// Forward declarations of the per-component ForceLink symbols
// emitted by FLUXION_COMPONENT_REGISTER. Declaring them here lets
// the editor's force-link table reference them without each
// component header having to add a separate extern declaration —
// the macro already provides one inside the matching .cpp via
// `extern const volatile bool ...; const volatile bool ... = false;`.
namespace fluxion::scene
{
    extern const volatile bool CameraForceLink;
    extern const volatile bool FadeForceLink;
    extern const volatile bool LightForceLink;
    extern const volatile bool MoveForceLink;
    extern const volatile bool ParticleSystemForceLink;
    extern const volatile bool RotateForceLink;
    extern const volatile bool ScaleForceLink;
    extern const volatile bool ShapeRendererForceLink;
    extern const volatile bool SkinnedMeshRendererForceLink;
    extern const volatile bool SpriteRendererForceLink;
    extern const volatile bool StaticMeshRendererForceLink;
    extern const volatile bool TextRendererForceLink;
}

namespace fluxion::editor
{
    namespace
    {
        // Force-link reference table for component classes that
        // self-register via FLUXION_COMPONENT_REGISTER. The editor
        // links libfluxion.lib statically, and the MSVC / ld linker
        // drops any .obj from a static archive whose symbols are not
        // directly referenced by the binary — including the .obj
        // that owns a component's AutoRegistrar global. We have to
        // reference an OUT-OF-LINE symbol from each component's
        // .cpp to actually pull the .obj in; staticTypeName is
        // inline-defined in the header so its address tells the
        // linker nothing about which .obj to load. inspect() lives
        // in the matching .cpp for every reflection-aware component,
        // so taking its address forces the .obj — and with it the
        // anonymous-namespace AutoRegistrar that registers the type.
        //
        // Adding a new FLUXION_COMPONENT_REGISTER anywhere in the
        // engine? Add a line to this table.
        void forceLinkRegisteredComponents() noexcept
        {
            // Each FLUXION_COMPONENT_REGISTER emits a named
            // namespace-scope `<ClassName>ForceLink` bool with
            // external linkage. Reading it in this function makes
            // the linker resolve the symbol against the matching
            // component .cpp — which pulls the entire .obj into
            // the editor binary, so the AutoRegistrar in that .obj
            // runs at static init and registers the type. Adding
            // a new FLUXION_COMPONENT_REGISTER anywhere in the
            // engine? Add an extern + a sink line here.
            volatile bool sink = false;
            sink ^= scene::CameraForceLink;
            sink ^= scene::FadeForceLink;
            sink ^= scene::LightForceLink;
            sink ^= scene::MoveForceLink;
            sink ^= scene::ParticleSystemForceLink;
            sink ^= scene::RotateForceLink;
            sink ^= scene::ScaleForceLink;
            sink ^= scene::ShapeRendererForceLink;
            sink ^= scene::SkinnedMeshRendererForceLink;
            sink ^= scene::SpriteRendererForceLink;
            sink ^= scene::StaticMeshRendererForceLink;
            sink ^= scene::TextRendererForceLink;
            (void)sink;
        }
    }

    EditorApp::EditorApp():
        imgui_{*engine},
        projectBundle_{engine->getCache(), engine->getFileSystem()}
    {
        forceLinkRegisteredComponents();

        // Hook the engine's OS-window close confirmation. When
        // the user clicks the X with unsaved changes pending,
        // queue the discard modal and refuse the close; the
        // modal's Discard button will set userConfirmedClose_
        // true and re-trigger close, which the handler then lets
        // through. Capture `this` by value (engine outlives
        // EditorApp inside main).
        engine->setCloseConfirmHandler([this]() -> bool {
            if (userConfirmedClose_) return true;
            if (!dirty_) return true;
            pendingDiscardAction_ = PendingDiscard::Close;
            return false;
        });

        // Enable ImGui's docking system so panel windows can be
        // tabbed / split / re-arranged by dragging their title bars.
        // ImGui persists the layout to imgui.ini in the working
        // directory, so the user's arrangement survives across runs.
        // We deliberately do NOT enable Viewports (panels in
        // separate OS windows) — that needs platform backend
        // support our engine doesn't ship for mobile / web targets,
        // and intra-window docking covers the editor's UX needs.
        ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

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
        const auto assetResult = runAssetDatabaseSelfTest();
        const auto assetRefResult = runAssetReferenceRoundTripSelfTest();
        log(Log::Level::info) << "Editor reflection self-test: " << reflectionResult;
        log(Log::Level::info) << "Editor scene round-trip self-test: " << sceneResult;
        log(Log::Level::info) << "Editor play-control self-test: " << playResult;
        log(Log::Level::info) << "Editor asset DB self-test: " << assetResult;
        log(Log::Level::info) << "Editor asset reference round-trip self-test: " << assetRefResult;

        const auto driverName =
            std::string{graphics::getDriverName(engine->getGraphics().getDevice()->getDriver())};

        panels_.push_back(std::make_unique<InfoPanel>(driverName,
                                                       reflectionResult + " | play=" + playResult,
                                                       sceneResult + " | " + assetResult
                                                       + " | assetRef=" + assetRefResult));

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
            workingScene_, selection_, &editorLayer_,
            &commandStack_);
        viewportPanel_ = viewportPanel.get();
        panels_.push_back(std::move(viewportPanel));

        panels_.push_back(std::make_unique<SceneTreePanel>(&workingScene_, &selection_,
                                                           &commandStack_,
                                                           &editorLayer_,
                                                           viewportPanel_->getCameraActor()));
        panels_.push_back(std::make_unique<InspectorPanel>(
            &selection_, &workingLayer_, &commandStack_, &assetDatabase_,
            &engine->getCache(), &imgui_));

        auto playControlPanel = std::make_unique<PlayControlPanel>(&workingScene_);
        playControl_ = playControlPanel.get();
        panels_.push_back(std::move(playControlPanel));
        // Wire the viewport's border tint to the play state now
        // that both panels are constructed. The viewport is built
        // earlier in this ctor (before PlayControlPanel) because
        // SceneTreePanel needs its camera-actor pointer; pushing
        // the back-pointer through a setter avoids reordering.
        if (viewportPanel_) viewportPanel_->setPlayControl(playControl_);

        // Sprite editor (for slicing image assets) is built BEFORE the
        // AssetBrowser so the browser's context-menu can hold a back
        // pointer to it. Both share the AssetDatabase + the project
        // bundle so save+reimport happens through the same paths the
        // editor uses everywhere else.
        auto spriteEditor = std::make_unique<SpriteEditorPanel>(
            &assetDatabase_, &projectBundle_, &engine->getCache(), &imgui_);
        SpriteEditorPanel* spriteEditorPtr = spriteEditor.get();
        panels_.push_back(std::move(spriteEditor));

        panels_.push_back(std::make_unique<AssetBrowserPanel>(
            &assetDatabase_, &engine->getCache(), &imgui_,
            &selection_, spriteEditorPtr));

        panels_.push_back(std::make_unique<SpriteAnimationPanel>(&selection_, playControl_));

        imgui_.setDrawCallback([this]() {
            // Fullscreen dockspace host that covers the main viewport.
            // PassthruCentralNode keeps the central area transparent
            // so the engine's back-buffer clear (editor camera) shows
            // through wherever no panel is docked. Each panel ImGui
            // window can be dragged onto an edge / centre of this
            // dockspace to dock it; ImGui persists the layout in
            // imgui.ini between runs.
            ImGui::DockSpaceOverViewport(0, nullptr,
                ImGuiDockNodeFlags_PassthruCentralNode);

            // Detect Play -> Editing transitions (= Stop) so we can
            // clear the undo history. The snapshot/restore that
            // Stop performs returns the scene to its pre-Play
            // state, but any commands the user pushed during Play
            // (drop-to-create, asset re-bind, ...) still reference
            // actor UUIDs that no longer exist. Wiping the stack
            // avoids the ensuing inconsistencies on future undo /
            // redo. Per-frame poll keeps the EditorApp / panel
            // contract loose — no panel-state callback machinery.
            if (playControl_)
            {
                const int curState =
                    static_cast<int>(playControl_->state());
                const int editingValue =
                    static_cast<int>(PlayControlPanel::State::Editing);
                if (lastPlayState_ != editingValue && curState == editingValue)
                    commandStack_.clear();
                lastPlayState_ = curState;
            }

            drawMainMenu();
            // Top-level keyboard shortcuts: Ctrl+Z / Ctrl+Y / Ctrl+
            // Shift+Z drive undo/redo regardless of which panel has
            // focus, so the user can hit them after any mutation.
            // Frozen during Play / Pause for the same reason the
            // Edit menu items are frozen — undo/redo would diverge
            // the live scene from the pre-Play snapshot.
            const ImGuiIO& io = ImGui::GetIO();
            const bool ctrl = io.KeyCtrl;
            const bool shift = io.KeyShift;
            const bool isEditingForUndo = !playControl_ ||
                playControl_->state() == PlayControlPanel::State::Editing;
            if (isEditingForUndo &&
                ctrl && !shift && ImGui::IsKeyPressed(ImGuiKey_Z, false))
                commandStack_.undo();
            if (isEditingForUndo &&
                ((ctrl && shift && ImGui::IsKeyPressed(ImGuiKey_Z, false)) ||
                 (ctrl && !shift && ImGui::IsKeyPressed(ImGuiKey_Y, false))))
                commandStack_.redo();

            updateDirty();

            for (const auto& p : panels_)
                if (p && p->isVisible())
                    p->draw();
            drawFileDialog();
            drawDiscardModal();
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

        // Undo/redo are scene-mutation operations: running them
        // mid-Play would diverge the live scene from the pre-Play
        // snapshot, breaking Stop's restore. Frozen during
        // Playing/Paused; the menu items show the same labels but
        // greyed out so the user can see what would be there.
        const bool isEditing = !playControl_ ||
            playControl_->state() == PlayControlPanel::State::Editing;

        const auto undoLabel = commandStack_.undoLabel();
        const auto redoLabel = commandStack_.redoLabel();
        char buf[160];
        std::snprintf(buf, sizeof(buf), "Undo%s%s",
                      undoLabel.empty() ? "" : " ",
                      undoLabel.c_str());
        if (ImGui::MenuItem(buf, "Ctrl+Z",
                            /*selected*/ false,
                            /*enabled*/ isEditing && commandStack_.canUndo()))
            commandStack_.undo();
        std::snprintf(buf, sizeof(buf), "Redo%s%s",
                      redoLabel.empty() ? "" : " ",
                      redoLabel.c_str());
        if (ImGui::MenuItem(buf, "Ctrl+Y",
                            /*selected*/ false,
                            /*enabled*/ isEditing && commandStack_.canRedo()))
            commandStack_.redo();
        ImGui::EndMenu();
    }

    void EditorApp::drawMainMenu()
    {
        if (!ImGui::BeginMainMenuBar()) return;

        if (ImGui::BeginMenu("File"))
        {
            // File ops mutate or replace the live scene, which would
            // either invalidate Play Mode's pre-Play snapshot
            // (Save Scene serialises the post-Play, possibly
            // already-mutated state) or wipe the scene the snapshot
            // refers to (New / Open). Frozen until Stop returns the
            // editor to Editing state.
            const bool isEditing = !playControl_ ||
                playControl_->state() == PlayControlPanel::State::Editing;

            if (ImGui::MenuItem("New Scene", nullptr,
                                /*selected*/ false,
                                /*enabled*/ isEditing))
            {
                if (dirty_) pendingDiscardAction_ = PendingDiscard::NewScene;
                else        doNewScene();
            }

            if (ImGui::MenuItem("Open Scene...", nullptr,
                                /*selected*/ false,
                                /*enabled*/ isEditing))
            {
                if (dirty_)
                {
                    pendingDiscardAction_ = PendingDiscard::OpenScene;
                }
                else
                {
                    fileDialogMode_ = FileDialogMode::Open;
                    std::memset(fileDialogBuf_, 0, sizeof(fileDialogBuf_));
                    if (!currentScenePath_.empty())
                        std::snprintf(fileDialogBuf_, sizeof(fileDialogBuf_),
                                      "%s", currentScenePath_.c_str());
                }
            }

            // "Save" only enabled once we know where the scene came
            // from — otherwise we route through Save As to ask the
            // user where to write it.
            const bool haveSavePath = !currentScenePath_.empty();
            if (ImGui::MenuItem("Save Scene", "Ctrl+S",
                                /*selected*/ false,
                                /*enabled*/ isEditing && haveSavePath))
                doSaveScene();

            if (ImGui::MenuItem("Save Scene As...", nullptr,
                                /*selected*/ false,
                                /*enabled*/ isEditing))
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

        // Top-toolbar play controls. Drives the same state machine
        // the PlayControlPanel exposes — the panel stays around as
        // a "Time" details surface (timeScale slider, status
        // string), but the user can hit Play / Pause / Stop without
        // hunting for the panel.
        if (playControl_)
        {
            using S = PlayControlPanel::State;
            const auto state = playControl_->state();
            ImGui::Separator();

            // Tinted Play button when in Editing so the eye finds
            // it immediately; greyed-out when already running.
            const bool playEnabled = (state == S::Editing);
            ImGui::BeginDisabled(!playEnabled);
            if (state == S::Playing)
                ImGui::PushStyleColor(ImGuiCol_Button,
                                      ImVec4(0.20F, 0.65F, 0.30F, 1.0F));
            if (ImGui::Button("\xE2\x96\xB6 Play"))    // u25B6 ▶
                playControl_->enterPlay();
            if (state == S::Playing)
                ImGui::PopStyleColor();
            ImGui::EndDisabled();

            ImGui::BeginDisabled(state != S::Playing);
            if (ImGui::Button("\xE2\x9D\x9A Pause"))   // u275A ❚
                playControl_->enterPause();
            ImGui::EndDisabled();

            ImGui::BeginDisabled(state != S::Paused);
            if (ImGui::Button("\xE2\x96\xB6\xE2\x96\xB6 Resume"))
                playControl_->enterResume();
            ImGui::EndDisabled();

            ImGui::BeginDisabled(state != S::Paused);
            if (ImGui::Button("Step"))
                playControl_->stepOne();
            ImGui::EndDisabled();

            ImGui::BeginDisabled(state == S::Editing);
            if (ImGui::Button("\xE2\x96\xA0 Stop"))    // u25A0 ■
                playControl_->enterStop();
            ImGui::EndDisabled();
        }

        // Right-side info: current scene path + a transient status
        // message after save/load. Helps the user see at a glance
        // which file they're editing without having to dig into the
        // file dialog. The trailing "*" appears whenever dirty_ is
        // set — Unity-style unsaved indicator.
        ImGui::SameLine(ImGui::GetWindowWidth() - 480.0F);
        const char* dirtyMark = dirty_ ? "*" : "";
        if (!currentScenePath_.empty())
            ImGui::TextDisabled("[%s%s]", currentScenePath_.c_str(), dirtyMark);
        else
            ImGui::TextDisabled("[unsaved%s]", dirtyMark);
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
        savedUndoCount_ = commandStack_.undoCount();      // baseline = 0
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
            // Successfully written: snapshot the undo count so the
            // dirty mark clears until the next mutation.
            savedUndoCount_ = commandStack_.undoCount();
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
            // Just-loaded scene matches its on-disk form, so reset
            // the dirty baseline.
            savedUndoCount_ = commandStack_.undoCount();
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

    std::string EditorApp::runAssetDatabaseSelfTest()
    {
        // Phase 11b verification: walk samples/Resources, mint a
        // .meta sidecar for every asset that doesn't have one, and
        // hand the project root to the engine's FileSystem so the
        // import pipeline can resolve relative paths.
        //
        // Project root resolution: we don't know which directory the
        // editor was launched from (cwd is the binary dir under MSVC,
        // the project root under VS Code's launch), so try the three
        // most common candidate paths and pick the first that exists.
        // Mobile/web targets aren't editor targets — desktop only.
        const std::array<const char*, 3> candidates = {
            "samples/Resources",
            "../samples/Resources",
            "../../samples/Resources"
        };
        std::string root;
        for (const auto* c : candidates)
        {
            std::error_code ec;
            if (std::filesystem::is_directory(c, ec)) { root = c; break; }
        }

        if (root.empty()) return "SKIP (no samples/Resources nearby)";

        if (!assetDatabase_.setProjectRoot(root))
            return "FAIL setProjectRoot";

        const auto scan = assetDatabase_.rescan();

        // Make the project root part of the engine's resource search
        // path so Bundle::loadAsset(name, relativePath) can find the
        // file by relative path. addResourcePath is idempotent — a
        // future "switch project" call would removeResourcePath the
        // old root before adding the new one.
        engine->getFileSystem().addResourcePath(
            storage::Path{assetDatabase_.getProjectRoot()});

        // Push every importable asset through the loader chain so
        // the Cache becomes addressable by both name and uuid. Image
        // thumbnails in the AssetBrowser come from cache.getTexture(id)
        // — without this the panel would show only fallback tiles.
        // Failures (corrupt files, dependencies missing) are logged
        // by importAll itself and counted here for the InfoPanel.
        const auto imp = importAll(projectBundle_, assetDatabase_);

        std::string out = "PASS scanned=" + std::to_string(scan.scanned)
            + " loaded=" + std::to_string(scan.metaLoaded)
            + " created=" + std::to_string(scan.metaCreated)
            + " imported=" + std::to_string(imp.imported);
        if (imp.failed > 0)
            out += " (" + std::to_string(imp.failed) + " import-fail)";
        if (scan.metaInvalid > 0)
            out += " (" + std::to_string(scan.metaInvalid) + " meta-invalid)";
        return out;
    }

    void EditorApp::updateDirty()
    {
        // Coarse but cheap dirty signal: undo-count delta from the
        // last save. Every Command::push grows undoStack_; an undo
        // shrinks it. Rolling back to exactly the saved count still
        // reads as dirty when intermediate redo-only commands are
        // pending — acceptable for the badge, since "exactly the
        // bytes on disk" tracking would need content hashing.
        dirty_ = (commandStack_.undoCount() != savedUndoCount_);
    }

    void EditorApp::drawDiscardModal()
    {
        if (pendingDiscardAction_ == PendingDiscard::None) return;

        constexpr const char* popupId = "Discard unsaved changes?##editor_discard";
        if (!ImGui::IsPopupOpen(popupId))
            ImGui::OpenPopup(popupId);

        if (ImGui::BeginPopupModal(popupId, nullptr,
                                    ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::TextWrapped(
                "The current scene has unsaved changes. "
                "Continue and lose them, or cancel to keep editing?");
            ImGui::Separator();

            const bool discard = ImGui::Button("Discard");
            ImGui::SameLine();
            const bool cancel = ImGui::Button("Cancel");

            if (discard)
            {
                executePendingDiscard();
                ImGui::CloseCurrentPopup();
            }
            else if (cancel)
            {
                pendingDiscardAction_ = PendingDiscard::None;
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
    }

    void EditorApp::executePendingDiscard()
    {
        const auto action = pendingDiscardAction_;
        pendingDiscardAction_ = PendingDiscard::None;
        switch (action)
        {
            case PendingDiscard::None: return;
            case PendingDiscard::NewScene:
                doNewScene();
                break;
            case PendingDiscard::OpenScene:
                fileDialogMode_ = FileDialogMode::Open;
                std::memset(fileDialogBuf_, 0, sizeof(fileDialogBuf_));
                if (!currentScenePath_.empty())
                    std::snprintf(fileDialogBuf_, sizeof(fileDialogBuf_),
                                  "%s", currentScenePath_.c_str());
                break;
            case PendingDiscard::Close:
                // Mark the close as confirmed and trigger another
                // exit so the engine's WM_CLOSE handler lets it
                // through this time. engine->exit() does the
                // cross-platform shutdown shake-down.
                userConfirmedClose_ = true;
                engine->exit();
                break;
        }
    }

    std::string EditorApp::runAssetReferenceRoundTripSelfTest()
    {
        // Phase 11c verification: drive a JsonWriter + JsonReader pair
        // directly with a fabricated AssetReference and confirm the
        // id and type round-trip through the JSON form. No real
        // component is involved — we only need to prove the new
        // assetField virtual is wired into both inspectors.
        assets::AssetReference written{Uuid::generate(),
                                       assets::Asset::Type::sprite};
        json::Value root;
        {
            reflection::JsonWriter writer{root};
            writer.assetField("texture", written);
        }

        // Sanity: the writer must have produced { "texture": { "id":..., "type":"sprite" } }
        if (!root.is<json::Object>() ||
            !root.hasMember("texture") ||
            !root["texture"].is<json::Object>())
            return "FAIL writer shape";
        const auto& sub = root["texture"];
        if (!sub.hasMember("id") || !sub["id"].is<std::string>())
            return "FAIL writer id";
        if (!sub.hasMember("type") || !sub["type"].is<std::string>() ||
            sub["type"].as<std::string>() != "sprite")
            return "FAIL writer type";

        assets::AssetReference read{};   // null id, default type=image
        {
            reflection::JsonReader reader{root};
            reader.assetField("texture", read);
        }
        if (read.id != written.id) return "FAIL reader id";
        if (read.type != written.type) return "FAIL reader type";

        // Verify forwards-compat: a JSON object missing the asset
        // field leaves the destination at its constructor default.
        json::Value empty;
        empty = json::Object{};
        assets::AssetReference fallback{};
        {
            reflection::JsonReader reader{empty};
            reader.assetField("texture", fallback);
        }
        if (!fallback.id.isNull()) return "FAIL missing-field default";

        return "PASS";
    }
}
