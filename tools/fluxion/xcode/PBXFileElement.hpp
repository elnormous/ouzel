#ifndef FLUXION_XCODE_PBXFILEELEMENT_HPP
#define FLUXION_XCODE_PBXFILEELEMENT_HPP

#include "PBXObject.hpp"
#include "PBXSourceTree.hpp"
#include "storage/Path.hpp"

namespace fluxion::xcode
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

#endif // FLUXION_XCODE_PBXFILEELEMENT_HPP
