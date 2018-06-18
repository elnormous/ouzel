// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdlib>
#include <emscripten.h>
#include "EngineEm.hpp"
#include "audio/AudioDevice.hpp"
#include "graphics/RenderDevice.hpp"
#include "input/emscripten/InputManagerEm.hpp"
#include "utils/Utils.hpp"

static void loop(void* arg)
{
    if (!reinterpret_cast<ouzel::EngineEm*>(arg)->step())
        emscripten_cancel_main_loop();
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
        if (!init())
            return;

        start();

        emscripten_set_main_loop_arg(loop, this, 0, 1);
    }

    bool EngineEm::step()
    {
        update();
        audio->update();

        if (!active ||
            !renderer->getDevice()->process())
            return false;

        // TODO: check for result of the AudioDevice::process
        audio->getDevice()->process();

        input::InputManagerEm* inputEm = static_cast<input::InputManagerEm*>(inputManager.get());
        inputEm->update();

        return active;
    }

    void EngineEm::executeOnMainThread(const std::function<void(void)>& func)
    {
        if (func) func();
    }

    bool EngineEm::openURL(const std::string& url)
    {
        EM_ASM_ARGS({window.open(Pointer_stringify($0));}, url.c_str());

        return true;
    }
}
