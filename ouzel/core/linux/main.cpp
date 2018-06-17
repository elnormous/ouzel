// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "EngineLinux.hpp"
#include "utils/Log.hpp"

int main(int argc, char* argv[])
{
    try
    {
        ouzel::EngineLinux engine(argc, argv);

        return engine.run();
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
