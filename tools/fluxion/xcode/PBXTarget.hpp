#ifndef FLUXION_XCODE_PBXTARGET_HPP
#define FLUXION_XCODE_PBXTARGET_HPP

#include <string>
#include "PBXObject.hpp"

namespace fluxion::xcode
{
    class PBXTarget: public PBXObject
    {
    public:
        PBXTarget() = default;

        std::string getIsa() const override { return "PBXTarget"; }
    };
}

#endif // FLUXION_XCODE_PBXTARGET_HPP
