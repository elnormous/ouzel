// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXCONTAINERITEMPROXY_HPP
#define OUZEL_XCODE_PBXCONTAINERITEMPROXY_HPP

#include "PbxObject.hpp"
#include "utils/Plist.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PbxContainerItemProxy final: public PbxObject
        {
        public:
            PbxContainerItemProxy(const Id& containerPortal,
                                  const Id& remote,
                                  const std::string& info):
                PbxObject{"PBXContainerItemProxy"},
                containerPortalId{containerPortal},
                remoteGlobalId{remote},
                remoteInfo{info} {}

            plist::Value getValue() const
            {
                return plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"containerPortal", toString(containerPortalId)},
                    {"proxyType", 2},
                    {"remoteGlobalIDString", toString(remoteGlobalId)},
                    {"remoteInfo", remoteInfo}
                };
            }

        private:
            Id containerPortalId;
            Id remoteGlobalId;
            std::string remoteInfo;
        };
    }
}

#endif // OUZEL_XCODE_PBXCONTAINERITEMPROXY_HPP
