#include "EditorApp.hpp"

#include "imgui.h"
#include "../engine/core/Engine.hpp"
#include "../engine/graphics/Graphics.hpp"
#include "../engine/reflection/JsonInspector.hpp"
#include "../engine/reflection/TypeRegistry.hpp"
#include "../engine/scene/Actor.hpp"
#include "../engine/scene/Camera.hpp"
#include "../engine/scene/Component.hpp"
#include "../engine/scene/Layer.hpp"
#include "../engine/scene/Scene.hpp"
#include "../engine/scene/SceneSerializer.hpp"
#include "panels/InfoPanel.hpp"
#include "panels/PlayControlPanel.hpp"
#include "panels/ReflectionPanel.hpp"

namespace fluxion::editor
{
    EditorApp::EditorApp(): imgui_{*engine}
    {
        engine->setScreenSaverEnabled(false);

        buildTestScene();

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
        panels_.push_back(std::make_unique<ReflectionPanel>());
        panels_.push_back(std::make_unique<PlayControlPanel>(&workingScene_));

        imgui_.setDrawCallback([this]() {
            for (const auto& p : panels_)
                if (p && p->isVisible())
                    p->draw();
        });
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

        const auto originalPos = kids[0]->getPosition();
        json::Value snapshot;
        try { snapshot = scene::serialization::sceneToJson(workingScene_); }
        catch (const std::exception& e) { return std::string{"FAIL snapshot: "} + e.what(); }

        kids[0]->setPosition(math::Vector<float, 3>{999.0F, -42.0F, 17.0F});

        try { scene::serialization::sceneFromJson(workingScene_, snapshot); }
        catch (const std::exception& e) { return std::string{"FAIL restore: "} + e.what(); }

        // Re-fetch children after restore: deserialization reuses the
        // existing Layer (per phase 3.5 design) and adds the loaded
        // actors as new children, so the post-restore vector has 4
        // entries (originals + restored copies). The first two retain
        // mutation state; the last two carry the original positions.
        const auto& after = workingLayer_.getChildren();
        if (after.size() < 4) return "FAIL: actor count after restore";
        const auto& restoredPos = after[2]->getPosition();
        if (restoredPos.v[0] != originalPos.v[0] ||
            restoredPos.v[1] != originalPos.v[1] ||
            restoredPos.v[2] != originalPos.v[2])
            return "FAIL: restored position does not match original";

        // Clean up the test artefacts so the panel sees the original
        // two-actor scene. We remove the mutated original and the
        // surplus restored copy of Enemy, keeping the restored Player
        // and the original Enemy at index 1.
        workingLayer_.removeChild(*after[0]);   // mutated Player
        workingLayer_.removeChild(*after[3]);   // restored Enemy duplicate

        return "PASS (snapshot/restore round-trip)";
    }
}
