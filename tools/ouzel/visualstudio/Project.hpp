// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_VISUALSTUDIO_PROJECT_HPP
#define OUZEL_VISUALSTUDIO_PROJECT_HPP

#include "OuzelProject.hpp"

namespace ouzel
{
    namespace visualstudio
    {
        class Project final
        {
        public:
            Project(const OuzelProject& p):
                project(p)
            {
                const storage::Path projectFilename = project.getPath().getFilename();
                const storage::Path projectDirectory = project.getPath().getDirectory();

                auto vcxprojProjectPath = projectDirectory / storage::Path{project.getName() + ".vcxproj"};
                auto filtersProjectPath = projectDirectory / storage::Path{project.getName() + ".vcxproj.filters"};
                auto slnProjectPath = projectDirectory / storage::Path{project.getName() + ".sln"};

                std::ofstream vcxprojProjectFile(vcxprojProjectPath, std::ios::trunc);
                std::ofstream filtersProjectFile(filtersProjectPath, std::ios::trunc);
                std::ofstream slnProjectFile(slnProjectPath, std::ios::trunc);
            }

            void generate() {}

        private:
            const OuzelProject& project;
        };
    }
}

#endif // OUZEL_VISUALSTUDIO_PROJECT_HPP
