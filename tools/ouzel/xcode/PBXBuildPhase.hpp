// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXBUILDPHASE_HPP
#define OUZEL_XCODE_PBXBUILDPHASE_HPP

#include "PBXObject.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PBXBuildPhase: public PBXObject
        {
        public:
            PBXBuildPhase() = default;

            std::string getIsa() const override { return "PBXBuildPhase"; }
        };

        using PbxBuildPhaseRef = std::reference_wrapper<const PBXBuildPhase>;
    }
}

#endif // OUZEL_XCODE_PBXBUILDPHASE_HPP
