// Ouzel by Elviss Strazdins

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
    makefile,
    visualStudio,
    xcode
};

int main(int argc, const char* argv[])
{
    try
    {
        enum class Action
        {
            none,
            generateProject,
            exportAssets
        };

        Action action = Action::none;
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
                action = Action::generateProject;

                if (++i >= argc)
                    throw std::runtime_error("Invalid command");

                projectPath = ouzel::storage::Path{argv[i], ouzel::storage::Path::Format::native};
            }
            else if (std::string(argv[i]) == "--project")
            {
                if (++i >= argc)
                    throw std::runtime_error("Invalid command");

                if (std::string(argv[i]) == "all")
                {
                    projectTypes.insert(ProjectType::makefile);
                    projectTypes.insert(ProjectType::visualStudio);
                    projectTypes.insert(ProjectType::xcode);
                }
                else if (std::string(argv[i]) == "makefile")
                    projectTypes.insert(ProjectType::makefile);
                else if (std::string(argv[i]) == "visualstudio")
                    projectTypes.insert(ProjectType::visualStudio);
                else if (std::string(argv[i]) == "xcode")
                    projectTypes.insert(ProjectType::xcode);
                else
                    throw std::runtime_error("Invalid project");
            }
            else if (std::string(argv[i]) == "--export-assets")
            {
                action = Action::exportAssets;

                if (++i >= argc)
                    throw std::runtime_error("Invalid command");

                projectPath = ouzel::storage::Path{argv[i], ouzel::storage::Path::Format::native};
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
            case Action::none:
                throw std::runtime_error("No action selected");
            case Action::generateProject:
            {
                ouzel::Project project{projectPath};

                for (const auto projectType : projectTypes)
                    switch (projectType)
                    {
                        case ProjectType::makefile:
                            ouzel::makefile::generateBuildFiles(project);
                            break;
                        case ProjectType::visualStudio:
                            ouzel::visualstudio::generateBuildFiles(project);
                            break;
                        case ProjectType::xcode:
                            ouzel::xcode::generateBuildFiles(project);
                            break;
                    }
                break;
            }
            case Action::exportAssets:
            {
                ouzel::Project project{projectPath};
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
