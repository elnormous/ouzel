// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXNATIVETARGET_HPP
#define OUZEL_XCODE_PBXNATIVETARGET_HPP

#include <vector>
#include "PbxTarget.hpp"
#include "utils/Plist.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PbxNativeTarget final: public PbxTarget
        {
        public:
            PbxNativeTarget(const std::string& n,
                            const Id& buildConfigurationList,
                            const std::vector<Id>& buildPhases,
                            const Id& productReference):
                PbxTarget{"PBXNativeTarget"},
                name{n},
                buildConfigurationListId{buildConfigurationList},
                buildPhaseIds{buildPhases},
                productReferenceId{productReference} {}

            plist::Value getValue() const
            {
                auto result = plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"buildConfigurationList", toString(buildConfigurationListId)},
                    {"buildPhases", plist::Value::Array{}},
                    {"buildRules", plist::Value::Array{}},
                    {"dependencies", plist::Value::Array{}},
                    {"name", name},
                    {"productName", name},
                    {"productReference", toString(productReferenceId)},
                    {"productType", "com.apple.product-type.application"}
                };

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
