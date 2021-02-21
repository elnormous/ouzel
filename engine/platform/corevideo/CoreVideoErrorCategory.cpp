// Ouzel by Elviss Strazdins

#include "CoreVideoErrorCategory.hpp"

namespace ouzel::platform::corevideo
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
