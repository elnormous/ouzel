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
            PbxTarget() = default;

            std::string getIsa() const override { return "PBXTarget"; }
        };

        using PbxTargetRef = std::reference_wrapper<const PbxTarget>;
    }
}

#endif // OUZEL_XCODE_PBXTARGET_HPP
