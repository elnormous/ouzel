// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXPROJECT_HPP
#define OUZEL_XCODE_PBXPROJECT_HPP

#include <map>
#include <vector>
#include "PBXObject.hpp"
#include "PBXGroup.hpp"
#include "XCConfigurationList.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PBXProject final: public PBXObject
        {
        public:
            PBXProject(const std::string& initOrganization,
                       const XCConfigurationList& initBuildConfigurationList,
                       const PBXGroup& initMainGroup,
                       const PBXGroup& initProductRefGroup,
                       const std::map<std::string, PBXObjectRef>& initProjectReferences,
                       const std::vector<PBXTargetRef>& initTargets):
                organization{initOrganization},
                buildConfigurationList{initBuildConfigurationList},
                mainGroup(initMainGroup),
                productRefGroup(initProductRefGroup),
                projectReferences{initProjectReferences},
                targets{initTargets} {}

            std::string getIsa() const override { return "PBXProject"; }

            plist::Value encode() const override
            {
                auto result = PBXObject::encode();

                result["attributes"] = plist::Value::Dictionary{
                    {"LastUpgradeCheck", "0800"},
                    {"ORGANIZATIONNAME", organization}
                };
                result["buildConfigurationList"] = toString(buildConfigurationList.getId());
                result["compatibilityVersion"] = "Xcode 9.3";
                result["developmentRegion"] = "en";
                result["hasScannedForEncodings"] = 0;
                result["knownRegions"] = plist::Value::Array{"en", "Base"};
                result["mainGroup"] = toString(mainGroup.getId());
                result["productRefGroup"] = toString(productRefGroup.getId());
                result["projectDirPath"] = "";
                result["projectRoot"] = "";
                result["targets"] = plist::Value::Array{};

                if (!projectReferences.empty())
                {
                    auto references = plist::Value::Dictionary{};

                    for (const auto& projectReference : projectReferences)
                    {
                        const PBXObject& object = projectReference.second;
                        references[projectReference.first] = toString(object.getId());
                    }

                    result["projectReferences"] = plist::Value::Array{references};
                }

                for (const PBXTarget& target : targets)
                    result["targets"].pushBack(toString(target.getId()));

                return result;
            }

        private:
            std::string organization;
            const XCConfigurationList& buildConfigurationList;
            const PBXGroup& mainGroup;
            const PBXGroup& productRefGroup;
            std::map<std::string, PBXObjectRef> projectReferences;
            std::vector<PBXTargetRef> targets;
        };
    }
}

#endif // OUZEL_XCODE_PBXPROJECT_HPP
