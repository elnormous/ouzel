// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include "Noncopyable.h"

namespace ouzel
{
    class Engine;
    
    class FileSystem: public Noncopyable
    {
        friend Engine;
    public:
		static const std::string DIRECTORY_SEPARATOR;
        
        virtual ~FileSystem();
        
        std::string getHomeDirectory();
        
        std::string getPath(const std::string& filename) const;
        
        void addResourcePath(const std::string& path);
        
        std::string getExtension(const std::string& path) const;
        
        bool fileExists(const std::string& filename) const;
        
    protected:
        FileSystem();
        
        std::vector<std::string> _resourcePaths;
    };
}
