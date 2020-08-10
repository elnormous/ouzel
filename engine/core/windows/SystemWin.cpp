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
    namespace
    {
        std::vector<std::string> parseArgs(int argc, LPWSTR* argv)
        {
            std::vector<std::string> result;
            if (argv)
            {
                int bufferSize;
                std::vector<char> buffer;

                for (int i = 0; i < argc; ++i)
                {
                    bufferSize = WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, nullptr, 0, nullptr, nullptr);
                    if (bufferSize == 0)
                        throw std::system_error(GetLastError(), std::system_category(), "Failed to convert wide char to UTF-8");

                    buffer.resize(bufferSize);
                    if (WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, buffer.data(), bufferSize, nullptr, nullptr) == 0)
                        throw std::system_error(GetLastError(), std::system_category(), "Failed to convert wide char to UTF-8");

                    result.push_back(buffer.data());
                }
            }
            return result;
        }
    }

    System::System(int argc, LPWSTR* argv):
        core::System(parseArgs(argc, argv))
    {
    }
}
