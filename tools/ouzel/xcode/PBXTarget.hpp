// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXTARGET_HPP
#define OUZEL_XCODE_PBXTARGET_HPP

#include <string>
#include "PBXObject.hpp"

namespace ouzel::xcode
{
    class PBXTarget: public PBXObject
    {
    public:
        PBXTarget() = default;

        std::string getIsa() const override { return "PBXTarget"; }
    };
}

#endif // OUZEL_XCODE_PBXTARGET_HPP
