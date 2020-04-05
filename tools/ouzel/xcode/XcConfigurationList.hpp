// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_XCCONFIGURATIONLIST_HPP
#define OUZEL_XCODE_XCCONFIGURATIONLIST_HPP

#include <vector>
#include "PbxObject.hpp"

namespace ouzel
{
    namespace xcode
    {
        class XcConfigurationList final: public PbxObject
        {
        public:
            XcConfigurationList(const std::vector<Id>& configurations,
                                const std::string& defaultConfiguration):
                configurationIds{configurations},
                defaultConfigurationName{defaultConfiguration} {}

            std::string getIsa() const override { return "XCConfigurationList"; }

            plist::Value encode() const override
            {
                auto result = PbxObject::encode();
                result["buildConfigurations"] = plist::Value::Array{};
                result["defaultConfigurationIsVisible"] = 0;
                result["defaultConfigurationName"] = defaultConfigurationName;

                for (auto configurationId : configurationIds)
                    result["buildConfigurations"].pushBack(toString(configurationId));

                return result;
            }

        private:
            std::vector<Id> configurationIds;
            std::string defaultConfigurationName;
        };
    }
}

#endif // OUZEL_XCODE_XCCONFIGURATIONLIST_HPP
