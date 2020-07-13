// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <cstdlib>
#include <shellapi.h>
#include "SystemWin.hpp"
#include "EngineWin.hpp"
#include "../../utils/Log.hpp"

int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
    try
    {
        int argc;
        LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);

        auto engine = std::make_unique<ouzel::core::windows::Engine>(argc, argv);

        if (argv) LocalFree(argv);

        engine->run();
        engine.reset(); // must release engine instance before exit on Windows
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        ouzel::logger.log(ouzel::Log::Level::error) << e.what();
        return EXIT_FAILURE;
    }
}

namespace ouzel::core::windows
{
}
