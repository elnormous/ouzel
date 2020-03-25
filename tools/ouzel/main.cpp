// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#include <cstdlib>
#include <iostream>
#include <set>
#include <stdexcept>
#include "storage/Path.hpp"
#include "OuzelProject.hpp"
#include "MakefileProject.hpp"
#include "VisualStudioProject.hpp"
#include "XcodeProject.hpp"

enum class ProjectType
{
    Makefile,
    VisualStudio,
    Xcode
};

template <class T> void generateProject(const ouzel::OuzelProject& project)
{
    T output(project);
    output.generate();
}

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

        for (int i = 1; i < argc; ++i)
        {
            if (std::string(argv[i]) == "--help")
            {
                std::cout << "Usage:\n";
                std::cout << argv[0] << " [--help] [--generate-project <project-file>]"
                    " [--project <all|makefile|visualstudio|xcode>]"
                    " [--export-assets <project-file>]\n";
                return EXIT_SUCCESS;
            }
            else if (std::string(argv[i]) == "--generate-project")
            {
                action = Action::GenerateProject;

                if (++i >= argc)
                    throw std::runtime_error("Invalid command");

                projectPath = std::string(argv[i]);
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

                projectPath = std::string(argv[i]);
            }
        }

        switch (action)
        {
            case Action::None:
                throw std::runtime_error("No action selected");
            case Action::GenerateProject:
            {
                ouzel::OuzelProject project(projectPath);

                for (auto projectType : projectTypes)
                    switch (projectType)
                    {
                        case ProjectType::Makefile:
                            generateProject<ouzel::MakefileProject>(project);
                            break;
                        case ProjectType::VisualStudio:
                            generateProject<ouzel::VisualStudioProject>(project);
                            break;
                        case ProjectType::Xcode:
                            generateProject<ouzel::XcodeProject>(project);
                            break;
                    }
                break;
            }
            case Action::ExportAssets:
            {
                ouzel::OuzelProject project(projectPath);
                project.exportAssets();
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
