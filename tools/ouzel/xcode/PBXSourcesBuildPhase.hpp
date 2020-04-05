// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXSOURCEBUILDPHASE_HPP
#define OUZEL_XCODE_PBXSOURCEBUILDPHASE_HPP

#include <vector>
#include "PBXBuildPhase.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PBXSourcesBuildPhase final: public PBXBuildPhase
        {
        public:
            PBXSourcesBuildPhase(const std::vector<PbxBuildFileRef>& initFiles):
                files{initFiles} {}

            std::string getIsa() const override { return "PBXSourcesBuildPhase"; }

            plist::Value encode() const override
            {
                auto result = PBXBuildPhase::encode();
                result["buildActionMask"] = 2147483647;
                result["files"] = plist::Value::Array{};
                result["runOnlyForDeploymentPostprocessing"] = 0;

                for (const PBXBuildFile& file : files)
                    result["files"].pushBack(toString(file.getId()));

                return result;
            }

        private:
            std::vector<PbxBuildFileRef> files;
        };
    }
}

#endif // OUZEL_XCODE_PBXSOURCEBUILDPHASE_HPP
