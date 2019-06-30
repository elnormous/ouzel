// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <cstdlib>
#include <shellapi.h>
#include "EngineWin.hpp"
#include "utils/Log.hpp"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
        int argc;
        LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

        std::unique_ptr<ouzel::EngineWin> engine = std::make_unique<ouzel::EngineWin>(argc, argv);

        if (argv) LocalFree(argv);

        engine->run();
        engine.reset(); // must release engine instance before exit on Windows
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        ouzel::engine->log(ouzel::Log::Level::ERR) << e.what();
        return EXIT_FAILURE;
    }
}
