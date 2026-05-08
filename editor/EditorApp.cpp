#include "EditorApp.hpp"

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
                                                           &editorLayer_,
                                                           viewportPanel_->getCameraActor()));
        panels_.push_back(std::make_unique<InspectorPanel>(&selection_));
        panels_.push_back(std::make_unique<PlayControlPanel>(&workingScene_));

        imgui_.setDrawCallback([this]() {
            for (const auto& p : panels_)
                if (p && p->isVisible())
                    p->draw();
        });
        // Picking + AABB overlay live entirely inside ViewportPanel
        // now: clicks come from ImGui::IsItemClicked on the image,
        // the AABB outlines render on the panel's window draw list.
        // The back-buffer area outside any panel is just cleared by
        // editorCamera_ — no editor decoration there.
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
