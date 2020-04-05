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
            PbxBuildPhase() = default;

            std::string getIsa() const override { return "PBXBuildPhase"; }
        };

        using PbxBuildPhaseRef = std::reference_wrapper<const PbxBuildPhase>;
    }
}

#endif // OUZEL_XCODE_PBXBUILDPHASE_HPP
