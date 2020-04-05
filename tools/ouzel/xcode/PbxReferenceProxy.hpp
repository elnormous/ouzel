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
            PbxReferenceProxy(const std::string n,
                              const storage::Path& p,
                              const std::string& type,
                              PbxSourceTree tree,
                              const PbxContainerItemProxy& remoteRef):
                PbxFileReference{n, p, type, tree},
                remoteRefId{remoteRef.getId()}
                {}

            std::string getIsa() const override { return "PBXReferenceProxy"; }

            plist::Value encode() const override
            {
                auto result = PbxFileReference::encode();
                result["remoteRef"] = toString(remoteRefId);
                return result;
            }

        private:
            const Id& remoteRefId;
        };
    }
}

#endif // OUZEL_XCODE_PBXREFERENCEPROXY_HPP
