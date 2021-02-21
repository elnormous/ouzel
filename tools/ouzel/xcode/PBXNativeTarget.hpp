// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXNATIVETARGET_HPP
#define OUZEL_XCODE_PBXNATIVETARGET_HPP

#include <string>
#include <vector>
#include "PBXTarget.hpp"
#include "PBXFileReference.hpp"
#include "PBXBuildPhase.hpp"
#include "PBXTargetDependency.hpp"
#include "XCConfigurationList.hpp"

namespace ouzel::xcode
{
    class PBXNativeTarget final: public PBXTarget
    {
    public:
        PBXNativeTarget() = default;

        std::string getIsa() const override { return "PBXNativeTarget"; }

        plist::Value encode() const override
        {
            auto result = PBXTarget::encode();
            if (buildConfigurationList)
                result["buildConfigurationList"] = toString(buildConfigurationList->getId());
            result["buildPhases"] = plist::Value::Array{};
            for (const auto buildPhase : buildPhases)
                if (buildPhase) result["buildPhases"].pushBack(toString(buildPhase->getId()));

            result["buildRules"] = plist::Value::Array{};
            result["dependencies"] = plist::Value::Array{};
            for (const auto dependency : dependencies)
                if (dependency) result["dependencies"].pushBack(toString(dependency->getId()));

            result["name"] = name;
            result["productName"] = name;
            if (productReference)
                result["productReference"] = toString(productReference->getId());
            result["productType"] = "com.apple.product-type.application";

            return result;
        }

        std::string name;
        const XCConfigurationList* buildConfigurationList = nullptr;
        std::vector<const PBXBuildPhase*> buildPhases;
        std::vector<const PBXTargetDependency*> dependencies;
        const PBXFileReference* productReference = nullptr;
    };
}

#endif // OUZEL_XCODE_PBXNATIVETARGET_HPP
