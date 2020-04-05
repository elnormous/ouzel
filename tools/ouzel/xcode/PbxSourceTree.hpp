// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXSOURCETREE_HPP
#define OUZEL_XCODE_PBXSOURCETREE_HPP

#include <string>

namespace ouzel
{
    namespace xcode
    {
        enum class PbxSourceTree
        {
            Absolute,
            Group,
            SourceRoot,
            BuildProductsDir,
            SdkRoot,
            DeveloperDir
        };

        inline std::string toString(PbxSourceTree sourceTree)
        {
            switch (sourceTree)
            {
                case PbxSourceTree::Absolute: return "<absolute>";
                case PbxSourceTree::Group: return "<group>";
                case PbxSourceTree::SourceRoot: return "SOURCE_ROOT";
                case PbxSourceTree::BuildProductsDir: return "BUILT_PRODUCTS_DIR";
                case PbxSourceTree::SdkRoot: return "SDKROOT";
                case PbxSourceTree::DeveloperDir: return "DEVELOPER_DIR";
            }
        }
    }
}

#endif // OUZEL_XCODE_PBXSOURCETREE_HPP
