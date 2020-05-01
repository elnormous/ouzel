// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_VISUALSTUDIO_BUILDSYSTEM_HPP
#define OUZEL_VISUALSTUDIO_BUILDSYSTEM_HPP

#include "Solution.hpp"
#include "VcxProject.hpp"
#include "VcxProjectFilters.hpp"

namespace ouzel
{
    namespace visualstudio
    {
        inline void generateBuildFiles(const Project& project)
        {
            const storage::Path projectFilename = project.getPath().getFilename();
            const storage::Path projectDirectory = project.getPath().getDirectory();

            for (const auto& target : project.getTargets())
                if (target.platform == Platform::Windows)
                {
                    VcxProject vcxProject;
                    VcxProjectFilters vcxProjectFilters;

                    const auto vcxprojProjectPath = projectDirectory / target.name + ".vcxproj";
                    std::ofstream vcxprojProjectFile(vcxprojProjectPath, std::ios::trunc);
                    vcxprojProjectFile << vcxProject.encode();

                    const auto filtersProjectPath = projectDirectory / target.name + ".vcxproj.filters";
                    std::ofstream vcxProjectFiltersFile(filtersProjectPath, std::ios::trunc);
                    vcxProjectFiltersFile << vcxProjectFilters.encode();
                }

            auto solution = Solution{};
            auto slnProjectPath = projectDirectory / project.getName() + ".sln";
            std::ofstream slnProjectFile(slnProjectPath, std::ios::trunc);
            slnProjectFile << solution.encode();
        }
    }
}

#endif // OUZEL_VISUALSTUDIO_BUILDSYSTEM_HPP
