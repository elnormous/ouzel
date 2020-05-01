// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_VISUALSTUDIO_SOLUTION_HPP
#define OUZEL_VISUALSTUDIO_SOLUTION_HPP

#include <vector>
#include "VcxProject.hpp"

namespace ouzel
{
    namespace visualstudio
    {
        class Solution final
        {
        public:
            explicit Solution(const std::vector<VcxProject>& p):
                projects(p) {}

            std::string encode() const
            {
                return std::string{};
            }

            std::vector<VcxProject> projects;
        };
    }
}

#endif // OUZEL_VISUALSTUDIO_SOLUTION_HPP
