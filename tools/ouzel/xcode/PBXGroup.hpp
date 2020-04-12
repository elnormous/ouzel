// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXGROUP_HPP
#define OUZEL_XCODE_PBXGROUP_HPP

#include <vector>
#include "PBXFileElement.hpp"
#include "PBXSourceTree.hpp"
#include "storage/Path.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PBXGroup final: public PBXFileElement
        {
        public:
            PBXGroup(const std::string& initName,
                     const storage::Path& initPath,
                     const std::vector<PBXFileElementRef>& initChildren,
                     PBXSourceTree initSourceTree):
                name{initName},
                path{initPath},
                children{initChildren},
                sourceTree{initSourceTree} {}

            std::string getIsa() const override { return "PBXGroup"; }

            plist::Value encode() const override
            {
                auto result = PBXFileElement::encode();
                result["children"] = plist::Value::Array{};
                for (const PBXFileElement& child : children)
                    result["children"].pushBack(toString(child.getId()));

                result["sourceTree"] = toString(sourceTree);

                if (!std::string(path).empty())
                    result["path"] = std::string(path);
                else if (!name.empty())
                    result["name"] = name;

                return result;
            }

        private:
            std::string name;
            storage::Path path;
            std::vector<PBXFileElementRef> children;
            PBXSourceTree sourceTree;
        };
    }
}

#endif // OUZEL_XCODE_PBXGROUP_HPP
