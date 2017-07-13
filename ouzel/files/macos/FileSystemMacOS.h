// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "files/FileSystem.h"

#if defined(__OBJC__)
#include <Foundation/Foundation.h>
typedef NSFileManager* NSFileManagerPtr;
#else
#include <objc/objc.h>
typedef id NSFileManagerPtr;
#endif


namespace ouzel
{
    class Engine;

    class FileSystemMacOS: public FileSystem
    {
        friend Engine;
    public:
        virtual std::string getStorageDirectory(bool user = true) const override;
        virtual std::string getTempDirectory() const override;

    protected:
        FileSystemMacOS();

        NSFileManagerPtr fileManager = Nil;
    };
}
