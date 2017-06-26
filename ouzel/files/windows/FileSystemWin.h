// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "files/FileSystem.h"

namespace ouzel
{
    class Engine;

    class FileSystemWin: public FileSystem
    {
        friend Engine;
    public:

    protected:
        FileSystemWin();
    };
}
