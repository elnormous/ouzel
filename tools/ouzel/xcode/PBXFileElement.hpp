// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXFILEELEMENT_HPP
#define OUZEL_XCODE_PBXFILEELEMENT_HPP

#include "PBXObject.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PBXFileElement: public PBXObject
        {
        public:
            PBXFileElement() = default;

            std::string getIsa() const override { return "PBXFileElement"; }
        };

        using PBXFileElementRef = std::reference_wrapper<const PBXFileElement>;
    }
}

#endif // OUZEL_XCODE_PBXFILEELEMENT_HPP
