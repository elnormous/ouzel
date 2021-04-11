// Ouzel by Elviss Strazdins

#ifndef OUZEL_XCODE_BUILDSYSTEM_HPP
#define OUZEL_XCODE_BUILDSYSTEM_HPP

#include "XcodeProject.hpp"

namespace ouzel::xcode
{
    inline void generateBuildFiles(const ouzel::Project& project)
    {
        Project p{project};
        const auto projectDirectory = project.getPath().getDirectory();
        p.save(projectDirectory / storage::Path{project.getName() + ".xcodeproj"});
    }
}

#endif // OUZEL_XCODE_BUILDSYSTEM_HPP
