// Ouzel by Elviss Strazdins

#ifndef OUZEL_XCODE_PBXLEGACYTARGET_HPP
#define OUZEL_XCODE_PBXLEGACYTARGET_HPP

#include <string>
#include <vector>
#include "PBXTarget.hpp"
#include "PBXFileReference.hpp"
#include "PBXBuildPhase.hpp"
#include "PBXTargetDependency.hpp"
#include "XCConfigurationList.hpp"

namespace ouzel::xcode
{
    class PBXLegacyTarget final: public PBXTarget
    {
    public:
        PBXLegacyTarget() = default;

        std::string getIsa() const override { return "PBXLegacyTarget"; }

        plist::Value encode() const override
        {
            auto result = PBXTarget::encode();

            result["buildToolPath"] = buildToolPath;
            result["buildArgumentsString"] = buildArgumentsString;

            if (buildConfigurationList)
                result["buildConfigurationList"] = toString(buildConfigurationList->getId());
            result["buildPhases"] = plist::Array{};
            for (const auto buildPhase : buildPhases)
                if (buildPhase) result["buildPhases"].pushBack(toString(buildPhase->getId()));

            result["dependencies"] = plist::Array{};
            for (const auto dependency : dependencies)
                if (dependency) result["dependencies"].pushBack(toString(dependency->getId()));

            result["name"] = name;
            result["passBuildSettingsInEnvironment"] = 1;
            result["productName"] = name;

            return result;
        }

        std::string name;
        std::string buildToolPath;
        std::string buildArgumentsString;
        const XCConfigurationList* buildConfigurationList = nullptr;
        std::vector<const PBXBuildPhase*> buildPhases;
        std::vector<const PBXTargetDependency*> dependencies;
    };
}

#endif // OUZEL_XCODE_PBXLEGACYTARGET_HPP
