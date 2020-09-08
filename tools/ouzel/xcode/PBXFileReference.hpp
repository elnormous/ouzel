// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXFILEREFERENCE_HPP
#define OUZEL_XCODE_PBXFILEREFERENCE_HPP

#include "PBXFileElement.hpp"
#include "PBXFileType.hpp"

namespace ouzel::xcode
{
    class PBXFileReference: public PBXFileElement
    {
    public:
        PBXFileReference() = default;

        std::string getIsa() const override { return "PBXFileReference"; }

        plist::Value encode() const override
        {
            auto result = PBXFileElement::encode();
            result["explicitFileType"] = toString(fileType);
            result["includeInIndex"] = 0;
            return result;
        }

        PBXFileType fileType = PBXFileType::text;
    };
}

#endif // OUZEL_XCODE_PBXFILEREFERENCE_HPP
