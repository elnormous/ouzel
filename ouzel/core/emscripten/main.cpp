// Copyright 2015-2018 Elviss Strazdins.
// This file is part of the Ouzel engine.

#include <cstdlib>
#include "EngineEm.hpp"
#include "utils/Log.hpp"

int main(int argc, char* argv[])
{
    try
    {
        ouzel::EngineEm engine(argc, argv);
        engine.run();
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
