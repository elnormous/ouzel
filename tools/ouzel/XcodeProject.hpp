// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODEPROJECT_HPP
#define OUZEL_XCODEPROJECT_HPP

#include "OuzelProject.hpp"

namespace ouzel
{
    class XcodeProject final
    {
    public:
        XcodeProject(const OuzelProject& p):
            project(p)
        {
        }

    private:
        const OuzelProject& project;
    };
}

#endif // OUZEL_XCODEPROJECT_HPP
