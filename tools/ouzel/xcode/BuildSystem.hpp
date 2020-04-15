// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_BUILDSYSTEM_HPP
#define OUZEL_XCODE_BUILDSYSTEM_HPP

#include "XcodeProject.hpp"

namespace ouzel
{
    namespace xcode
    {
        inline void generateBuildFiles(const ouzel::Project& project)
        {
            Project p(project);
            const storage::Path projectDirectory = project.getPath().getDirectory();
            p.save(projectDirectory / storage::Path{project.getName() + ".xcodeproj"});
        }
    }
}

#endif // OUZEL_XCODE_BUILDSYSTEM_HPP
