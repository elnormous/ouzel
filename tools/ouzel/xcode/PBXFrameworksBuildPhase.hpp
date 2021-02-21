// Ouzel by Elviss Strazdins

#ifndef OUZEL_XCODE_PBXFRAMEWORKSBUILDPHASE_HPP
#define OUZEL_XCODE_PBXFRAMEWORKSBUILDPHASE_HPP

#include <vector>
#include "PBXBuildPhase.hpp"
#include "PBXBuildFile.hpp"

namespace ouzel::xcode
{
    class PBXFrameworksBuildPhase final: public PBXBuildPhase
    {
    public:
        PBXFrameworksBuildPhase() = default;

        std::string getIsa() const override { return "PBXFrameworksBuildPhase"; }

        plist::Value encode() const override
        {
            auto result = PBXBuildPhase::encode();
            result["files"] = plist::Value::Array{};
            for (const auto file : files)
                if (file) result["files"].pushBack(toString(file->getId()));

            return result;
        }

        std::vector<const PBXBuildFile*> files;
    };
}

#endif // OUZEL_XCODE_PBXFRAMEWORKSBUILDPHASE_HPP
