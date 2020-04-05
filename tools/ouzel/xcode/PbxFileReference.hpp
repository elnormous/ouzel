// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXFILEREFERENCE_HPP
#define OUZEL_XCODE_PBXFILEREFERENCE_HPP

#include "PbxFileElement.hpp"
#include "PbxSourceTree.hpp"
#include "storage/Path.hpp"
#include "utils/Plist.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PbxFileReference final: public PbxFileElement
        {
        public:
            PbxFileReference(const std::string n,
                             const storage::Path& p,
                             const std::string& type,
                             PbxSourceTree tree):
                PbxFileElement{"PBXFileReference"},
                name{n},
                path{p},
                fileType{type},
                sourceTree{tree} {}

            const storage::Path& getPath() const noexcept { return path; }

            plist::Value getValue() const
            {
                auto result = plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"explicitFileType", fileType},
                    {"includeInIndex", 0},
                    {"path", std::string(path)},
                    {"sourceTree", toString(sourceTree)}
                };

                if (!name.empty())
                    result["name"] = name;

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
