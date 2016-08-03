// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ApplicationIOS.h"

int main(int argc, char* argv[])
{
    ouzel::ApplicationIOS application(argc, argv);

    return application.run() ? 0 : 1;
}
