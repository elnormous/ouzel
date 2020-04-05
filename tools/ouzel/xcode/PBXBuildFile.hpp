// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXBUILDFILE_HPP
#define OUZEL_XCODE_PBXBUILDFILE_HPP

#include "PBXObject.hpp"
#include "PBXFileReference.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PBXBuildFile final: public PBXObject
        {
        public:
            PBXBuildFile(const PBXFileReference& initFileRef):
                fileRef{initFileRef} {}

            std::string getIsa() const override { return "PBXBuildFile"; }

            plist::Value encode() const override
            {
                auto result = PBXObject::encode();
                result["fileRef"] = toString(fileRef.getId());
                return result;
            }

        private:
            const PBXFileReference& fileRef;
        };

        using PBXBuildFileRef = std::reference_wrapper<const PBXBuildFile>;
    }
}

#endif // OUZEL_XCODE_PBXBUILDFILE_HPP
