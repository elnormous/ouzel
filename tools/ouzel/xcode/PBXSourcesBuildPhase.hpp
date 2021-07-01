// Ouzel by Elviss Strazdins

#ifndef OUZEL_XCODE_PBXSOURCESBUILDPHASE_HPP
#define OUZEL_XCODE_PBXSOURCESBUILDPHASE_HPP

#include <string>
#include <vector>
#include "PBXBuildPhase.hpp"
#include "PBXBuildFile.hpp"

namespace ouzel::xcode
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

#endif // OUZEL_XCODE_PBXSOURCESBUILDPHASE_HPP
