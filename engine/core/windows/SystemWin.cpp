// Ouzel by Elviss Strazdins

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
        using LocalFreeFunction = HLOCAL(WINAPI*)(HLOCAL);
        std::unique_ptr<LPWSTR, LocalFreeFunction> argv{
            CommandLineToArgvW(GetCommandLineW(), &argc),
            &LocalFree
        };

        ouzel::core::windows::System system{argc, argv.get()};

        ouzel::core::windows::Engine engine{argc, argv.get()};
        engine.run();
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        ouzel::log(ouzel::Log::Level::error) << e.what();
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
                for (int i = 0; i < argc; ++i)
                {
                    const auto bufferSize = WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, nullptr, 0, nullptr, nullptr);
                    if (bufferSize == 0)
                        throw std::system_error(GetLastError(), std::system_category(), "Failed to convert wide char to UTF-8");

                    auto buffer = std::make_unique<char[]>(bufferSize);
                    if (WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, buffer.get(), bufferSize, nullptr, nullptr) == 0)
                        throw std::system_error(GetLastError(), std::system_category(), "Failed to convert wide char to UTF-8");

                    result.push_back(buffer.get());
                }

            return result;
        }
    }

    System::System(int argc, LPWSTR* argv):
        core::System{parseArgs(argc, argv)}
    {
    }
}
