// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include <cstdlib>
#include "EngineWin.hpp"
#include "utils/Log.hpp"

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    try
    {
        int argc;
        LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

        std::unique_ptr<ouzel::EngineWin> engine(new ouzel::EngineWin(argc, argv));

        if (argv) LocalFree(argv);

        engine->run();
        engine.reset(); // must release engine instance before exit on Windows
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        ouzel::Log(ouzel::Log::Level::ERR) << e.what();
        return EXIT_FAILURE;
    }
    catch (...)
    {
        return EXIT_FAILURE;
    }
}
