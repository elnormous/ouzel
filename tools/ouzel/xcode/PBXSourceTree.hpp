// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_XCODE_PBXSOURCETREE_HPP
#define OUZEL_XCODE_PBXSOURCETREE_HPP

#include <string>

namespace ouzel
{
    namespace xcode
    {
        enum class PBXSourceTree
        {
            Absolute,
            Group,
            SourceRoot,
            BuildProductsDir,
            SdkRoot,
            DeveloperDir
        };

        inline std::string toString(PBXSourceTree sourceTree)
        {
            switch (sourceTree)
            {
                case PBXSourceTree::Absolute: return "<absolute>";
                case PBXSourceTree::Group: return "<group>";
                case PBXSourceTree::SourceRoot: return "SOURCE_ROOT";
                case PBXSourceTree::BuildProductsDir: return "BUILT_PRODUCTS_DIR";
                case PBXSourceTree::SdkRoot: return "SDKROOT";
                case PBXSourceTree::DeveloperDir: return "DEVELOPER_DIR";
            }
        }
    }
}

#endif // OUZEL_XCODE_PBXSOURCETREE_HPP
