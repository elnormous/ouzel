// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXCONTAINERITEMPROXY_HPP
#define OUZEL_XCODE_PBXCONTAINERITEMPROXY_HPP

#include "PbxObject.hpp"
#include "PbxFileReference.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PbxContainerItemProxy final: public PbxObject
        {
        public:
            PbxContainerItemProxy(const PbxFileReference& containerPortal,
                                  const Id& remote,
                                  const std::string& info):
                containerPortalId{containerPortal.getId()},
                remoteGlobalId{remote},
                remoteInfo{info} {}

            std::string getIsa() const override { return "PBXContainerItemProxy"; }

            plist::Value encode() const override
            {
                auto result = PbxObject::encode();
                result["containerPortal"] = toString(containerPortalId);
                result["proxyType"] = 2;
                result["remoteGlobalIDString"] = toString(remoteGlobalId);
                result["remoteInfo"] = remoteInfo;
                return result;
            }

        private:
            Id containerPortalId;
            Id remoteGlobalId;
            std::string remoteInfo;
        };
    }
}

#endif // OUZEL_XCODE_PBXCONTAINERITEMPROXY_HPP
