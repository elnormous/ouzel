// Ouzel by Elviss Strazdins

#ifndef OUZEL_XCODE_XCCONFIGURATIONLIST_HPP
#define OUZEL_XCODE_XCCONFIGURATIONLIST_HPP

#include <string>
#include <vector>
#include "PBXObject.hpp"
#include "XCBuildConfiguration.hpp"

namespace ouzel::xcode
{
    class XCConfigurationList final: public PBXObject
    {
    public:
        XCConfigurationList() = default;

        std::string getIsa() const override { return "XCConfigurationList"; }

        plist::Value encode() const override
        {
            auto result = PBXObject::encode();
            result["buildConfigurations"] = plist::Value::Array{};
            for (const auto configuration : configurations)
                if (configuration)
                    result["buildConfigurations"].pushBack(toString(configuration->getId()));

            result["defaultConfigurationIsVisible"] = 0;
            result["defaultConfigurationName"] = defaultConfigurationName;

            return result;
        }

        std::vector<const XCBuildConfiguration*> configurations;
        std::string defaultConfigurationName;
    };
}

#endif // OUZEL_XCODE_XCCONFIGURATIONLIST_HPP
