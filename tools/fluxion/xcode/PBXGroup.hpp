#ifndef FLUXION_XCODE_PBXGROUP_HPP
#define FLUXION_XCODE_PBXGROUP_HPP

#include <vector>
#include "PBXFileElement.hpp"

namespace fluxion::xcode
{
    class PBXGroup final: public PBXFileElement
    {
    public:
        PBXGroup() = default;

        std::string getIsa() const override { return "PBXGroup"; }

        plist::Value encode() const override
        {
            auto result = PBXFileElement::encode();
            result["children"] = plist::Array{};
            for (const auto child : children)
                if (child) result["children"].pushBack(toString(child->getId()));

            return result;
        }

        std::vector<const PBXFileElement*> children;
    };
}

#endif // FLUXION_XCODE_PBXGROUP_HPP
