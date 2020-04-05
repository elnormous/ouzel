// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXFRAMEWORKSBUILDPHASE_HPP
#define OUZEL_XCODE_PBXFRAMEWORKSBUILDPHASE_HPP

#include <vector>
#include "PbxBuildPhase.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PbxFrameworksBuildPhase final: public PbxBuildPhase
        {
        public:
            PbxFrameworksBuildPhase(const std::vector<Id>& files):
                fileIds{files} {}

            std::string getIsa() const override { return "PBXFrameworksBuildPhase"; }

            plist::Value encode() const override
            {
                auto result = PbxBuildPhase::encode();
                result["buildActionMask"] = 2147483647;
                result["files"] = plist::Value::Array{};
                result["runOnlyForDeploymentPostprocessing"] = 0;

                for (auto fileId : fileIds)
                    result["files"].pushBack(toString(fileId));

                return result;
            }

        private:
            std::vector<Id> fileIds;
        };
    }
}

#endif // OUZEL_XCODE_PBXFRAMEWORKSBUILDPHASE_HPP
