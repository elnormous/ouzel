// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXGROUP_HPP
#define OUZEL_XCODE_PBXGROUP_HPP

#include <vector>
#include "PBXFileElement.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PBXGroup final: public PBXFileElement
        {
        public:
            PBXGroup() = default;

            std::string getIsa() const override { return "PBXGroup"; }

            plist::Value encode() const override
            {
                auto result = PBXFileElement::encode();
                result["children"] = plist::Value::Array{};
                for (auto child : children)
                    if (child) result["children"].pushBack(toString(child->getId()));

                return result;
            }

            std::vector<const PBXFileElement*> children;
        };
    }
}

#endif // OUZEL_XCODE_PBXGROUP_HPP
