// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_VISUALSTUDIO_BUILDSYSTEM_HPP
#define OUZEL_VISUALSTUDIO_BUILDSYSTEM_HPP

#include "Project.hpp"

namespace ouzel
{
    namespace visualstudio
    {
        inline void generateBuildFiles(const Project& project)
        {
            const storage::Path projectFilename = project.getPath().getFilename();
            const storage::Path projectDirectory = project.getPath().getDirectory();

            auto vcxprojProjectPath = projectDirectory / storage::Path{project.getName() + ".vcxproj"};
            auto filtersProjectPath = projectDirectory / storage::Path{project.getName() + ".vcxproj.filters"};
            auto slnProjectPath = projectDirectory / storage::Path{project.getName() + ".sln"};

            std::ofstream vcxprojProjectFile(vcxprojProjectPath, std::ios::trunc);
            std::ofstream filtersProjectFile(filtersProjectPath, std::ios::trunc);
            std::ofstream slnProjectFile(slnProjectPath, std::ios::trunc);
        }
    }
}

#endif // OUZEL_VISUALSTUDIO_BUILDSYSTEM_HPP
