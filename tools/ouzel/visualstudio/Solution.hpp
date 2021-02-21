// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_VISUALSTUDIO_SOLUTION_HPP
#define OUZEL_VISUALSTUDIO_SOLUTION_HPP

#include <string>
#include <vector>
#include "VcxProject.hpp"

namespace ouzel::visualstudio
{
    class Solution final
    {
    public:
        explicit Solution(const std::vector<std::reference_wrapper<const VcxProject>>& p):
            projects(p) {}

        std::string encode() const
        {
            std::string result = "Microsoft Visual Studio Solution File, Format Version 12.00\n"
                "# Visual Studio Version 16\n"
                "VisualStudioVersion = 16.0.29509.3\n"
                "MinimumVisualStudioVersion = 10.0.40219.1\n";

            for (const VcxProject& project : projects)
            {
                result += "Project(\"{" + toString(project.type) + "}\") = \"" + project.name + "\", \"" + std::string(project.path) + "\", \"{" + toString(project.guid) + "}\"\n";

                if (!project.dependencies.empty())
                {
                    result += "\tProjectSection(ProjectDependencies) = postProject\n";

                    for (const auto& dependency : project.dependencies)
                        result += "\t\t{" + toString(dependency) + "} = {" + toString(dependency) + "}\n";

                    result += "\tEndProjectSection\n";
                }

                result += "EndProject\n";
            }

            result += "Gloabl\n"
                "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution\n";

            const std::vector<Configuration> configurations = {
                {"Debug", "x64", "x64", Configuration::Type::Debug},
                {"Debug", "Win32", "x86", Configuration::Type::Debug},
                {"Release", "x64", "x64", Configuration::Type::Release},
                {"Release", "Win32", "x86", Configuration::Type::Release}
            };

            for (const auto& configuration : configurations)
                result += "\t\t" + configuration.name + "|" + configuration.architecture + " = " + configuration.name + "|" + configuration.architecture + "\n";

            result += "\tEndGlobalSection\n"
                "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution\n";

            for (const VcxProject& project : projects)
                for (const auto& configuration : project.configurations)
                {
                    result += "\t\t{" + toString(project.guid) + "}." + configuration.name + "|" + configuration.architecture + ".ActiveCfg = " + configuration.name + "|" + configuration.platform + "\n";
                    result += "\t\t{" + toString(project.guid) + "}." + configuration.name + "|" + configuration.architecture + ".Build.0 = " + configuration.name + "|" + configuration.platform + "\n";
                }

            result += "\tEndGlobalSection\n"
                "\tGlobalSection(SolutionProperties) = preSolution\n"
                "\t\tHideSolutionNode = FALSE\n"
                "\tEndGlobalSection\n"
                "EndGlobal\n";

            return result;
        }

        std::vector<std::reference_wrapper<const VcxProject>> projects;
    };
}

#endif // OUZEL_VISUALSTUDIO_SOLUTION_HPP
