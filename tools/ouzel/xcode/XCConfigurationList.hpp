// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_XCCONFIGURATIONLIST_HPP
#define OUZEL_XCODE_XCCONFIGURATIONLIST_HPP

#include <vector>
#include "PBXObject.hpp"

namespace ouzel
{
    namespace xcode
    {
        class XCConfigurationList final: public PBXObject
        {
        public:
            XCConfigurationList(const std::vector<XcBuildConfigurationRef>& initConfigurations,
                                const std::string& initDefaultConfigurationName):
                configurations{initConfigurations},
                defaultConfigurationName{initDefaultConfigurationName} {}

            std::string getIsa() const override { return "XCConfigurationList"; }

            plist::Value encode() const override
            {
                auto result = PBXObject::encode();
                result["buildConfigurations"] = plist::Value::Array{};
                result["defaultConfigurationIsVisible"] = 0;
                result["defaultConfigurationName"] = defaultConfigurationName;

                for (const XCBuildConfiguration& configuration : configurations)
                    result["buildConfigurations"].pushBack(toString(configuration.getId()));

                return result;
            }

        private:
            std::vector<XcBuildConfigurationRef> configurations;
            std::string defaultConfigurationName;
        };
    }
}

#endif // OUZEL_XCODE_XCCONFIGURATIONLIST_HPP
