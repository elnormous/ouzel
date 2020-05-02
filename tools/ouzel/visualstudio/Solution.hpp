// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_VISUALSTUDIO_SOLUTION_HPP
#define OUZEL_VISUALSTUDIO_SOLUTION_HPP

#include <map>
#include <string>
#include <vector>
#include "VcxProject.hpp"

namespace ouzel
{
    namespace visualstudio
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

                const std::map<std::string, std::string> configurations = {
                    {"Debug|x64", "Debug|x64"},
                    {"Debug|x86", "Debug|Win32"},
                    {"Release|x64", "Release|x64"},
                    {"Release|x86", "Release|Win32"}
                };

                result += "Gloabl\n"
                    "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution\n";
                    
                for (const auto& configuration : configurations)
                    result += "\t\t" + configuration.first + " = " + configuration.first + "\n";

                result += "\tEndGlobalSection\n"
                    "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution\n";

                for (const VcxProject& project : projects)
                    for (const auto& configuration : configurations)
                    {
                        result += "\t\t{" + toString(project.guid) + "}." + configuration.first + ".ActiveCfg = " + configuration.second + "\n";
                        result += "\t\t{" + toString(project.guid) + "}." + configuration.first + ".Build.0 = " + configuration.second + "\n";
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
}

#endif // OUZEL_VISUALSTUDIO_SOLUTION_HPP
