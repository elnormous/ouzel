// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <cstdlib>
#include <iostream>
#include <set>
#include <stdexcept>
#include "storage/Path.hpp"
#include "Project.hpp"
#include "makefile/BuildSystem.hpp"
#include "visualstudio/BuildSystem.hpp"
#include "xcode/BuildSystem.hpp"

enum class ProjectType
{
    Makefile,
    VisualStudio,
    Xcode
};

int main(int argc, const char* argv[])
{
    try
    {
        enum class Action
        {
            None,
            GenerateProject,
            ExportAssets
        };

        Action action = Action::None;
        ouzel::storage::Path projectPath;
        std::set<ProjectType> projectTypes;
        std::string targetName;

        for (int i = 1; i < argc; ++i)
        {
            if (std::string(argv[i]) == "--help")
            {
                std::cout << "Usage:\n";
                std::cout << argv[0] << " [--help] [--generate-project <project-file>]"
                    " [--project <all|makefile|visualstudio|xcode>]"
                    " [--export-assets <project-file>]\n"
                    " [--target <target-name>]\n";
                return EXIT_SUCCESS;
            }
            else if (std::string(argv[i]) == "--generate-project")
            {
                action = Action::GenerateProject;

                if (++i >= argc)
                    throw std::runtime_error("Invalid command");

                projectPath = ouzel::storage::Path{argv[i], ouzel::storage::Path::Format::Native};
            }
            else if (std::string(argv[i]) == "--project")
            {
                if (++i >= argc)
                    throw std::runtime_error("Invalid command");

                if (std::string(argv[i]) == "all")
                {
                    projectTypes.insert(ProjectType::Makefile);
                    projectTypes.insert(ProjectType::VisualStudio);
                    projectTypes.insert(ProjectType::Xcode);
                }
                else if (std::string(argv[i]) == "makefile")
                    projectTypes.insert(ProjectType::Makefile);
                else if (std::string(argv[i]) == "visualstudio")
                    projectTypes.insert(ProjectType::VisualStudio);
                else if (std::string(argv[i]) == "xcode")
                    projectTypes.insert(ProjectType::Xcode);
                else
                    throw std::runtime_error("Invalid project");
            }
            else if (std::string(argv[i]) == "--export-assets")
            {
                action = Action::ExportAssets;

                if (++i >= argc)
                    throw std::runtime_error("Invalid command");

                projectPath = ouzel::storage::Path{argv[i], ouzel::storage::Path::Format::Native};
            }
            else if (std::string(argv[i]) == "--target")
            {
                if (++i >= argc)
                    throw std::runtime_error("Invalid command");

                targetName = argv[i];
            }
        }

        switch (action)
        {
            case Action::None:
                throw std::runtime_error("No action selected");
            case Action::GenerateProject:
            {
                ouzel::Project project(projectPath);

                for (auto projectType : projectTypes)
                    switch (projectType)
                    {
                        case ProjectType::Makefile:
                            ouzel::makefile::generateBuildFiles(project);
                            break;
                        case ProjectType::VisualStudio:
                            ouzel::visualstudio::generateBuildFiles(project);
                            break;
                        case ProjectType::Xcode:
                            ouzel::xcode::generateBuildFiles(project);
                            break;
                    }
                break;
            }
            case Action::ExportAssets:
            {
                ouzel::Project project(projectPath);
                project.exportAssets(targetName);
                break;
            }
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return EXIT_SUCCESS;
}
