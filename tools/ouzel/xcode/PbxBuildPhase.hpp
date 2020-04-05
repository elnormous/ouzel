// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXBUILDPHASE_HPP
#define OUZEL_XCODE_PBXBUILDPHASE_HPP

#include "PbxObject.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PbxBuildPhase: public PbxObject
        {
        public:
            PbxBuildPhase(const std::string& i): PbxObject{i} {}
        };
    }
}

#endif // OUZEL_XCODE_PBXBUILDPHASE_HPP
