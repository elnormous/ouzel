// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <cstdlib>
#include "EngineMacOS.hpp"
#include "utils/Log.hpp"

int main(int argc, char* argv[])
{
    try
    {
        ouzel::EngineMacOS engine(argc, argv);
        engine.run();
        return EXIT_SUCCESS;
    }
    catch (const std::exception& e)
    {
        ouzel::engine->log(ouzel::Log::Level::ERR) << e.what();
        return EXIT_FAILURE;
    }
}
