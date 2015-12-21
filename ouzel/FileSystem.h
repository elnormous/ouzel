// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include "Noncopyable.h"
#include "ReferenceCounted.h"

namespace ouzel
{
    class FileSystem: public Noncopyable, public ReferenceCounted
    {
    public:
        FileSystem();
        virtual ~FileSystem();
        
        std::string getResourcePath(const std::string& filename);
    };
}
