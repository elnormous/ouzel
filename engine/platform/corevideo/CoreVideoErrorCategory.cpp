// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

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
