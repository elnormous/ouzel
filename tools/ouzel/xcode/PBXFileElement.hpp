// Ouzel by Elviss Strazdins

#ifndef OUZEL_XCODE_PBXFILEELEMENT_HPP
#define OUZEL_XCODE_PBXFILEELEMENT_HPP

#include "PBXObject.hpp"
#include "PBXSourceTree.hpp"
#include "storage/Path.hpp"

namespace ouzel::xcode
{
    class PBXFileElement: public PBXObject
    {
    public:
        PBXFileElement() = default;

        std::string getIsa() const override { return "PBXFileElement"; }

        plist::Value encode() const override
        {
            auto result = PBXObject::encode();
            if (!name.empty()) result["name"] = name;
            if (!path.isEmpty()) result["path"] = std::string(path);
            result["sourceTree"] = toString(sourceTree);

            return result;
        }

        std::string name;
        storage::Path path;
        PBXSourceTree sourceTree = PBXSourceTree::absolute;
    };
}

#endif // OUZEL_XCODE_PBXFILEELEMENT_HPP
