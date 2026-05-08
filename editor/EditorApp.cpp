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
#include "panels/ReflectionPanel.hpp"

namespace fluxion::editor
{
    EditorApp::EditorApp(): imgui_{*engine}
    {
        engine->setScreenSaverEnabled(false);

        // Self-tests run once at startup. Their results are passed to
        // the InfoPanel so the user sees them on first frame; failures
        // are also logged for CI to scrape.
        const auto reflectionResult = runReflectionSelfTest();
        const auto sceneResult = runSceneRoundTripSelfTest();
        log(Log::Level::info) << "Editor reflection self-test: " << reflectionResult;
        log(Log::Level::info) << "Editor scene round-trip self-test: " << sceneResult;

        const auto driverName =
            std::string{graphics::getDriverName(engine->getGraphics().getDevice()->getDriver())};

        panels_.push_back(std::make_unique<InfoPanel>(driverName,
                                                       reflectionResult,
                                                       sceneResult));
        panels_.push_back(std::make_unique<ReflectionPanel>());

        imgui_.setDrawCallback([this]() {
            for (const auto& p : panels_)
                if (p && p->isVisible())
                    p->draw();
        });
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
}
