// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXSOURCEBUILDPHASE_HPP
#define OUZEL_XCODE_PBXSOURCEBUILDPHASE_HPP

#include <vector>
#include "PbxBuildPhase.hpp"
#include "utils/Plist.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PbxSourcesBuildPhase final: public PbxBuildPhase
        {
        public:
            PbxSourcesBuildPhase(const std::vector<Id>& files):
                PbxBuildPhase{"PBXSourcesBuildPhase"},
                fileIds{files} {}

            plist::Value getValue() const
            {
                auto result = plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"buildActionMask", 2147483647},
                    {"files", plist::Value::Array{}},
                    {"runOnlyForDeploymentPostprocessing", 0},
                };

                for (auto fileId : fileIds)
                    result["files"].pushBack(toString(fileId));

                return result;
            }

        private:
            std::vector<Id> fileIds;
        };
    }
}

#endif // OUZEL_XCODE_PBXSOURCEBUILDPHASE_HPP
