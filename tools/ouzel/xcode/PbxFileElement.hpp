// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXFILEELEMENT_HPP
#define OUZEL_XCODE_PBXFILEELEMENT_HPP

#include "PbxObject.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PbxFileElement: public PbxObject
        {
        public:
            PbxFileElement() = default;

            std::string getIsa() const override { return "PBXFileElement"; }
        };
    }
}

#endif // OUZEL_XCODE_PBXFILEELEMENT_HPP
