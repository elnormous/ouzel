// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXBUILDFILE_HPP
#define OUZEL_XCODE_PBXBUILDFILE_HPP

#include "PbxObject.hpp"
#include "PbxFileReference.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PbxBuildFile final: public PbxObject
        {
        public:
            PbxBuildFile(const PbxFileReference& initFileRef):
                fileRef{initFileRef} {}

            std::string getIsa() const override { return "PBXBuildFile"; }

            plist::Value encode() const override
            {
                auto result = PbxObject::encode();
                result["fileRef"] = toString(fileRef.getId());
                return result;
            }

        private:
            const PbxFileReference& fileRef;
        };

        using PbxBuildFileRef = std::reference_wrapper<const PbxBuildFile>;
    }
}

#endif // OUZEL_XCODE_PBXBUILDFILE_HPP
