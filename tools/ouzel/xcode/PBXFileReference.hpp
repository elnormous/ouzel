// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXFILEREFERENCE_HPP
#define OUZEL_XCODE_PBXFILEREFERENCE_HPP

#include "PBXFileElement.hpp"
#include "PBXFileType.hpp"
#include "PBXSourceTree.hpp"
#include "storage/Path.hpp"

namespace ouzel
{
    namespace xcode
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
                result["path"] = std::string(path);
                result["sourceTree"] = toString(sourceTree);
                if (!name.empty()) result["name"] = name;
                return result;
            }

            std::string name;
            storage::Path path;
            PBXFileType fileType;
            PBXSourceTree sourceTree;
        };
    }
}

#endif // OUZEL_XCODE_PBXFILEREFERENCE_HPP
