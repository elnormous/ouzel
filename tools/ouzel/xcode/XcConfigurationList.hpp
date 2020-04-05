// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_XCCONFIGURATIONLIST_HPP
#define OUZEL_XCODE_XCCONFIGURATIONLIST_HPP

#include <vector>
#include "PbxObject.hpp"
#include "utils/Plist.hpp"

namespace ouzel
{
    namespace xcode
    {
        class XcConfigurationList final: public PbxObject
        {
        public:
            XcConfigurationList(const std::vector<Id>& configurations,
                                const std::string& defaultConfiguration):
                PbxObject{"XCConfigurationList"},
                configurationIds{configurations},
                defaultConfigurationName{defaultConfiguration} {}

            plist::Value getValue() const
            {
                auto result = plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"buildConfigurations", plist::Value::Array{}},
                    {"defaultConfigurationIsVisible", 0},
                    {"defaultConfigurationName", defaultConfigurationName},
                };

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
