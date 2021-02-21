// Ouzel by Elviss Strazdins

#include <cstdlib>
#include "SystemLinux.hpp"
#include "EngineLinux.hpp"
#include "../../utils/Log.hpp"

int main(int argc, char* argv[])
{
    try
    {
        ouzel::core::linux::System system(argc, argv);

        ouzel::core::linux::Engine engine(argc, argv);
        engine.run();
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        ouzel::logger.log(ouzel::Log::Level::error) << e.what();
        return EXIT_FAILURE;
    }
}

namespace ouzel::core::linux
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
        core::System(parseArgs(argc, argv))
    {
    }
}
