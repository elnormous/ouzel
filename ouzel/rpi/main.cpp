// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "ApplicationRPI.h"

using namespace ouzel;

int main(int argc, char* argv[])
{
    ouzel::ApplicationRPI application(argc, argv);

    return application.run() ? 0 : 1;
}
