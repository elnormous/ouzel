// Ouzel by Elviss Strazdins

#ifndef OUZEL_XCODE_PBXBUILDFILE_HPP
#define OUZEL_XCODE_PBXBUILDFILE_HPP

#include "PBXObject.hpp"
#include "PBXFileReference.hpp"

namespace ouzel::xcode
{
    class PBXBuildFile final: public PBXObject
    {
    public:
        PBXBuildFile() = default;

        std::string getIsa() const override { return "PBXBuildFile"; }

        plist::Value encode() const override
        {
            auto result = PBXObject::encode();
            if (fileRef) result["fileRef"] = toString(fileRef->getId());
            return result;
        }

        const PBXFileReference* fileRef = nullptr;
    };
}

#endif // OUZEL_XCODE_PBXBUILDFILE_HPP
