// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <cstdlib>
#include <emscripten.h>
#include "EngineEm.hpp"
#include "../../audio/AudioDevice.hpp"
#include "../../audio/openal/OALAudioDevice.hpp"
#include "../../graphics/RenderDevice.hpp"
#include "../../input/emscripten/InputSystemEm.hpp"
#include "../../utils/Log.hpp"

namespace ouzel::core::emscripten
{
    namespace
    {
        std::vector<std::string> parseArgs(int argc, char* argv[])
        {
            std::vector<std::string> result;
            for (int i = 0; i < argc; ++i)
                result.push_back(argv[i]);
            return result;
        }

        void loop(void* arg)
        {
            static_cast<emscripten::Engine*>(arg)->step();
        }
    }

    Engine::Engine(int argc, char* argv[]):
        core::Engine(parseArgs(argc, argv))
    {
    }

    void Engine::run()
    {
        init();
        start();

        emscripten_set_main_loop_arg(loop, this, 0, 1);
    }

    void Engine::step()
    {
        auto inputEm = static_cast<input::emscripten::InputSystem*>(inputManager->getInputSystem());

        if (active)
        {
            try
            {
                update();
            }
            catch (const std::exception& e)
            {
                logger.log(Log::Level::error) << e.what();
                exit();
            }

            try
            {
                audio->update();
            }
            catch (const std::exception& e)
            {
                logger.log(Log::Level::error) << e.what();
            }

            try
            {
                graphics->getDevice()->process();
            }
            catch (const std::exception& e)
            {
                logger.log(Log::Level::error) << e.what();
            }

            if (audio->getDevice()->getDriver() == audio::Driver::openAL)
            {
                auto audioDevice = static_cast<audio::openal::AudioDevice*>(audio->getDevice());
                try
                {
                    audioDevice->process();
                }
                catch (const std::exception& e)
                {
                    logger.log(Log::Level::error) << e.what();
                }
            }

            try
            {
                inputEm->update();
            }
            catch (const std::exception& e)
            {
                logger.log(Log::Level::error) << e.what();
            }
        }
        else
            emscripten_cancel_main_loop();
    }

    void Engine::runOnMainThread(const std::function<void()>& func)
    {
        if (func) func();
    }

    void Engine::openUrl(const std::string& url)
    {
        EM_ASM_ARGS({window.open(Pointer_stringify($0));}, url.c_str());
    }
}
