// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_XCCONFIGURATIONLIST_HPP
#define OUZEL_XCODE_XCCONFIGURATIONLIST_HPP

#include <vector>
#include "PBXObject.hpp"
#include "XCBuildConfiguration.hpp"

namespace ouzel
{
    namespace xcode
    {
        class XCConfigurationList final: public PBXObject
        {
        public:
            XCConfigurationList(const std::vector<XCBuildConfigurationRef>& initConfigurations,
                                const std::string& initDefaultConfigurationName):
                configurations{initConfigurations},
                defaultConfigurationName{initDefaultConfigurationName} {}

            std::string getIsa() const override { return "XCConfigurationList"; }

            plist::Value encode() const override
            {
                auto result = PBXObject::encode();
                result["buildConfigurations"] = plist::Value::Array{};
                for (const XCBuildConfiguration& configuration : configurations)
                    result["buildConfigurations"].pushBack(toString(configuration.getId()));

                result["defaultConfigurationIsVisible"] = 0;
                result["defaultConfigurationName"] = defaultConfigurationName;

                return result;
            }

        private:
            std::vector<XCBuildConfigurationRef> configurations;
            std::string defaultConfigurationName;
        };
    }
}

#endif // OUZEL_XCODE_XCCONFIGURATIONLIST_HPP
