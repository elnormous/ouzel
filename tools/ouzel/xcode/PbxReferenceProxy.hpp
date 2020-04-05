// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXREFERENCEPROXY_HPP
#define OUZEL_XCODE_PBXREFERENCEPROXY_HPP

#include "PbxObject.hpp"
#include "PbxSourceTree.hpp"
#include "storage/Path.hpp"
#include "utils/Plist.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PbxReferenceProxy final: public PbxObject
        {
        public:
            PbxReferenceProxy(const std::string& type,
                              const storage::Path& p,
                              const Id& remoteRef,
                              PbxSourceTree tree):
                PbxObject{"PBXReferenceProxy"},
                fileType{type},
                path{p},
                remoteRefId{remoteRef},
                sourceTree{tree} {}

            plist::Value getValue() const
            {
                return plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"fileType", fileType},
                    {"path", std::string{path}},
                    {"remoteRef", toString(remoteRefId)},
                    {"sourceTree", toString(sourceTree)}
                };
            }

        private:
            std::string fileType;
            storage::Path path;
            const Id& remoteRefId;
            PbxSourceTree sourceTree;
        };
    }
}

#endif // OUZEL_XCODE_PBXREFERENCEPROXY_HPP
