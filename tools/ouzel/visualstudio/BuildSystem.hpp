// Ouzel by Elviss Strazdins

#ifndef OUZEL_VISUALSTUDIO_BUILDSYSTEM_HPP
#define OUZEL_VISUALSTUDIO_BUILDSYSTEM_HPP

#include "Solution.hpp"
#include "VcxProject.hpp"
#include "VcxProjectFilters.hpp"
#include "storage/Path.hpp"

namespace ouzel::visualstudio
{
    inline void generateBuildFiles(const Project& project)
    {
        const std::vector<Configuration> configurations = {
            {"Debug", "x64", "x64"},
            {"Debug", "Win32", "x86"},
            {"Release", "x64", "x64"},
            {"Release", "Win32", "x86"}
        };

        constexpr auto cppProjectGuid = Guid{0x8BC9CEB8, 0x8B4A, 0x11D0, 0x8D, 0x11, {0x00, 0xA0, 0xC9, 0x1B, 0xC9, 0x42}};

        const Guid libouzelUuid = {0xC60AB6A6, 0x67FF, 0x4704, 0xBD, 0xCD, {0xDE, 0x2F, 0x38, 0x2F, 0xE2, 0x51}};
        const Guid ouzelUuid = {0x7A4B3E2B, 0x7010, 0x4F64, 0xBE, 0x2A, {0xA3, 0xBA, 0x1E, 0x8C, 0x8C, 0x6F}};

        const auto libouzelProject = VcxProject(libouzelUuid, cppProjectGuid, "libouzel", storage::Path{"libouzel.vxxproj"}, {}, configurations);
        const auto ouzelProject = VcxProject(ouzelUuid, cppProjectGuid, "ouzel", storage::Path{"ouzel.vxxproj"}, {}, configurations);

        const storage::Path projectFilename = project.getPath().getFilename();
        const storage::Path projectDirectory = project.getPath().getDirectory();

        std::vector<VcxProject> platformProjects;

        for (const auto& target : project.getTargets())
            if (target.platform == Platform::windows)
            {
                const auto vcxprojProjectPath = projectDirectory / target.name + ".vcxproj";

                auto vcxProject = VcxProject{cppProjectGuid, project.getName(), vcxprojProjectPath, {ouzelUuid}, configurations};
                VcxProjectFilters vcxProjectFilters{vcxProject};

                std::ofstream vcxprojProjectFile(vcxprojProjectPath, std::ios::trunc);
                vcxprojProjectFile << vcxProject.encode();

                const auto filtersProjectPath = projectDirectory / target.name + ".vcxproj.filters";
                std::ofstream vcxProjectFiltersFile(filtersProjectPath, std::ios::trunc);
                vcxProjectFiltersFile << vcxProjectFilters.encode();

                platformProjects.push_back(vcxProject);
            }

        std::vector<std::reference_wrapper<const VcxProject>> projects;
        for (const auto& platformProject : platformProjects)
            projects.push_back(platformProject);

        projects.push_back(libouzelProject);
        projects.push_back(ouzelProject);

        const auto solution = Solution{projects};
        const auto solutionPath = projectDirectory / project.getName() + ".sln";
        std::ofstream solutionFile(solutionPath, std::ios::trunc);
        solutionFile << solution.encode();
    }
}

#endif // OUZEL_VISUALSTUDIO_BUILDSYSTEM_HPP
