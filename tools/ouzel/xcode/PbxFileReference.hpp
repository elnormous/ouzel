// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXFILEREFERENCE_HPP
#define OUZEL_XCODE_PBXFILEREFERENCE_HPP

#include "PbxFileElement.hpp"
#include "PbxFileType.hpp"
#include "PbxSourceTree.hpp"
#include "storage/Path.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PbxFileReference: public PbxFileElement
        {
        public:
            PbxFileReference(const std::string initName,
                             const storage::Path& initPath,
                             const PbxFileType& initFileType,
                             PbxSourceTree initSourceTree):
                name{initName},
                path{initPath},
                fileType{initFileType},
                sourceTree{initSourceTree} {}

            std::string getIsa() const override { return "PBXFileReference"; }

            const storage::Path& getPath() const noexcept { return path; }

            plist::Value encode() const override
            {
                auto result = PbxFileElement::encode();
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
            PbxFileType fileType;
            PbxSourceTree sourceTree;
        };

        using PbxFileReferenceRef = std::reference_wrapper<const PbxFileReference>;
    }
}

#endif // OUZEL_XCODE_PBXFILEREFERENCE_HPP
