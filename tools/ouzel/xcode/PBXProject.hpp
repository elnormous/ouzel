// Ouzel by Elviss Strazdins

#ifndef OUZEL_XCODE_PBXPROJECT_HPP
#define OUZEL_XCODE_PBXPROJECT_HPP

#include <map>
#include <string>
#include <vector>
#include "PBXObject.hpp"
#include "PBXGroup.hpp"
#include "XCConfigurationList.hpp"

namespace ouzel::xcode
{
    class PBXProject final: public PBXObject
    {
    public:
        PBXProject() = default;

        std::string getIsa() const override { return "PBXProject"; }

        plist::Value encode() const override
        {
            auto result = PBXObject::encode();

            result["attributes"] = plist::Value::Dictionary{
                {"LastUpgradeCheck", "0800"},
                {"ORGANIZATIONNAME", organization}
            };
            if (buildConfigurationList)
                result["buildConfigurationList"] = toString(buildConfigurationList->getId());
            result["compatibilityVersion"] = "Xcode 10.0";
            result["developmentRegion"] = "en";
            result["hasScannedForEncodings"] = 0;
            result["knownRegions"] = plist::Value::Array{"en", "Base"};
            if (mainGroup) result["mainGroup"] = toString(mainGroup->getId());
            if (productRefGroup)
                result["productRefGroup"] = toString(productRefGroup->getId());
            result["projectDirPath"] = "";
            result["projectRoot"] = "";
            result["targets"] = plist::Value::Array{};
            for (const auto target : targets)
                if (target) result["targets"].pushBack(toString(target->getId()));

            if (!projectReferences.empty())
            {
                result["projectReferences"] = plist::Value::Array{};

                for (const auto& projectReference : projectReferences)
                {
                    plist::Value::Dictionary reference;

                    for (const auto& [name, object] : projectReference)
                        if (object) reference[name] = toString(object->getId());

                    result["projectReferences"].pushBack(reference);
                }
            }

            return result;
        }

        std::string organization;
        const XCConfigurationList* buildConfigurationList = nullptr;
        const PBXGroup* mainGroup = nullptr;
        const PBXGroup* productRefGroup = nullptr;
        std::vector<std::map<std::string, const PBXObject*>> projectReferences;
        std::vector<const PBXTarget*> targets;
    };
}

#endif // OUZEL_XCODE_PBXPROJECT_HPP
