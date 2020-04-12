// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXLEGACYTARGET_HPP
#define OUZEL_XCODE_PBXLEGACYTARGET_HPP

#include <vector>
#include "PBXTarget.hpp"
#include "PBXFileReference.hpp"
#include "PBXBuildPhase.hpp"
#include "PBXTargetDependency.hpp"
#include "XCConfigurationList.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PBXLegacyTarget final: public PBXTarget
        {
        public:
            PBXLegacyTarget(const std::string& initName,
                            const std::string& initBuildToolPath,
                            const std::string& initBuildArgumentsString,
                            const XCConfigurationList& initBuildConfigurationList,
                            const std::vector<PBXBuildPhaseRef>& initBuildPhases,
                            const std::vector<PBXTargetDependencyRef>& initDependencies):
                name{initName},
                buildToolPath{initBuildToolPath},
                buildArgumentsString{initBuildArgumentsString},
                buildConfigurationList{initBuildConfigurationList},
                buildPhases{initBuildPhases},
                dependencies{initDependencies} {}

            std::string getIsa() const override { return "PBXNativeTarget"; }

            plist::Value encode() const override
            {
                auto result = PBXTarget::encode();
                result["buildConfigurationList"] = toString(buildConfigurationList.getId());
                result["buildPhases"] = plist::Value::Array{};
                for (const PBXBuildPhase& buildPhase : buildPhases)
                    result["buildPhases"].pushBack(toString(buildPhase.getId()));

                result["dependencies"] = plist::Value::Array{};
                for (const PBXTargetDependency& dependency : dependencies)
                    result["dependencies"].pushBack(toString(dependency.getId()));

                result["name"] = name;
                result["passBuildSettingsInEnvironment"] = 1;
                result["productName"] = name;

                return result;
            }

        private:
            std::string name;
            std::string buildToolPath;
            std::string buildArgumentsString;
            const XCConfigurationList& buildConfigurationList;
            std::vector<PBXBuildPhaseRef> buildPhases;
            std::vector<PBXTargetDependencyRef> dependencies;
        };
    }
}

#endif // OUZEL_XCODE_PBXLEGACYTARGET_HPP
