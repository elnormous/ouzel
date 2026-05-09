#ifndef FLUXION_XCODE_BUILDSYSTEM_HPP
#define FLUXION_XCODE_BUILDSYSTEM_HPP

#include "XcodeProject.hpp"

namespace fluxion::xcode
{
    inline void generateBuildFiles(const fluxion::Project& project)
    {
        Project p{project};
        const auto projectDirectory = project.getPath().getDirectory();
        p.save(projectDirectory / storage::Path{project.getName() + ".xcodeproj"});
    }
}

#endif // FLUXION_XCODE_BUILDSYSTEM_HPP
