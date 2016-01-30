// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "File.h"
#include "Engine.h"
#include "FileSystem.h"

namespace ouzel
{
    File::File(const std::string& filename, Mode mode, bool binary)
    {
        std::string modeStr;
        
        switch (mode)
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
        
        if (binary)
        {
            modeStr += "b";
        }
        
        _file.reset(fopen(Engine::getInstance()->getFileSystem()->getPath(filename).c_str(), modeStr.c_str()), std::fclose);
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
