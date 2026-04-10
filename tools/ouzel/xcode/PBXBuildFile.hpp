#ifndef FLUXION_XCODE_PBXBUILDFILE_HPP
#define FLUXION_XCODE_PBXBUILDFILE_HPP

#include "PBXObject.hpp"
#include "PBXFileReference.hpp"

namespace fluxion::xcode
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

#endif // FLUXION_XCODE_PBXBUILDFILE_HPP
