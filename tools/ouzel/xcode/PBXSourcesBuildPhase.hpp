// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXSOURCESBUILDPHASE_HPP
#define OUZEL_XCODE_PBXSOURCESBUILDPHASE_HPP

#include <vector>
#include "PBXBuildPhase.hpp"
#include "PBXBuildFile.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PBXSourcesBuildPhase final: public PBXBuildPhase
        {
        public:
            PBXSourcesBuildPhase(const std::vector<PBXBuildFileRef>& initFiles):
                files{initFiles} {}

            std::string getIsa() const override { return "PBXSourcesBuildPhase"; }

            plist::Value encode() const override
            {
                auto result = PBXBuildPhase::encode();
                result["files"] = plist::Value::Array{};
                for (const PBXBuildFile& file : files)
                    result["files"].pushBack(toString(file.getId()));

                return result;
            }

        private:
            std::vector<PBXBuildFileRef> files;
        };
    }
}

#endif // OUZEL_XCODE_PBXSOURCESBUILDPHASE_HPP
