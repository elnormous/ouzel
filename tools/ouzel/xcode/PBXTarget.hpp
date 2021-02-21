// Ouzel by Elviss Strazdins

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
