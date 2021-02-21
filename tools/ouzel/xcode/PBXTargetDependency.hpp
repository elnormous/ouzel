// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXTARGETDEPENDENCY_HPP
#define OUZEL_XCODE_PBXTARGETDEPENDENCY_HPP

#include <string>
#include "PBXObject.hpp"
#include "PBXContainerItemProxy.hpp"
#include "PBXTarget.hpp"

namespace ouzel::xcode
{
    class PBXTargetDependency: public PBXObject
    {
    public:
        PBXTargetDependency() = default;

        std::string getIsa() const override { return "PBXTargetDependency"; }

        plist::Value encode() const override
        {
            auto result = PBXObject::encode();
            result["name"] = name;
            if (targetProxy) result["targetProxy"] = toString(targetProxy->getId());
            if (target) result["target"] = toString(target->getId());

            return result;
        }

        std::string name;
        const PBXContainerItemProxy* targetProxy = nullptr;
        const PBXTarget* target = nullptr;
    };
}

#endif // OUZEL_XCODE_PBXTARGETDEPENDENCY_HPP
