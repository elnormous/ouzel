// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "files/FileSystem.hpp"

namespace ouzel
{
    class Engine;

    class FileSystemEm: public FileSystem
    {
        friend Engine;
    public:

    protected:
        FileSystemEm();
    };
}
