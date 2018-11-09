// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <cstdlib>
#include <emscripten.h>
#include "EngineEm.hpp"
#include "audio/AudioDevice.hpp"
#include "graphics/RenderDevice.hpp"
#include "input/emscripten/InputSystemEm.hpp"
#include "utils/Log.hpp"

static void loop(void* arg)
{
    reinterpret_cast<ouzel::EngineEm*>(arg)->step();
}

namespace ouzel
{
    EngineEm::EngineEm(int argc, char* argv[])
    {
        for (int i = 0; i < argc; ++i)
            args.push_back(argv[i]);
    }

    void EngineEm::run()
    {
        init();
        start();

        emscripten_set_main_loop_arg(loop, this, 0, 1);
    }

    void EngineEm::step()
    {
        input::InputSystemEm* inputEm = static_cast<input::InputSystemEm*>(inputManager->getInputSystem());

        if (active)
        {
            try
            {
                update();
            }
            catch (const std::exception& e)
            {
                engine->log(Log::Level::ERR) << e.what();
                exit();
            }

            try
            {
                audio->update();
            }
            catch (const std::exception& e)
            {
                engine->log(Log::Level::ERR) << e.what();
            }

            try
            {
                renderer->getDevice()->process();
            }
            catch (const std::exception& e)
            {
                engine->log(Log::Level::ERR) << e.what();
            }

            try
            {
                audio->getDevice()->process();
            }
            catch (const std::exception& e)
            {
                engine->log(Log::Level::ERR) << e.what();
            }

            try
            {
                inputEm->update();
            }
            catch (const std::exception& e)
            {
                engine->log(Log::Level::ERR) << e.what();
            }
        }
        else
            emscripten_cancel_main_loop();
    }

    void EngineEm::executeOnMainThread(const std::function<void()>& func)
    {
        if (func) func();
    }

    void EngineEm::openURL(const std::string& url)
    {
        EM_ASM_ARGS({window.open(Pointer_stringify($0));}, url.c_str());
    }
}
