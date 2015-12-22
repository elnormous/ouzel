// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include "CompileConfig.h"
#include "Noncopyable.h"
#include "ReferenceCounted.h"

namespace ouzel
{
    class FileSystem: public Noncopyable, public ReferenceCounted
    {
    public:
		static const std::string DIRECTORY_SEPARATOR;

        FileSystem();
        virtual ~FileSystem();
        
        std::string getPath(const std::string& filename);
        
        void addResourcePath(const std::string& path);
        
    protected:
        bool fileExists(const std::string& filename);
        
        std::vector<std::string> _resourcePaths;
    };
}
