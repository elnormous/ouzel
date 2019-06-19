// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "ouzel.hpp"
#include "MainMenu.hpp"
#include "SpritesSample.hpp"
#include "GUISample.hpp"
#include "RTSample.hpp"
#include "AnimationsSample.hpp"
#include "InputSample.hpp"
#include "SoundSample.hpp"
#include "PerspectiveSample.hpp"

using namespace ouzel;

class Samples: public ouzel::Application
{
public:
    explicit Samples(const std::vector<std::string>& args):
        bundle(new assets::Bundle(engine->getCache(), engine->getFileSystem()))
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
                    ouzel::engine->log(ouzel::Log::Level::WARN) << "No sample specified";
            }
            else
                ouzel::engine->log(ouzel::Log::Level::WARN) << "Invalid argument \"" << *arg << "\"";
        }

        engine->getFileSystem().addResourcePath("Resources");

#if !defined(__ANDROID__)
        ouzel::storage::Archive archive(engine->getFileSystem().getPath("gui.zip"));
        engine->getFileSystem().addArchive("gui.zip", std::move(archive));
#endif

        bundle->loadAssets("assets.json");

        std::unique_ptr<ouzel::scene::Scene> currentScene;

        if (!sample.empty())
        {
            if (sample == "sprites") currentScene.reset(new SpritesSample());
            else if (sample == "gui") currentScene.reset(new GUISample());
            else if (sample == "render_target") currentScene.reset(new RTSample());
            else if (sample == "animations") currentScene.reset(new AnimationsSample());
            else if (sample == "input") currentScene.reset(new InputSample());
            else if (sample == "sound") currentScene.reset(new SoundSample());
            else if (sample == "perspective") currentScene.reset(new PerspectiveSample());
        }

        if (!currentScene) currentScene.reset(new MainMenu());

        engine->getSceneManager().setScene(std::move(currentScene));
    }
private:
    std::unique_ptr<assets::Bundle> bundle;
};

std::unique_ptr<ouzel::Application> ouzel::main(const std::vector<std::string>& args)
{
    return std::unique_ptr<Application>(new Samples(args));
}
