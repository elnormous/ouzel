// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include "TextureOGL.h"
#include "Utils.h"

namespace ouzel
{
    TextureOGL::TextureOGL(const std::string& filename, Renderer* renderer):
        Texture(filename, renderer)
    {
        std::string path = std::string("file://") + getResourcePath(filename);
        
        CFStringRef pathStringRef = CFStringCreateWithCString(kCFAllocatorDefault, path.c_str(), kCFStringEncodingASCII);
        
        CFURLRef url = CFURLCreateWithString(kCFAllocatorDefault, pathStringRef, nullptr);
        
        CGImageSourceRef imageSourceRef = CGImageSourceCreateWithURL(url, NULL);
        CGImageRef imageRef = CGImageSourceCreateImageAtIndex (imageSourceRef, 0, NULL);
        
        uint32_t width = static_cast<uint32_t>(CGImageGetWidth(imageRef));
        uint32_t height = static_cast<uint32_t>(CGImageGetHeight(imageRef));
        CGRect rect = {{0, 0}, {static_cast<CGFloat>(width), static_cast<CGFloat>(height)}};
        void* data = calloc(width * 4, height);
        CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB();
        CGContextRef bitmapContext = CGBitmapContextCreate (data,
                                                            width, height, 8,
                                                            width * 4, space,
                                                            kCGBitmapByteOrder32Host |
                                                            kCGImageAlphaPremultipliedFirst);
        CGContextSetBlendMode(bitmapContext, kCGBlendModeCopy);
        CGContextDrawImage(bitmapContext, rect, imageRef);
        CGContextRelease(bitmapContext);
        CFRelease(imageSourceRef);
        CFRelease(imageRef);
        
        CFRelease(pathStringRef);
        CFRelease(url);
        
        glGenTextures(1, &_textureId);
        
        glBindTexture(GL_TEXTURE_2D, _textureId);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height,
                     0, GL_BGRA, GL_UNSIGNED_BYTE, data);
        
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        
        glGenerateMipmap(GL_TEXTURE_2D);
        
        checkOpenGLErrors();
        
        free(data);
        
        _size.width = static_cast<float>(width);
        _size.height = static_cast<float>(height);
        
        printf("texture: %s (%d), width: %d, height: %d\n", filename.c_str(), _textureId, width, height);
    }
    
    TextureOGL::~TextureOGL()
    {
        if (_textureId)
        {
            glDeleteTextures(1, &_textureId);
        }
    }
}
