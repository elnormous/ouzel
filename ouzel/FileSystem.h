// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <vector>
#include "Noncopyable.h"
#include "ReferenceCounted.h"

namespace ouzel
{
    class FileSystem: public Noncopyable, public ReferenceCounted
    {
    public:
		static const std::string DIRECTORY_SEPARATOR;

        static FileSystem* getInstance();
        
        FileSystem();
        virtual ~FileSystem();
        
        std::string getPath(const std::string& filename) const;
        
        void addResourcePath(const std::string& path);
        
        std::string getExtension(const std::string& path) const;
        
    protected:
        bool fileExists(const std::string& filename) const;
        
        std::vector<std::string> _resourcePaths;
    };
}
