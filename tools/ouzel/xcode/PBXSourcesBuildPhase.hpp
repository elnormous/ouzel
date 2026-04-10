#ifndef FLUXION_XCODE_PBXSOURCESBUILDPHASE_HPP
#define FLUXION_XCODE_PBXSOURCESBUILDPHASE_HPP

#include <string>
#include <vector>
#include "PBXBuildPhase.hpp"
#include "PBXBuildFile.hpp"

namespace fluxion::xcode
{
    class PBXSourcesBuildPhase final: public PBXBuildPhase
    {
    public:
        PBXSourcesBuildPhase() = default;

        std::string getIsa() const override { return "PBXSourcesBuildPhase"; }

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

#endif // FLUXION_XCODE_PBXSOURCESBUILDPHASE_HPP
