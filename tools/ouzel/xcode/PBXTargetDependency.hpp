// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXTARGETDEPENDENCY_HPP
#define OUZEL_XCODE_PBXTARGETDEPENDENCY_HPP

#include "PBXObject.hpp"
#include "PBXContainerItemProxy.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PBXTargetDependency: public PBXObject
        {
        public:
            PBXTargetDependency(const std::string& initName,
                                const PBXContainerItemProxy& initTargetProxy):
                name{initName},
                targetProxy{initTargetProxy} {}

            std::string getIsa() const override { return "PBXTargetDependency"; }

            plist::Value encode() const override
            {
                auto result = PBXObject::encode();
                result["name"] = name;
                result["targetProxy"] = toString(targetProxy.getId());

                return result;
            }

        private:
            std::string name;
            PBXContainerItemProxy targetProxy;
        };

        using PBXTargetDependencyRef = std::reference_wrapper<const PBXTargetDependency>;
    }
}

#endif // OUZEL_XCODE_PBXTARGETDEPENDENCY_HPP
