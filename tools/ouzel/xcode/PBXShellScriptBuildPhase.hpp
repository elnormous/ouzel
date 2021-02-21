// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXSHELLSCRIPTBUILDPHASE_HPP
#define OUZEL_XCODE_PBXSHELLSCRIPTBUILDPHASE_HPP

#include <string>
#include "PBXBuildPhase.hpp"

namespace ouzel::xcode
{
    class PBXShellScriptBuildPhase final: public PBXBuildPhase
    {
    public:
        PBXShellScriptBuildPhase() = default;

        std::string getIsa() const override { return "PBXShellScriptBuildPhase"; }

        plist::Value encode() const override
        {
            auto result = PBXBuildPhase::encode();
            result["files"] = plist::Value::Array();
            result["inputFileListPaths"] = plist::Value::Array();
            result["inputPaths"] = plist::Value::Array();
            result["outputFileListPaths"] = plist::Value::Array();
            result["outputPaths"] = plist::Value::Array();
            result["shellPath"] = "/bin/sh";
            result["shellScript"] = shellScript;

            return result;
        }

        std::string shellScript;
    };
}

#endif // OUZEL_XCODE_PBXSHELLSCRIPTBUILDPHASE_HPP
