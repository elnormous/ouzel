// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXNATIVETARGET_HPP
#define OUZEL_XCODE_PBXNATIVETARGET_HPP

#include <vector>
#include "PbxTarget.hpp"
#include "PbxFileReference.hpp"
#include "PbxBuildPhase.hpp"
#include "XcConfigurationList.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PbxNativeTarget final: public PbxTarget
        {
        public:
            PbxNativeTarget(const std::string& initName,
                            const XcConfigurationList& initBuildConfigurationList,
                            const std::vector<PbxBuildPhaseRef>& initBuildPhases,
                            const PbxFileReference& initProductReference):
                name{initName},
                buildConfigurationList{initBuildConfigurationList},
                buildPhases{initBuildPhases},
                productReference{initProductReference} {}

            std::string getIsa() const override { return "PBXNativeTarget"; }

            plist::Value encode() const override
            {
                auto result = PbxTarget::encode();
                result["buildConfigurationList"] = toString(buildConfigurationList.getId());
                result["buildPhases"] = plist::Value::Array{};
                result["buildRules"] = plist::Value::Array{};
                result["dependencies"] = plist::Value::Array{};
                result["name"] = name;
                result["productName"] = name;
                result["productReference"] = toString(productReference.getId());
                result["productType"] = "com.apple.product-type.application";

                for (const PbxBuildPhase& buildPhase : buildPhases)
                    result["buildPhases"].pushBack(toString(buildPhase.getId()));

                return result;
            }

        private:
            std::string name;
            const XcConfigurationList& buildConfigurationList;
            std::vector<PbxBuildPhaseRef> buildPhases;
            const PbxFileReference& productReference;
        };
    }
}

#endif // OUZEL_XCODE_PBXNATIVETARGET_HPP
