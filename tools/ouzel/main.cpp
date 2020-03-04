// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <cstdlib>
#include <iostream>
#include <set>
#include <stdexcept>
#include "storage/File.hpp"
#include "utils/Json.hpp"
#include "OuzelProject.hpp"
#include "MakefileProject.hpp"
#include "VisualStudioProject.hpp"
#include "XcodeProject.hpp"

int main(int argc, const char* argv[])
{
    enum class Action
    {
        None,
        GenerateProject,
        ExportAssets
    };

    enum class Project
    {
        Makefile,
        VisualStudio,
        Xcode
    };

    enum class Platform
    {
        Windows,
        MacOS,
        Linux,
        IOS,
        TvOS,
        Android,
        Emscripten
    };

    Action action = Action::None;
    std::string projectPath;
    std::set<Project> projects;
    std::set<Platform> platforms;

    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]) == "--help")
        {
            std::cout << "Usage:\n";
            std::cout << argv[0] << " [--help] [--generate-project <project-file>]"
                " [--project <all|makefile|visualstudio|xcode>]"
                " [--platform <all|windows|macos|linux|ios|tvos|android|emscripten>]"
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
            if (std::string(argv[i]) == "all")
            {
                projects.insert(Project::Makefile);
                projects.insert(Project::VisualStudio);
                projects.insert(Project::Xcode);
            }
            else if (std::string(argv[i]) == "makefile")
                projects.insert(Project::Makefile);
            else if (std::string(argv[i]) == "visualstudio")
                projects.insert(Project::VisualStudio);
            else if (std::string(argv[i]) == "xcode")
                projects.insert(Project::Xcode);
            else
                throw std::runtime_error("Invalid project");
        }
        else if (std::string(argv[i]) == "--platform")
        {
            if (std::string(argv[i]) == "all")
            {
                platforms.insert(Platform::Windows);
                platforms.insert(Platform::MacOS);
                platforms.insert(Platform::Linux);
                platforms.insert(Platform::IOS);
                platforms.insert(Platform::TvOS);
                platforms.insert(Platform::Android);
                platforms.insert(Platform::Emscripten);
            }
            else if (std::string(argv[i]) == "windows")
                platforms.insert(Platform::Windows);
            else if (std::string(argv[i]) == "macos")
                platforms.insert(Platform::MacOS);
            else if (std::string(argv[i]) == "linux")
                platforms.insert(Platform::Linux);
            else if (std::string(argv[i]) == "ios")
                platforms.insert(Platform::IOS);
            else if (std::string(argv[i]) == "tvos")
                platforms.insert(Platform::TvOS);
            else if (std::string(argv[i]) == "android")
                platforms.insert(Platform::Android);
            else if (std::string(argv[i]) == "emscripten")
                platforms.insert(Platform::Emscripten);
            else
                throw std::runtime_error("Invalid platform");
        }
        else if (std::string(argv[i]) == "--export-assets")
        {
            action = Action::ExportAssets;

            if (++i >= argc)
                throw std::runtime_error("Invalid command");

            projectPath = std::string(argv[i]);
        }
    }

    try
    {
        switch (action)
        {
            case Action::None:
                throw std::runtime_error("No action selected");
            case Action::GenerateProject:
                break;
            case Action::ExportAssets:
                break;
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    ouzel::OuzelProject project(projectPath);

    return EXIT_SUCCESS;
}
