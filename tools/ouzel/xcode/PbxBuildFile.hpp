// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXBUILDFILE_HPP
#define OUZEL_XCODE_PBXBUILDFILE_HPP

#include "PbxObject.hpp"
#include "utils/Plist.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PbxBuildFile final: public PbxObject
        {
        public:
            PbxBuildFile(const Id& fileRef):
                PbxObject{"PBXBuildFile"}, fileRefId{fileRef} {}

            plist::Value getValue() const
            {
                return plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"fileRef", toString(fileRefId)}
                };
            }

        private:
            Id fileRefId;
        };
    }
}

#endif // OUZEL_XCODE_PBXBUILDFILE_HPP
