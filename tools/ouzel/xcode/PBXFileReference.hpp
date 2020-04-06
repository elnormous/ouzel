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
            PBXFileReference(const std::string& initName,
                             const storage::Path& initPath,
                             const PBXFileType& initFileType,
                             PBXSourceTree initSourceTree):
                name{initName},
                path{initPath},
                fileType{initFileType},
                sourceTree{initSourceTree} {}

            std::string getIsa() const override { return "PBXFileReference"; }

            const storage::Path& getPath() const noexcept { return path; }

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

        private:
            std::string name;
            storage::Path path;
            PBXFileType fileType;
            PBXSourceTree sourceTree;
        };

        using PBXFileReferenceRef = std::reference_wrapper<const PBXFileReference>;
    }
}

#endif // OUZEL_XCODE_PBXFILEREFERENCE_HPP
