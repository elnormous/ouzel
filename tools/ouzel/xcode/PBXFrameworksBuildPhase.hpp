// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXFRAMEWORKSBUILDPHASE_HPP
#define OUZEL_XCODE_PBXFRAMEWORKSBUILDPHASE_HPP

#include <vector>
#include "PBXBuildPhase.hpp"
#include "PBXBuildFile.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PBXFrameworksBuildPhase final: public PBXBuildPhase
        {
        public:
            PBXFrameworksBuildPhase(const std::vector<PBXBuildFileRef>& initFiles):
                files{initFiles} {}

            std::string getIsa() const override { return "PBXFrameworksBuildPhase"; }

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

#endif // OUZEL_XCODE_PBXFRAMEWORKSBUILDPHASE_HPP
