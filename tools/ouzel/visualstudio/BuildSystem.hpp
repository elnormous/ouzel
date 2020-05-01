// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_VISUALSTUDIO_BUILDSYSTEM_HPP
#define OUZEL_VISUALSTUDIO_BUILDSYSTEM_HPP

#include "Solution.hpp"
#include "VcxProject.hpp"
#include "VcxProjectFilters.hpp"
#include "storage/Path.hpp"

namespace ouzel
{
    namespace visualstudio
    {
        inline void generateBuildFiles(const Project& project)
        {

            const Uuid libouzelUuid = {0xC60AB6A6, 0x67FF, 0x4704, 0xBD, 0xCD, 0xDE, 0x2F, 0x38, 0x2F, 0xE2, 0x51};
            const Uuid ouzelUuid = {0x8BC9CEB8, 0x8B4A, 0x11D0, 0x8D, 0x11, 0x00, 0xA0, 0xC9, 0x1B, 0xC9, 0x42};

            const auto libouzelProject = VcxProject(libouzelUuid, "libouzel", storage::Path{"libouzel.vxxproj"}, std::vector<Uuid>{});
            const auto ouzelProject = VcxProject(ouzelUuid, "ouzel", storage::Path{"ouzel.vxxproj"},  std::vector<Uuid>{});

            const storage::Path projectFilename = project.getPath().getFilename();
            const storage::Path projectDirectory = project.getPath().getDirectory();

            std::vector<VcxProject> platformProjects;

            for (const auto& target : project.getTargets())
                if (target.platform == Platform::Windows)
                {
                    const auto vcxprojProjectPath = projectDirectory / target.name + ".vcxproj";

                    auto vcxProject = VcxProject{project.getName(), vcxprojProjectPath, {libouzelUuid}};
                    VcxProjectFilters vcxProjectFilters;

                    std::ofstream vcxprojProjectFile(vcxprojProjectPath, std::ios::trunc);
                    vcxprojProjectFile << vcxProject.encode();

                    const auto filtersProjectPath = projectDirectory / target.name + ".vcxproj.filters";
                    std::ofstream vcxProjectFiltersFile(filtersProjectPath, std::ios::trunc);
                    vcxProjectFiltersFile << vcxProjectFilters.encode();

                    platformProjects.push_back(vcxProject);
                }

            std::vector<std::reference_wrapper<const VcxProject>> projects;
            projects.push_back(libouzelProject);
            projects.push_back(ouzelProject);

            for (const auto& project : platformProjects)
                projects.push_back(project);

            const auto solution = Solution{projects};
            const auto solutionPath = projectDirectory / project.getName() + ".sln";
            std::ofstream solutionFile(solutionPath, std::ios::trunc);
            solutionFile << solution.encode();
        }
    }
}

#endif // OUZEL_VISUALSTUDIO_BUILDSYSTEM_HPP
