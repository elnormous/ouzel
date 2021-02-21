// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include "MainMenu.hpp"
#include "SpritesSample.hpp"
#include "GUISample.hpp"
#include "RTSample.hpp"
#include "AnimationsSample.hpp"
#include "InputSample.hpp"
#include "SoundSample.hpp"
#include "PerspectiveSample.hpp"

using namespace ouzel;

namespace samples
{
    class Samples: public ouzel::Application
    {
    public:
        explicit Samples(const std::vector<std::string>& args):
            bundle(std::make_unique<assets::Bundle>(engine->getCache(), engine->getFileSystem()))
        {
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
                        ouzel::logger.log(ouzel::Log::Level::warning) << "No sample specified";
                }
                else
                    ouzel::logger.log(ouzel::Log::Level::warning) << "Invalid argument \"" << *arg << "\"";
            }

#if !defined(__ANDROID__)
            ouzel::storage::Archive archive(engine->getFileSystem().getPath("gui.zip"));
            engine->getFileSystem().addArchive("gui.zip", std::move(archive));
#endif

            bundle->loadAssets("assets.json");

            std::unique_ptr<ouzel::scene::Scene> currentScene;

            if (!sample.empty())
            {
                if (sample == "sprites") currentScene = std::make_unique<SpritesSample>();
                else if (sample == "gui") currentScene = std::make_unique<GUISample>();
                else if (sample == "render_target") currentScene = std::make_unique<RTSample>();
                else if (sample == "animations") currentScene = std::make_unique<AnimationsSample>();
                else if (sample == "input") currentScene = std::make_unique<InputSample>();
                else if (sample == "sound") currentScene = std::make_unique<SoundSample>();
                else if (sample == "perspective") currentScene = std::make_unique<PerspectiveSample>();
            }

            if (!currentScene) currentScene = std::make_unique<MainMenu>();

            engine->getSceneManager().setScene(std::move(currentScene));
        }
    private:
        std::unique_ptr<assets::Bundle> bundle;
    };
}

std::unique_ptr<ouzel::Application> ouzel::main(const std::vector<std::string>& args)
{
    return std::make_unique<samples::Samples>(args);
}
