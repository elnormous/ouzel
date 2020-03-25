// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODEPROJECT_HPP
#define OUZEL_XCODEPROJECT_HPP

#include <fstream>
#include "OuzelProject.hpp"
#include "storage/FileSystem.hpp"

namespace ouzel
{
    class XcodeProject final
    {
    public:
        XcodeProject(const OuzelProject& p):
            project(p)
        {
        }

        void generate()
        {
            auto projectDirectory = project.getDirectoryPath() / storage::Path{project.getName() + ".xcodeproj"};
            auto projectDirectoryType = projectDirectory.getType();

            if (projectDirectoryType == storage::Path::Type::NotFound)
            {
                storage::FileSystem::createDirectory(projectDirectory);
            }
            else if (projectDirectoryType != storage::Path::Type::Directory)
            {
                storage::FileSystem::deleteFile(projectDirectory);
                storage::FileSystem::createDirectory(projectDirectory);
            }

            auto projectFile = projectDirectory / storage::Path{"project.pbxproj"};

            std::ofstream file(projectFile, std::ios::trunc);
            file << "// !$*UTF8*$!\n"
            "{\n";

            file << "}\n";
        }

    private:
        const OuzelProject& project;
    };
}

#endif // OUZEL_XCODEPROJECT_HPP
