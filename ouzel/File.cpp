// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.


#include "File.h"
#include "Engine.h"
#include "FileSystem.h"

namespace ouzel
{
    File::File(const std::string& filename, Mode mode, bool binary):
        _mode(mode), _binary(binary)
    {
        _path = FileSystem::getInstance()->getPath(filename);
        
        open();
    }
    
    File::File(const File& other):
        _path(other._path), _mode(other._mode), _binary(other._binary)
    {
        if (_file) fclose(_file);
        
        open();
    }
    
    File::File(File&& other):
        _path(other._path), _mode(other._mode), _binary(other._binary), _file(other._file)
    {
        other._file = nullptr;
    }
    
    File::~File()
    {
        if (_file) fclose(_file);
    }
    
    File& File::operator = (const File& other)
    {
        _path = other._path;
        _mode = other._mode;
        _binary = other._binary;
        
        open();
        
        return *this;
    }
    
    File& File::operator = (File&& other)
    {
        _path = other._path;
        _mode = other._mode;
        _binary = other._binary;
        _file = other._file;
        
        other._file = nullptr;
        
        return *this;
    }
    
    void File::open()
    {
        std::string modeStr;
        
        switch (_mode)
        {
            case Mode::READ:
                modeStr = "r";
                break;
            case Mode::WRITE:
                modeStr = "w";
                break;
            case Mode::APPEND:
                modeStr = "a";
                break;
        }
        
        if (_binary)
        {
            modeStr += "b";
        }
        
        _file = fopen(_path.c_str(), modeStr.c_str());
    }
    
    int64_t File::read(char* buffer, uint32_t size)
    {
        return static_cast<int64_t>(fread(buffer, size, 1, _file));
    }
    
    int64_t File::write(const char* buffer, uint32_t size)
    {
        return static_cast<int64_t>(fwrite(buffer, size, 1, _file));
    }
}
