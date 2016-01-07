// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <string>
#include <stdio.h>
#include <cstdint>

namespace ouzel
{
    class FileSystem;
    
    class File
    {
    public:
        enum class Mode
        {
            READ,
            WRITE,
            APPEND
        };
        
        File(const std::string& filename, Mode mode, bool binary);
        File(const File& other);
        File(File&& other);
        
        virtual ~File();
        
        File& operator=(const File& other);
        File& operator=(File&& other);
        
        bool isOpen() const { return _file != nullptr; }
        FILE* getFile() const { return _file; }
        
        int64_t read(char* buffer, uint32_t size);
        int64_t write(const char* buffer, uint32_t size);
        
    protected:
        void open();
        
        std::string _path;
        Mode _mode;
        bool _binary;
        
        FILE* _file = nullptr;
    };
}
