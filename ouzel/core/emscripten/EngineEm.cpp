// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <cstdlib>
#include <emscripten.h>
#include "EngineEm.hpp"
#include "audio/AudioDevice.hpp"
#include "graphics/RenderDevice.hpp"
#include "input/emscripten/InputManagerEm.hpp"
#include "utils/Utils.hpp"

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
        if (active)
        {
            update();
            audio->update();

            renderer->getDevice()->process();
            audio->getDevice()->process();

            input::InputManagerEm* inputEm = static_cast<input::InputManagerEm*>(inputManager.get());
            inputEm->update();
        }
        else
            emscripten_cancel_main_loop();
    }

    void EngineEm::executeOnMainThread(const std::function<void(void)>& func)
    {
        if (func) func();
    }

    void EngineEm::openURL(const std::string& url)
    {
        EM_ASM_ARGS({window.open(Pointer_stringify($0));}, url.c_str());
    }
}
