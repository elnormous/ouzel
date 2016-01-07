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
        
        if (FILE* f = fopen(_path.c_str(), modeStr.c_str()))
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
