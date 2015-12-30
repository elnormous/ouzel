// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "Image.h"
#include "Utils.h"
#include "Engine.h"
#include "FileSystem.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace ouzel
{
    Image::Image()
    {
        
    }
    
    Image::~Image()
    {
        if (_data)
        {
            stbi_image_free(_data);
        }
    }
    
    bool Image::loadFromFile(const std::string& filename)
    {
        _filename = filename;
        
        std::string path = FileSystem::getInstance()->getPath(filename);
        
        int width;
        int height;
        int comp;
        _data = stbi_load(path.c_str(), &width, &height, &comp, STBI_rgb_alpha);
        
        if (!_data)
        {
            log("Failed to open texture file %s", filename.c_str());
            return false;
        }
        
        _size.width = static_cast<float>(width);
        _size.height = static_cast<float>(height);
        
        return true;
    }
}
