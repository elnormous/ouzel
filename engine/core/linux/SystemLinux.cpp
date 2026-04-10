#include <cstdlib>
#include "SystemLinux.hpp"
#include "EngineLinux.hpp"
#include "../../utils/Log.hpp"

int main(int argc, char* argv[])
{
    try
    {
        fluxion::core::linux::System system{argc, argv};
        system.run();
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        fluxion::log(fluxion::Log::Level::error) << e.what();
        return EXIT_FAILURE;
    }
}

namespace fluxion::core::linux
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
    }

    System::System(int argc, char* argv[]):
        core::System{parseArgs(argc, argv)},
        engine{getArgs()}
    {
    }

    void System::run()
    {
        engine.run();
    }
}
