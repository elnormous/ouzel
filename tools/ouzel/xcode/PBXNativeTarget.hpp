// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXNATIVETARGET_HPP
#define OUZEL_XCODE_PBXNATIVETARGET_HPP

#include <vector>
#include "PBXTarget.hpp"
#include "PBXFileReference.hpp"
#include "PBXBuildPhase.hpp"
#include "XCConfigurationList.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PBXNativeTarget final: public PBXTarget
        {
        public:
            PBXNativeTarget(const std::string& initName,
                            const XCConfigurationList& initBuildConfigurationList,
                            const std::vector<PbxBuildPhaseRef>& initBuildPhases,
                            const PBXFileReference& initProductReference):
                name{initName},
                buildConfigurationList{initBuildConfigurationList},
                buildPhases{initBuildPhases},
                productReference{initProductReference} {}

            std::string getIsa() const override { return "PBXNativeTarget"; }

            plist::Value encode() const override
            {
                auto result = PBXTarget::encode();
                result["buildConfigurationList"] = toString(buildConfigurationList.getId());
                result["buildPhases"] = plist::Value::Array{};
                result["buildRules"] = plist::Value::Array{};
                result["dependencies"] = plist::Value::Array{};
                result["name"] = name;
                result["productName"] = name;
                result["productReference"] = toString(productReference.getId());
                result["productType"] = "com.apple.product-type.application";

                for (const PBXBuildPhase& buildPhase : buildPhases)
                    result["buildPhases"].pushBack(toString(buildPhase.getId()));

                return result;
            }

        private:
            std::string name;
            const XCConfigurationList& buildConfigurationList;
            std::vector<PbxBuildPhaseRef> buildPhases;
            const PBXFileReference& productReference;
        };
    }
}

#endif // OUZEL_XCODE_PBXNATIVETARGET_HPP
