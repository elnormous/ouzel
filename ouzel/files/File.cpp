// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "File.hpp"

namespace ouzel
{
    File::File()
    {
    }

    File::File(const std::string& filename, int mode)
    {
        open(filename, mode);
    }

    File::~File()
    {

    }

    File::File(File&& other)
    {

    }

    File& File::operator=(File&& other)
    {
        return *this;
    }

    bool File::open(const std::string& filename, int mode)
    {
        return true;
    }

    bool File::close()
    {
        return true;
    }

    bool File::read(char* buffer, uint32_t& size)
    {
        return true;
    }

    bool File::write(const char* buffer, uint32_t& size)
    {
        return true;
    }

    bool File::seek(uint32_t position, int mode)
    {
        return true;
    }
}
