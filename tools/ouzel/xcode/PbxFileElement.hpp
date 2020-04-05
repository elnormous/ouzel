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
            PbxFileElement(const std::string& i): PbxObject{i} {}
        };
    }
}

#endif // OUZEL_XCODE_PBXFILEELEMENT_HPP
