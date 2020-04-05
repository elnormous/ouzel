// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_XCBUILDCONFIGURATION_HPP
#define OUZEL_XCODE_XCBUILDCONFIGURATION_HPP

#include <map>
#include "PbxObject.hpp"

namespace ouzel
{
    namespace xcode
    {
        class XcBuildConfiguration final: public PbxObject
        {
        public:
            XcBuildConfiguration(const std::string& initName,
                                 const std::map<std::string, std::string>& initBuildSettings):
                name{initName},
                buildSettings{initBuildSettings} {}

            std::string getIsa() const override { return "XCBuildConfiguration"; }

            const std::string& getName() const noexcept { return name; }

            plist::Value encode() const override
            {
                auto result = PbxObject::encode();
                result["buildSettings"] = plist::Value::Dictionary{};
                result["name"] = name;

                for (const auto& buildSetting : buildSettings)
                    result["buildSettings"][buildSetting.first] = buildSetting.second;

                return result;
            }

        private:
            std::string name;
            std::map<std::string, std::string> buildSettings;
        };

        using XcBuildConfigurationRef = std::reference_wrapper<const XcBuildConfiguration>;
    }
}

#endif // OUZEL_XCODE_XCBUILDCONFIGURATION_HPP
