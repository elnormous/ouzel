// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXNATIVETARGET_HPP
#define OUZEL_XCODE_PBXNATIVETARGET_HPP

#include <vector>
#include "PbxTarget.hpp"
#include "PbxFileReference.hpp"
#include "XcConfigurationList.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PbxNativeTarget final: public PbxTarget
        {
        public:
            PbxNativeTarget(const std::string& n,
                            const XcConfigurationList& buildConfigurationList,
                            const std::vector<Id>& buildPhases,
                            const PbxFileReference& productReference):
                name{n},
                buildConfigurationListId{buildConfigurationList.getId()},
                buildPhaseIds{buildPhases},
                productReferenceId{productReference.getId()} {}

            std::string getIsa() const override { return "PBXNativeTarget"; }

            plist::Value encode() const override
            {
                auto result = PbxTarget::encode();
                result["buildConfigurationList"] = toString(buildConfigurationListId);
                result["buildPhases"] = plist::Value::Array{};
                result["buildRules"] = plist::Value::Array{};
                result["dependencies"] = plist::Value::Array{};
                result["name"] = name;
                result["productName"] = name;
                result["productReference"] = toString(productReferenceId);
                result["productType"] = "com.apple.product-type.application";

                for (auto buildPhaseId : buildPhaseIds)
                    result["buildPhases"].pushBack(toString(buildPhaseId));

                return result;
            }

        private:
            std::string name;
            Id buildConfigurationListId;
            std::vector<Id> buildPhaseIds;
            Id productReferenceId;
        };
    }
}

#endif // OUZEL_XCODE_PBXNATIVETARGET_HPP
