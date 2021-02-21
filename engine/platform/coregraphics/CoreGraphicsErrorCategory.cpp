// Ouzel by Elviss Strazdins

#include "CoreGraphicsErrorCategory.hpp"

namespace ouzel::platform::coregraphics
{
    namespace
    {
        const ErrorCategory errorCategory;
    }

    const std::error_category& getErrorCategory() noexcept
    {
        return errorCategory;
    }
}
