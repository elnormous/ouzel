#ifndef FLUXION_XCODE_PBXTARGETDEPENDENCY_HPP
#define FLUXION_XCODE_PBXTARGETDEPENDENCY_HPP

#include <string>
#include "PBXObject.hpp"
#include "PBXContainerItemProxy.hpp"
#include "PBXTarget.hpp"

namespace fluxion::xcode
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

#endif // FLUXION_XCODE_PBXTARGETDEPENDENCY_HPP
