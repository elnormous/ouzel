// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

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
