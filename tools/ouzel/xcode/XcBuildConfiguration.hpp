// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_XCBUILDCONFIGURATION_HPP
#define OUZEL_XCODE_XCBUILDCONFIGURATION_HPP

#include <map>
#include "PbxObject.hpp"
#include "utils/Plist.hpp"

namespace ouzel
{
    namespace xcode
    {
        class XcBuildConfiguration final: public PbxObject
        {
        public:
            XcBuildConfiguration(const std::string& n,
                                 const std::map<std::string, std::string>& settings):
                PbxObject{"XCBuildConfiguration"},
                name{n},
                buildSettings{settings} {}

            const std::string& getName() const noexcept { return name; }

            plist::Value getValue() const
            {
                auto result = plist::Value::Dictionary{
                    {"isa", getIsa()},
                    {"buildSettings", plist::Value::Dictionary{}},
                    {"name", name}
                };

                for (const auto& buildSetting : buildSettings)
                    result["buildSettings"][buildSetting.first] = buildSetting.second;

                return result;
            }

        private:
            std::string name;
            std::map<std::string, std::string> buildSettings;
        };
    }
}

#endif // OUZEL_XCODE_XCBUILDCONFIGURATION_HPP
