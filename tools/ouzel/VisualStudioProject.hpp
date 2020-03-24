// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_VISUALSTUDIOPROJECT_HPP
#define OUZEL_VISUALSTUDIOPROJECT_HPP

#include "OuzelProject.hpp"

namespace ouzel
{
    class VisualStudioProject final
    {
    public:
        VisualStudioProject(const OuzelProject& p):
            project(p)
        {
        }

    private:
        const OuzelProject& project;
    };
}

#endif // OUZEL_VISUALSTUDIOPROJECT_HPP
