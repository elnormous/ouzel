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
            }

            void generate() {}

        private:
            const OuzelProject& project;
        };
    }
}

#endif // OUZEL_VISUALSTUDIO_PROJECT_HPP
