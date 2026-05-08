#include "MainMenu.hpp"
#include "SpritesSample.hpp"
#include "GUISample.hpp"
#include "RTSample.hpp"
#include "AnimationsSample.hpp"
#include "InputSample.hpp"
#include "SoundSample.hpp"
#include "PerspectiveSample.hpp"
#include "ImGuiSample.hpp"
#include "ImGuiHost.hpp"
#include "../engine/gui/imgui/ImGuiInspector.hpp"
#include "../engine/reflection/JsonInspector.hpp"
#include "../engine/reflection/TypeRegistry.hpp"
#include "../engine/scene/Camera.hpp"
#include "../engine/scene/Component.hpp"
#include "../engine/utils/Log.hpp"
#include "../engine/utils/Uuid.hpp"
#include "imgui.h"

using namespace fluxion;

namespace samples
{
    ImGuiHost& imguiHost() noexcept
    {
        static ImGuiHost instance;
        return instance;
    }

    namespace
    {
        // Result of the startup self-test, surfaced in the smoke-test panel.
        std::string& selfTestResult()
        {
            static std::string r{"not run"};
            return r;
        }

        // In-process self-test: round-trips one component through JsonWriter
        // and JsonReader and asserts the inspectable state survives. Logs
        // pass/fail at info/error level on engine startup so a regression
        // is visible immediately, without needing a unit-test harness.
        void runReflectionSelfTest()
        {
            auto cam = reflection::TypeRegistry::instance().create("scene.Camera");
            if (!cam)
            {
                selfTestResult() = "FAIL: scene.Camera not registered";
                fluxion::log(Log::Level::error) << "Reflection self-test: scene.Camera not registered";
                return;
            }
            auto* camera = static_cast<scene::Camera*>(cam.get());

            const float testFov = 0.7F;
            const float testNear = 12.5F;
            const float testFar = 250.0F;
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
            if (ok)
            {
                selfTestResult() = "PASS (scene.Camera round-trip)";
                fluxion::log(Log::Level::info) << "Reflection self-test: scene.Camera round-trip OK";
            }
            else
            {
                selfTestResult() = "FAIL (round-trip mismatch)";
                fluxion::log(Log::Level::error) << "Reflection self-test: scene.Camera round-trip MISMATCH"
                                                << " fov=" << camera2->getFov() << "/" << testFov
                                                << " near=" << camera2->getNearPlane() << "/" << testNear
                                                << " far=" << camera2->getFarPlane() << "/" << testFar;
            }
        }
    }

    namespace
    {
        void drawTextRefWindow()
        {
            ImGui::SetNextWindowPos(ImVec2(20, 220), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(640, 280), ImGuiCond_FirstUseEver);
            ImGui::Begin("Text rendering reference");
            ImGui::TextUnformatted("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
            ImGui::TextUnformatted("abcdefghijklmnopqrstuvwxyz");
            ImGui::TextUnformatted("0123456789  !@#$%^&*()_+-=[]{}|;:,.<>?/");
            ImGui::TextUnformatted("The quick brown fox jumps over the lazy dog.");
            ImGui::Separator();
            ImGui::TextColored(ImVec4(1.0F, 0.4F, 0.4F, 1.0F), "Red text  - kerning: AVA TaT WoWo fl ffi rn cl");
            ImGui::TextColored(ImVec4(0.4F, 1.0F, 0.4F, 1.0F), "Green text - small chars: i.l!1Il|");
            ImGui::TextColored(ImVec4(0.4F, 0.7F, 1.0F, 1.0F), "Blue text - wide letters: WMmwHX@#%%");
            ImGui::Separator();
            ImGui::Text("Color block test:");
            ImDrawList* dl = ImGui::GetWindowDrawList();
            ImVec2 p = ImGui::GetCursorScreenPos();
            for (int i = 0; i < 8; ++i)
            {
                const float v = i / 7.0F;
                dl->AddRectFilled(ImVec2(p.x + i * 24, p.y),
                                  ImVec2(p.x + i * 24 + 22, p.y + 22),
                                  IM_COL32(static_cast<int>(v * 255),
                                           static_cast<int>(v * 255),
                                           static_cast<int>(v * 255), 255));
            }
            ImGui::Dummy(ImVec2(0, 26));
            ImGui::Text("(grayscale ramp: black -> white)");
            ImGui::End();
        }

        void drawReflectionWindow()
        {
            ImGui::SetNextWindowPos(ImVec2(20, 470), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(560, 460), ImGuiCond_FirstUseEver);
            ImGui::Begin("Reflection + serialization");

            const auto types = reflection::TypeRegistry::instance().listTypes();
            ImGui::Text("TypeRegistry has %zu component type(s):", types.size());
            ImGui::Indent();
            for (const auto& t : types) ImGui::BulletText("%s", t.c_str());
            ImGui::Unindent();
            ImGui::Separator();

            // Live editor for one component instance per registered type.
            // Uses TypeRegistry::create for the factory side (proves the
            // registration works) and Component::inspect for the metadata
            // side (drives the ImGui-based Inspector).
            static std::string selectedType = types.empty() ? std::string{} : types.front();
            if (ImGui::BeginCombo("##type", selectedType.c_str()))
            {
                for (const auto& t : types)
                {
                    const bool sel = (t == selectedType);
                    if (ImGui::Selectable(t.c_str(), sel)) selectedType = t;
                    if (sel) ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }

            static std::string instantiatedType;
            static std::unique_ptr<scene::Component> instance;
            if (instantiatedType != selectedType)
            {
                instance = reflection::TypeRegistry::instance().create(selectedType);
                instantiatedType = selectedType;
            }

            if (instance)
            {
                ImGui::TextDisabled("Editing a temporary instance of %s",
                                    selectedType.c_str());
                ImGui::Separator();
                gui::imgui::Inspector inspector;
                instance->inspect(inspector);

                ImGui::Separator();
                ImGui::TextDisabled("Serialization round-trip (phase 3)");

                // Persistent buffer for the JSON text area. We capture the
                // current state on demand and the user can edit it freely
                // before clicking Load to push it back into the instance.
                static std::string jsonBuffer{"(click 'Serialize' to populate)"};
                static std::string lastStatus;
                static Uuid demoId;

                if (ImGui::Button("Generate UUID"))
                    demoId = Uuid::generate();
                ImGui::SameLine();
                ImGui::Text("uuid: %s", demoId.toString().c_str());

                if (ImGui::Button("Serialize"))
                {
                    json::Value out;
                    reflection::serializeComponent(*instance, out);
                    jsonBuffer = json::encode(out, /*whiteSpaces*/ true);
                    lastStatus = "Serialized.";
                }
                ImGui::SameLine();
                if (ImGui::Button("Load"))
                {
                    try
                    {
                        const auto parsed = json::parse(jsonBuffer);
                        reflection::deserializeComponent(*instance, parsed);
                        lastStatus = "Loaded.";
                    }
                    catch (const std::exception& e)
                    {
                        lastStatus = std::string{"Parse error: "} + e.what();
                    }
                }
                ImGui::SameLine();
                ImGui::TextDisabled("%s", lastStatus.c_str());

                struct Cb {
                    static int resize(ImGuiInputTextCallbackData* data) {
                        if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
                            auto* s = static_cast<std::string*>(data->UserData);
                            s->resize(static_cast<std::size_t>(data->BufTextLen));
                            data->Buf = s->data();
                        }
                        return 0;
                    }
                };
                if (jsonBuffer.capacity() < 1024) jsonBuffer.reserve(1024);
                ImGui::InputTextMultiline("##json",
                                          jsonBuffer.data(), jsonBuffer.capacity() + 1,
                                          ImVec2(-1, 120),
                                          ImGuiInputTextFlags_CallbackResize,
                                          Cb::resize, &jsonBuffer);
            }
            else
            {
                ImGui::TextColored(ImVec4(1, 0.4F, 0.4F, 1),
                                   "TypeRegistry::create returned null");
            }

            ImGui::End();
        }

        void drawSmokeTestWindow(const std::string& driverName)
        {
            ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiCond_FirstUseEver);
            ImGui::SetNextWindowSize(ImVec2(420, 180), ImGuiCond_FirstUseEver);
            ImGui::Begin("Fluxion ImGui smoke test");
            const ImGuiIO& io = ImGui::GetIO();
            ImGui::Text("Renderer: %s", driverName.c_str());
            ImGui::Text("Display: %.0f x %.0f", io.DisplaySize.x, io.DisplaySize.y);
            const bool hasMouse = io.MousePos.x > -1.0e6F;
            if (hasMouse)
                ImGui::Text("Mouse: %.0f, %.0f  Frame %d  %.1f FPS",
                            io.MousePos.x, io.MousePos.y,
                            ImGui::GetFrameCount(), io.Framerate);
            else
                ImGui::Text("Mouse: (move pointer over window)  Frame %d  %.1f FPS",
                            ImGui::GetFrameCount(), io.Framerate);
            static float slider = 0.5F;
            ImGui::SliderFloat("slider", &slider, 0.0F, 1.0F);
            ImGui::Text("WantCapture: kbd=%d mouse=%d",
                        io.WantCaptureKeyboard ? 1 : 0,
                        io.WantCaptureMouse ? 1 : 0);
            ImGui::TextWrapped("Open the \"ImGui\" sample to toggle these windows from native Fluxion buttons.");
            ImGui::Separator();
            ImGui::Text("Reflection self-test: %s", selfTestResult().c_str());
            ImGui::End();
        }
    }

    class Samples: public fluxion::Application
    {
    public:
        explicit Samples(const std::vector<std::string>& args):
            bundle{engine->getCache(), engine->getFileSystem()},
            imgui{*engine}
        {
            // Publish the context to the ImGuiSample scene through the shared
            // ImGuiHost. Toggle flags drive what the per-frame draw callback
            // emits; the master switch is on the Context itself.
            imguiHost().context = &imgui;

            // Run the reflection round-trip self-test once at startup so a
            // regression in serialization shows up in the log even if the
            // user never opens the Reflection panel.
            runReflectionSelfTest();

            const auto driverName = std::string{graphics::getDriverName(engine->getGraphics().getDevice()->getDriver())};
            imgui.setDrawCallback([driverName]() {
                const auto& host = imguiHost();
                if (host.showSmokeTest.load(std::memory_order_acquire))
                    drawSmokeTestWindow(driverName);
                if (host.showTextRef.load(std::memory_order_acquire))
                    drawTextRefWindow();
                if (host.showReflection.load(std::memory_order_acquire))
                    drawReflectionWindow();
                if (host.showDemo.load(std::memory_order_acquire))
                {
                    ImGui::ShowDemoWindow();
                    // ShowDemoWindow internally calls SetNextWindowPos with
                    // FirstUseEver. We pin its position by name once (on the
                    // first frame the demo appears) so the centered native
                    // Fluxion buttons stay clickable. Track whether we've
                    // already pinned to avoid fighting the user's drags.
                    static bool demoPinned = false;
                    if (!demoPinned)
                    {
                        const ImVec2 dispSize = ImGui::GetIO().DisplaySize;
                        constexpr ImVec2 demoSize{540.0F, 680.0F};
                        ImGui::SetWindowPos("Dear ImGui Demo",
                                            ImVec2(dispSize.x - demoSize.x - 10.0F, 10.0F),
                                            ImGuiCond_Always);
                        ImGui::SetWindowSize("Dear ImGui Demo", demoSize,
                                             ImGuiCond_Always);
                        demoPinned = true;
                    }
                }
            });

            // disable screen saver
            engine->setScreenSaverEnabled(false);

            std::string sample;

            for (auto arg = args.begin(); arg != args.end(); ++arg)
            {
                if (arg == args.begin())
                {
                    // skip the first parameter
                    continue;
                }

                if (*arg == "-sample")
                {
                    if (++arg != args.end())
                        sample = *arg;
                    else
                        fluxion::log(fluxion::Log::Level::warning) << "No sample specified";
                }
                else
                    fluxion::log(fluxion::Log::Level::warning) << "Invalid argument \"" << *arg << "\"";
            }

#ifndef __ANDROID__
            fluxion::storage::Archive archive{engine->getFileSystem().getPath("gui.zip")};
            engine->getFileSystem().addArchive("gui.zip", std::move(archive));
#endif

            bundle.loadAssets("assets.json");

            std::unique_ptr<fluxion::scene::Scene> currentScene;

            if (!sample.empty())
            {
                if (sample == "sprites") currentScene = std::make_unique<SpritesSample>();
                else if (sample == "gui") currentScene = std::make_unique<GUISample>();
                else if (sample == "render_target") currentScene = std::make_unique<RTSample>();
                else if (sample == "animations") currentScene = std::make_unique<AnimationsSample>();
                else if (sample == "input") currentScene = std::make_unique<InputSample>();
                else if (sample == "sound") currentScene = std::make_unique<SoundSample>();
                else if (sample == "perspective") currentScene = std::make_unique<PerspectiveSample>();
                else if (sample == "imgui") currentScene = std::make_unique<ImGuiSample>();
            }

            if (!currentScene) currentScene = std::make_unique<MainMenu>();

            engine->getSceneManager().setScene(std::move(currentScene));
        }
    private:
        assets::Bundle bundle;
        gui::imgui::Context imgui;
    };
}

std::unique_ptr<fluxion::Application> fluxion::main(const std::vector<std::string>& args)
{
    return std::make_unique<samples::Samples>(args);
}
