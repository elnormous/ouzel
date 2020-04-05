// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXPROJECT_HPP
#define OUZEL_XCODE_PBXPROJECT_HPP

#include <map>
#include <vector>
#include "PbxObject.hpp"
#include "PbxGroup.hpp"
#include "XcConfigurationList.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PbxProject final: public PbxObject
        {
        public:
            PbxProject(const std::string& initOrganization,
                       const XcConfigurationList& initBuildConfigurationList,
                       const PbxGroup& initMainGroup,
                       const PbxGroup& initProductRefGroup,
                       const std::map<std::string, PbxObjectRef>& initProjectReferences,
                       const std::vector<PbxTargetRef>& initTargets):
                organization{initOrganization},
                buildConfigurationList{initBuildConfigurationList},
                mainGroup(initMainGroup),
                productRefGroup(initProductRefGroup),
                projectReferences{initProjectReferences},
                targets{initTargets} {}

            std::string getIsa() const override { return "PBXProject"; }

            plist::Value encode() const override
            {
                auto result = PbxObject::encode();

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
                        const PbxObject& object = projectReference.second;
                        references[projectReference.first] = toString(object.getId());
                    }

                    result["projectReferences"] = plist::Value::Array{references};
                }

                for (const PbxTarget& target : targets)
                    result["targets"].pushBack(toString(target.getId()));

                return result;
            }

        private:
            std::string organization;
            const XcConfigurationList& buildConfigurationList;
            const PbxGroup& mainGroup;
            const PbxGroup& productRefGroup;
            std::map<std::string, PbxObjectRef> projectReferences;
            std::vector<PbxTargetRef> targets;
        };
    }
}

#endif // OUZEL_XCODE_PBXPROJECT_HPP
