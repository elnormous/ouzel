// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_OSLERRORCATEGORY_HPP
#define OUZEL_AUDIO_OSLERRORCATEGORY_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_OPENSL

#include <system_error>
#include <SLES/OpenSLES.h>

namespace ouzel::audio::opensl
{
    class ErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept final
        {
            return "OpenSL";
        }

        std::string message(int condition) const final
        {
            switch (condition)
            {
                case SL_RESULT_PRECONDITIONS_VIOLATED: return "SL_RESULT_PRECONDITIONS_VIOLATED";
                case SL_RESULT_PARAMETER_INVALID: return "SL_RESULT_PARAMETER_INVALID";
                case SL_RESULT_MEMORY_FAILURE: return "SL_RESULT_MEMORY_FAILURE";
                case SL_RESULT_RESOURCE_ERROR: return "SL_RESULT_RESOURCE_ERROR";
                case SL_RESULT_RESOURCE_LOST: return "SL_RESULT_RESOURCE_LOST";
                case SL_RESULT_IO_ERROR: return "SL_RESULT_IO_ERROR";
                case SL_RESULT_BUFFER_INSUFFICIENT: return "SL_RESULT_BUFFER_INSUFFICIENT";
                case SL_RESULT_CONTENT_CORRUPTED: return "SL_RESULT_CONTENT_CORRUPTED";
                case SL_RESULT_CONTENT_UNSUPPORTED: return "SL_RESULT_CONTENT_UNSUPPORTED";
                case SL_RESULT_CONTENT_NOT_FOUND: return "SL_RESULT_CONTENT_NOT_FOUND";
                case SL_RESULT_PERMISSION_DENIED: return "SL_RESULT_PERMISSION_DENIED";
                case SL_RESULT_FEATURE_UNSUPPORTED: return "SL_RESULT_FEATURE_UNSUPPORTED";
                case SL_RESULT_INTERNAL_ERROR: return "SL_RESULT_INTERNAL_ERROR";
                case SL_RESULT_UNKNOWN_ERROR: return "SL_RESULT_UNKNOWN_ERROR";
                case SL_RESULT_OPERATION_ABORTED: return "SL_RESULT_OPERATION_ABORTED";
                case SL_RESULT_CONTROL_LOST: return "SL_RESULT_CONTROL_LOST";
                default: return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };
}
#endif

#endif // OUZEL_AUDIO_OSLERRORCATEGORY_HPP
