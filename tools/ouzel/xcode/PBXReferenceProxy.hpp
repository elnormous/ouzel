// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXREFERENCEPROXY_HPP
#define OUZEL_XCODE_PBXREFERENCEPROXY_HPP

#include "PBXFileReference.hpp"
#include "PBXContainerItemProxy.hpp"
#include "PBXSourceTree.hpp"
#include "storage/Path.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PBXReferenceProxy final: public PBXFileReference
        {
        public:
            PBXReferenceProxy(const std::string initName,
                              const storage::Path& initPath,
                              const PBXFileType& initFileType,
                              PBXSourceTree initSourceTree,
                              const PBXContainerItemProxy& initRemoteRef):
                PBXFileReference{initName, initPath, initFileType, initSourceTree},
                remoteRef{initRemoteRef} {}

            std::string getIsa() const override { return "PBXReferenceProxy"; }

            plist::Value encode() const override
            {
                auto result = PBXFileReference::encode();
                result["remoteRef"] = toString(remoteRef.getId());
                return result;
            }

        private:
            const PBXContainerItemProxy& remoteRef;
        };
    }
}

#endif // OUZEL_XCODE_PBXREFERENCEPROXY_HPP
