// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXREFERENCEPROXY_HPP
#define OUZEL_XCODE_PBXREFERENCEPROXY_HPP

#include "PbxFileReference.hpp"
#include "PbxContainerItemProxy.hpp"
#include "PbxSourceTree.hpp"
#include "storage/Path.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PbxReferenceProxy final: public PbxFileReference
        {
        public:
            PbxReferenceProxy(const std::string initName,
                              const storage::Path& initPath,
                              const std::string& initFileType,
                              PbxSourceTree initSourceTree,
                              const PbxContainerItemProxy& initRemoteRef):
                PbxFileReference{initName, initPath, initFileType, initSourceTree},
                remoteRef{initRemoteRef} {}

            std::string getIsa() const override { return "PBXReferenceProxy"; }

            plist::Value encode() const override
            {
                auto result = PbxFileReference::encode();
                result["remoteRef"] = toString(remoteRef.getId());
                return result;
            }

        private:
            const PbxContainerItemProxy& remoteRef;
        };
    }
}

#endif // OUZEL_XCODE_PBXREFERENCEPROXY_HPP
