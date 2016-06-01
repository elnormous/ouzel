// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "core/Engine.h"
#include "utils/Utils.h"

using namespace ouzel;

int main(int argc, char* argv[])
{
    ouzel::setArgs(argc, argv);

    ouzelMain(getArgs());

    if (!ouzel::sharedEngine)
    {
        return 0;
    }

    ouzel::sharedEngine->begin();

    for (;;)
    {
        if (!ouzel::sharedEngine->run())
        {
            break;
        }
    }

    ouzel::sharedEngine->end();

    return 0;
}
