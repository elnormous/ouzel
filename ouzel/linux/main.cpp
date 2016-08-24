// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ApplicationLinux.h"

int main(int argc, char* argv[])
{
    ouzel::ApplicationLinux application(argc, argv);

    return application.run();
}
