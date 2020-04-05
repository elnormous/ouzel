// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXSOURCEBUILDPHASE_HPP
#define OUZEL_XCODE_PBXSOURCEBUILDPHASE_HPP

#include <vector>
#include "PbxBuildPhase.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PbxSourcesBuildPhase final: public PbxBuildPhase
        {
        public:
            PbxSourcesBuildPhase(const std::vector<PbxBuildFileRef>& initFiles):
                files{initFiles} {}

            std::string getIsa() const override { return "PBXSourcesBuildPhase"; }

            plist::Value encode() const override
            {
                auto result = PbxBuildPhase::encode();
                result["buildActionMask"] = 2147483647;
                result["files"] = plist::Value::Array{};
                result["runOnlyForDeploymentPostprocessing"] = 0;

                for (const PbxBuildFile& file : files)
                    result["files"].pushBack(toString(file.getId()));

                return result;
            }

        private:
            std::vector<PbxBuildFileRef> files;
        };
    }
}

#endif // OUZEL_XCODE_PBXSOURCEBUILDPHASE_HPP
