// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXTARGET_HPP
#define OUZEL_XCODE_PBXTARGET_HPP

#include "PBXObject.hpp"

namespace ouzel
{
    namespace xcode
    {
        class PBXTarget: public PBXObject
        {
        public:
            PBXTarget() = default;

            std::string getIsa() const override { return "PBXTarget"; }
        };

        using PBXTargetRef = std::reference_wrapper<const PBXTarget>;
    }
}

#endif // OUZEL_XCODE_PBXTARGET_HPP
