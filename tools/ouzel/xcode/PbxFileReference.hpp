// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXFILEREFERENCE_HPP
#define OUZEL_XCODE_PBXFILEREFERENCE_HPP

#include "PbxFileElement.hpp"
#include "PbxSourceTree.hpp"
#include "storage/Path.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PbxFileReference: public PbxFileElement
        {
        public:
            PbxFileReference(const std::string n,
                             const storage::Path& p,
                             const std::string& type,
                             PbxSourceTree tree):
                name{n},
                path{p},
                fileType{type},
                sourceTree{tree} {}

            std::string getIsa() const override { return "PBXFileReference"; }

            const storage::Path& getPath() const noexcept { return path; }

            plist::Value encode() const override
            {
                auto result = PbxFileElement::encode();
                result["explicitFileType"] = fileType;
                result["includeInIndex"] = 0;
                result["path"] = std::string(path);
                result["sourceTree"] = toString(sourceTree);
                if (!name.empty()) result["name"] = name;
                return result;
            }

        private:
            std::string name;
            storage::Path path;
            std::string fileType;
            PbxSourceTree sourceTree;
        };
    }
}

#endif // OUZEL_XCODE_PBXFILEREFERENCE_HPP
