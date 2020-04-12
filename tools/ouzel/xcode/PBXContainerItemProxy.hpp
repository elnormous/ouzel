// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXCONTAINERITEMPROXY_HPP
#define OUZEL_XCODE_PBXCONTAINERITEMPROXY_HPP

#include "PBXObject.hpp"
#include "PBXFileReference.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PBXContainerItemProxy final: public PBXObject
        {
        public:
            enum ProxyType
            {
                NativeTarget = 1,
                Reference = 2
            };

            PBXContainerItemProxy() = default;

            std::string getIsa() const override { return "PBXContainerItemProxy"; }

            plist::Value encode() const override
            {
                auto result = PBXObject::encode();
                if (containerPortal) result["containerPortal"] = toString(containerPortal->getId());
                result["proxyType"] = static_cast<uint32_t>(proxyType);
                if (remoteGlobal) result["remoteGlobalIDString"] = toString(remoteGlobal->getId());
                result["remoteInfo"] = remoteInfo;
                return result;
            }

            const PBXObject* containerPortal = nullptr;
            ProxyType proxyType = NativeTarget;
            const PBXObject* remoteGlobal = nullptr;
            std::string remoteInfo;
        };
    }
}

#endif // OUZEL_XCODE_PBXCONTAINERITEMPROXY_HPP
