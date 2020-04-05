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
            PbxProject(const std::string& org,
                       const XcConfigurationList& buildConfigurationList,
                       const PbxGroup& mainGroup,
                       const PbxGroup& productRefGroup,
                       const std::map<std::string, Id>& references,
                       const std::vector<Id>& targets):
                organization{org},
                buildConfigurationListId{buildConfigurationList.getId()},
                mainGroupId(mainGroup.getId()),
                productRefGroupId(productRefGroup.getId()),
                projectReferences{references},
                targetIds{targets} {}

            std::string getIsa() const override { return "PBXProject"; }

            plist::Value encode() const override
            {
                auto result = PbxObject::encode();

                result["attributes"] = plist::Value::Dictionary{
                    {"LastUpgradeCheck", "0800"},
                    {"ORGANIZATIONNAME", organization}
                };
                result["buildConfigurationList"] = toString(buildConfigurationListId);
                result["compatibilityVersion"] = "Xcode 9.3";
                result["developmentRegion"] = "en";
                result["hasScannedForEncodings"] = 0;
                result["knownRegions"] = plist::Value::Array{"en", "Base"};
                result["mainGroup"] = toString(mainGroupId);
                result["productRefGroup"] = toString(productRefGroupId);
                result["projectDirPath"] = "";
                result["projectRoot"] = "";
                result["targets"] = plist::Value::Array{};

                if (!projectReferences.empty())
                {
                    auto references = plist::Value::Dictionary{};

                    for (const auto& projectReference : projectReferences)
                        references[projectReference.first] = toString(projectReference.second);

                    result["projectReferences"] = plist::Value::Array{references};
                }

                for (auto targetId : targetIds)
                    result["targets"].pushBack(toString(targetId));

                return result;
            }

        private:
            std::string organization;
            Id buildConfigurationListId;
            Id mainGroupId;
            Id productRefGroupId;
            std::map<std::string, Id> projectReferences;
            std::vector<Id> targetIds;
        };
    }
}

#endif // OUZEL_XCODE_PBXPROJECT_HPP
