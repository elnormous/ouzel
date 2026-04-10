#ifndef FLUXION_XCODE_PBXRESOURCESBUILDPHASE_HPP
#define FLUXION_XCODE_PBXRESOURCESBUILDPHASE_HPP

#include <vector>
#include "PBXBuildPhase.hpp"
#include "PBXBuildFile.hpp"

namespace fluxion::xcode
{
    class PBXResourcesBuildPhase final: public PBXBuildPhase
    {
    public:
        PBXResourcesBuildPhase() = default;

        std::string getIsa() const override { return "PBXResourcesBuildPhase"; }

        plist::Value encode() const override
        {
            auto result = PBXBuildPhase::encode();
            result["files"] = plist::Array{};
            for (const auto file : files)
                if (file) result["files"].pushBack(toString(file->getId()));

            return result;
        }

        std::vector<const PBXBuildFile*> files;
    };
}

#endif // FLUXION_XCODE_PBXRESOURCESBUILDPHASE_HPP
