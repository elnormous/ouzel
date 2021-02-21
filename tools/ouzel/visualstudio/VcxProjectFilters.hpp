// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_VISUALSTUDIO_VCXPROJECTFILTERS_HPP
#define OUZEL_VISUALSTUDIO_VCXPROJECTFILTERS_HPP

#include "VcxProject.hpp"

namespace ouzel::visualstudio
{
    class VcxProjectFilters final
    {
    public:
        VcxProjectFilters(const VcxProject& p): project{p} {}

        std::string encode() const
        {
            std::string result = "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                "<Project ToolsVersion=\"14.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n";

            // TODO: compile file item group
            // TODO: header file item group

            result += "</Project>";

            return result;
        }

        const VcxProject& project;
    };
}

#endif // OUZEL_VISUALSTUDIO_VCXPROJECTFILTERS_HPP
