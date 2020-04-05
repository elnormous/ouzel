// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXGROUP_HPP
#define OUZEL_XCODE_PBXGROUP_HPP

#include <vector>
#include "PbxObject.hpp"
#include "PbxSourceTree.hpp"
#include "storage/Path.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PbxGroup final: public PbxFileElement
        {
        public:
            PbxGroup(const std::string& initName,
                     const storage::Path& initPath,
                     const std::vector<PbxFileElementRef>& initChildren,
                     PbxSourceTree initSourceTree):
                name{initName},
                path{initPath},
                children{initChildren},
                sourceTree{initSourceTree} {}

            std::string getIsa() const override { return "PBXGroup"; }

            plist::Value encode() const override
            {
                auto result = PbxFileElement::encode();
                result["children"] = plist::Value::Array{};
                result["sourceTree"] = toString(sourceTree);

                if (!std::string(path).empty())
                    result["path"] = std::string(path);
                else if (!name.empty())
                    result["name"] = name;

                for (const PbxFileElement& child : children)
                    result["children"].pushBack(toString(child.getId()));

                return result;
            }

        private:
            std::string name;
            storage::Path path;
            std::vector<PbxFileElementRef> children;
            PbxSourceTree sourceTree;
        };
    }
}

#endif // OUZEL_XCODE_PBXGROUP_HPP
