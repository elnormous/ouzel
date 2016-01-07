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
        _path = Engine::getInstance()->getFileSystem()->getPath(filename);
        
        open();
    }
    
    File::File(const File& other):
        _path(other._path), _mode(other._mode), _binary(other._binary)
    {
        open();
    }
    
    File::File(File&& other)
    {
        _mode = std::move(other._mode);
        _binary = std::move(other._binary);
        _path = std::move(other._path);
        _file = std::move(other._file);
    }
    
    File::~File()
    {
        
    }
    
    File& File::operator=(const File& other)
    {
        _path = other._path;
        _mode = other._mode;
        _binary = other._binary;
        _file = other._file;
        
        return *this;
    }
    
    File& File::operator=(File&& other)
    {
        _path = std::move(other._path);
        _mode = std::move(other._mode);
        _binary = std::move(other._binary);
        _file = std::move(other._file);
        
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
        
        FILE* f = fopen(_path.c_str(), modeStr.c_str());
        
        if (f)
        {
            _file.reset(f, std::fclose);
        }
    }
    
    int64_t File::read(char* buffer, uint32_t size)
    {
        return static_cast<int64_t>(fread(buffer, size, 1, _file.get()));
    }
    
    int64_t File::write(const char* buffer, uint32_t size)
    {
        return static_cast<int64_t>(fwrite(buffer, size, 1, _file.get()));
    }
}
