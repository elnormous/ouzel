// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_MAKEFILEPROJECT_HPP
#define OUZEL_MAKEFILEPROJECT_HPP

#include "OuzelProject.hpp"

namespace ouzel
{
    class MakefileProject final
    {
    public:
        MakefileProject(const OuzelProject& p):
            project(p)
        {
        }

    private:
        const OuzelProject& project;
    };
}

#endif // OUZEL_MAKEFILEPROJECT_HPP
