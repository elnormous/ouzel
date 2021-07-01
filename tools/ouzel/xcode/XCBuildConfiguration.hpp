// Ouzel by Elviss Strazdins

#ifndef OUZEL_XCODE_XCBUILDCONFIGURATION_HPP
#define OUZEL_XCODE_XCBUILDCONFIGURATION_HPP

#include <map>
#include <string>
#include "PBXObject.hpp"

namespace ouzel::xcode
{
    class XCBuildConfiguration final: public PBXObject
    {
    public:
        XCBuildConfiguration() = default;

        std::string getIsa() const override { return "XCBuildConfiguration"; }

        plist::Value encode() const override
        {
            auto result = PBXObject::encode();
            result["buildSettings"] = plist::Dictionary{};
            for (const auto& [key, value] : buildSettings)
                result["buildSettings"][key] = value;

            result["name"] = name;

            return result;
        }

        std::string name;
        std::map<std::string, std::string> buildSettings;
    };
}

#endif // OUZEL_XCODE_XCBUILDCONFIGURATION_HPP
