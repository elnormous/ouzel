#ifndef FLUXION_XCODE_PBXSOURCETREE_HPP
#define FLUXION_XCODE_PBXSOURCETREE_HPP

#include <string>

namespace fluxion::xcode
{
    enum class PBXSourceTree
    {
        absolute,
        group,
        sourceRoot,
        buildProductsDir,
        sdkRoot,
        developerDir
    };

    inline std::string toString(PBXSourceTree sourceTree)
    {
        switch (sourceTree)
        {
            case PBXSourceTree::absolute: return "<absolute>";
            case PBXSourceTree::group: return "<group>";
            case PBXSourceTree::sourceRoot: return "SOURCE_ROOT";
            case PBXSourceTree::buildProductsDir: return "BUILT_PRODUCTS_DIR";
            case PBXSourceTree::sdkRoot: return "SDKROOT";
            case PBXSourceTree::developerDir: return "DEVELOPER_DIR";
        }

        throw std::runtime_error{"Unsupported source tree"};
    }
}

#endif // FLUXION_XCODE_PBXSOURCETREE_HPP
