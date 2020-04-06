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

            PBXContainerItemProxy(const PBXFileReference& initContainerPortal,
                                  ProxyType initProxyType,
                                  const Id& initRemoteGlobalId,
                                  const std::string& initRemoteInfo):
                containerPortal{initContainerPortal},
                proxyType{initProxyType},
                remoteGlobalId{initRemoteGlobalId},
                remoteInfo{initRemoteInfo} {}

            std::string getIsa() const override { return "PBXContainerItemProxy"; }

            plist::Value encode() const override
            {
                auto result = PBXObject::encode();
                result["containerPortal"] = toString(containerPortal.getId());
                result["proxyType"] = static_cast<uint32_t>(proxyType);
                result["remoteGlobalIDString"] = toString(remoteGlobalId);
                result["remoteInfo"] = remoteInfo;
                return result;
            }

        private:
            const PBXFileReference& containerPortal;
            ProxyType proxyType;
            Id remoteGlobalId;
            std::string remoteInfo;
        };
    }
}

#endif // OUZEL_XCODE_PBXCONTAINERITEMPROXY_HPP
