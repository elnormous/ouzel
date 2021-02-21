// Ouzel by Elviss Strazdins

#ifndef OUZEL_XCODE_PBXCONTAINERITEMPROXY_HPP
#define OUZEL_XCODE_PBXCONTAINERITEMPROXY_HPP

#include "PBXObject.hpp"
#include "PBXFileReference.hpp"

namespace ouzel::xcode
{
    class PBXContainerItemProxy final: public PBXObject
    {
    public:
        enum class ProxyType
        {
            nativeTarget = 1,
            reference = 2
        };

        PBXContainerItemProxy() = default;

        std::string getIsa() const override { return "PBXContainerItemProxy"; }

        plist::Value encode() const override
        {
            auto result = PBXObject::encode();
            if (containerPortal) result["containerPortal"] = toString(containerPortal->getId());
            result["proxyType"] = static_cast<std::uint32_t>(proxyType);
            if (remoteGlobal) result["remoteGlobalIDString"] = toString(remoteGlobal->getId());
            result["remoteInfo"] = remoteInfo;
            return result;
        }

        const PBXObject* containerPortal = nullptr;
        ProxyType proxyType = ProxyType::nativeTarget;
        const PBXObject* remoteGlobal = nullptr;
        std::string remoteInfo;
    };
}

#endif // OUZEL_XCODE_PBXCONTAINERITEMPROXY_HPP
