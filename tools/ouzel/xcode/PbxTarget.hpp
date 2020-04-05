// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXTARGET_HPP
#define OUZEL_XCODE_PBXTARGET_HPP

#include "PbxObject.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PbxTarget: public PbxObject
        {
        public:
            PbxTarget(const std::string& i): PbxObject{i} {}
        };
    }
}

#endif // OUZEL_XCODE_PBXTARGET_HPP
