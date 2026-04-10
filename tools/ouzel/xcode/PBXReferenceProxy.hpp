#ifndef FLUXION_XCODE_PBXREFERENCEPROXY_HPP
#define FLUXION_XCODE_PBXREFERENCEPROXY_HPP

#include <string>
#include "PBXFileReference.hpp"
#include "PBXContainerItemProxy.hpp"
#include "PBXSourceTree.hpp"
#include "storage/Path.hpp"

namespace fluxion::xcode
{
    class PBXReferenceProxy final: public PBXFileReference
    {
    public:
        PBXReferenceProxy() = default;

        std::string getIsa() const override { return "PBXReferenceProxy"; }

        plist::Value encode() const override
        {
            auto result = PBXFileReference::encode();
            if (remoteRef) result["remoteRef"] = toString(remoteRef->getId());
            return result;
        }

        const PBXContainerItemProxy* remoteRef = nullptr;
    };
}

#endif // FLUXION_XCODE_PBXREFERENCEPROXY_HPP
