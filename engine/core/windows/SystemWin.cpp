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
        std::unique_ptr<LPWSTR, decltype(&LocalFree)> argv(CommandLineToArgvW(GetCommandLineW(), &argc), LocalFree);

        ouzel::core::windows::System system(argc, argv.get());

        ouzel::core::windows::Engine engine(argc, argv.get());
        engine.run();
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
    System::System(int argc, LPWSTR* argv)
    {
    }
}
