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
            PbxGroup(const std::string& n,
                     const storage::Path& p,
                     const std::vector<Id>& children,
                     PbxSourceTree tree):
                name{n},
                path{p},
                childIds{children},
                sourceTree{tree} {}

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

                for (auto childId : childIds)
                    result["children"].pushBack(toString(childId));

                return result;
            }

        private:
            std::string name;
            storage::Path path;
            std::vector<Id> childIds;
            PbxSourceTree sourceTree;
        };
    }
}

#endif // OUZEL_XCODE_PBXGROUP_HPP
