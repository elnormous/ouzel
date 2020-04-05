// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXPROJECT_HPP
#define OUZEL_XCODE_PBXPROJECT_HPP

#include <map>
#include <vector>
#include "PbxObject.hpp"
#include "utils/Plist.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PbxProject final: public PbxObject
        {
        public:
            PbxProject(const std::string& org,
                       const Id& buildConfigurationList,
                       const Id& mainGroup,
                       const Id& productRefGroup,
                       const std::map<std::string, Id>& references,
                       const std::vector<Id>& targets):
                PbxObject{"PBXProject"},
                organization{org},
                buildConfigurationListId{buildConfigurationList},
                mainGroupId(mainGroup),
                productRefGroupId(productRefGroup),
                projectReferences{references},
                targetIds{targets} {}

            plist::Value getValue() const
            {
                auto result = plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"attributes", plist::Value::Dictionary{
                        {"LastUpgradeCheck", "0800"},
                        {"ORGANIZATIONNAME", organization}
                    }},
                    {"buildConfigurationList", toString(buildConfigurationListId)},
                    {"compatibilityVersion", "Xcode 9.3"},
                    {"developmentRegion", "en"},
                    {"hasScannedForEncodings", 0},
                    {"knownRegions", plist::Value::Array{"en", "Base"}},
                    {"mainGroup", toString(mainGroupId)},
                    {"productRefGroup", toString(productRefGroupId)},
                    {"projectDirPath", ""},
                    {"projectRoot", ""},
                    {"targets", plist::Value::Array{}}
                };

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
