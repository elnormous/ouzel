// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "EngineLinux.hpp"

int main(int argc, char* argv[])
{
    ouzel::EngineLinux engine(argc, argv);

    return engine.run();
}
