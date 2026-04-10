#ifndef FLUXION_XCODE_PBXBUILDPHASE_HPP
#define FLUXION_XCODE_PBXBUILDPHASE_HPP

#include "PBXObject.hpp"

namespace fluxion::xcode
{
    class PBXBuildPhase: public PBXObject
    {
    public:
        PBXBuildPhase() = default;

        std::string getIsa() const override { return "PBXBuildPhase"; }

        plist::Value encode() const override
        {
            auto result = PBXObject::encode();
            result["buildActionMask"] = 2147483647;
            result["runOnlyForDeploymentPostprocessing"] = 0;
            return result;
        }
    };
}

#endif // FLUXION_XCODE_PBXBUILDPHASE_HPP
