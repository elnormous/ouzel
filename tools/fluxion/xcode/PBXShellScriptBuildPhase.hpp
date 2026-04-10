#ifndef FLUXION_XCODE_PBXSHELLSCRIPTBUILDPHASE_HPP
#define FLUXION_XCODE_PBXSHELLSCRIPTBUILDPHASE_HPP

#include <string>
#include "PBXBuildPhase.hpp"

namespace fluxion::xcode
{
    class PBXShellScriptBuildPhase final: public PBXBuildPhase
    {
    public:
        PBXShellScriptBuildPhase() = default;

        std::string getIsa() const override { return "PBXShellScriptBuildPhase"; }

        plist::Value encode() const override
        {
            auto result = PBXBuildPhase::encode();
            result["files"] = plist::Array();
            result["inputFileListPaths"] = plist::Array();
            result["inputPaths"] = plist::Array();
            result["outputFileListPaths"] = plist::Array();
            result["outputPaths"] = plist::Array();
            result["shellPath"] = "/bin/sh";
            result["shellScript"] = shellScript;

            return result;
        }

        std::string shellScript;
    };
}

#endif // FLUXION_XCODE_PBXSHELLSCRIPTBUILDPHASE_HPP
