// Ouzel by Elviss Strazdins

#ifndef OUZEL_AUDIO_WASAPIERRORCATEGORY_HPP
#define OUZEL_AUDIO_WASAPIERRORCATEGORY_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_WASAPI

#include <system_error>
#include <Audioclient.h>

namespace ouzel::audio::wasapi
{
    class ErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept final
        {
            return "WASAPI";
        }

        std::string message(const int condition) const final
        {
            switch (condition)
            {
                case REGDB_E_CLASSNOTREG: return "REGDB_E_CLASSNOTREG";
                case CLASS_E_NOAGGREGATION: return "CLASS_E_NOAGGREGATION";
                case E_POINTER: return "E_POINTER";
                case E_INVALIDARG: return "E_INVALIDARG";
                case __HRESULT_FROM_WIN32(ERROR_NOT_FOUND) /* E_NOTFOUND */: return "E_NOTFOUND";
                case E_OUTOFMEMORY: return "E_OUTOFMEMORY";
                case E_NOINTERFACE: return "E_NOINTERFACE";
                case AUDCLNT_E_NOT_INITIALIZED: return "AUDCLNT_E_NOT_INITIALIZED";
                case AUDCLNT_E_ALREADY_INITIALIZED: return "AUDCLNT_E_ALREADY_INITIALIZED";
                case AUDCLNT_E_WRONG_ENDPOINT_TYPE: return "AUDCLNT_E_WRONG_ENDPOINT_TYPE";
                case AUDCLNT_E_DEVICE_INVALIDATED: return "AUDCLNT_E_DEVICE_INVALIDATED";
                case AUDCLNT_E_NOT_STOPPED: return "AUDCLNT_E_NOT_STOPPED";
                case AUDCLNT_E_BUFFER_TOO_LARGE: return "AUDCLNT_E_BUFFER_TOO_LARGE";
                case AUDCLNT_E_OUT_OF_ORDER: return "AUDCLNT_E_OUT_OF_ORDER";
                case AUDCLNT_E_UNSUPPORTED_FORMAT: return "AUDCLNT_E_UNSUPPORTED_FORMAT";
                case AUDCLNT_E_INVALID_SIZE: return "AUDCLNT_E_INVALID_SIZE";
                case AUDCLNT_E_DEVICE_IN_USE: return "AUDCLNT_E_DEVICE_IN_USE";
                case AUDCLNT_E_BUFFER_OPERATION_PENDING: return "AUDCLNT_E_BUFFER_OPERATION_PENDING";
                case AUDCLNT_E_THREAD_NOT_REGISTERED: return "AUDCLNT_E_THREAD_NOT_REGISTERED";
                case AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED: return "AUDCLNT_E_EXCLUSIVE_MODE_NOT_ALLOWED";
                case AUDCLNT_E_ENDPOINT_CREATE_FAILED: return "AUDCLNT_E_ENDPOINT_CREATE_FAILED";
                case AUDCLNT_E_SERVICE_NOT_RUNNING: return "AUDCLNT_E_SERVICE_NOT_RUNNING";
                case AUDCLNT_E_EVENTHANDLE_NOT_EXPECTED: return "AUDCLNT_E_EVENTHANDLE_NOT_EXPECTED";
                case AUDCLNT_E_EXCLUSIVE_MODE_ONLY: return "AUDCLNT_E_EXCLUSIVE_MODE_ONLY";
                case AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL: return "AUDCLNT_E_BUFDURATION_PERIOD_NOT_EQUAL";
                case AUDCLNT_E_EVENTHANDLE_NOT_SET: return "AUDCLNT_E_EVENTHANDLE_NOT_SET";
                case AUDCLNT_E_INCORRECT_BUFFER_SIZE: return "AUDCLNT_E_INCORRECT_BUFFER_SIZE";
                case AUDCLNT_E_BUFFER_SIZE_ERROR: return "AUDCLNT_E_BUFFER_SIZE_ERROR";
                case AUDCLNT_E_CPUUSAGE_EXCEEDED: return "AUDCLNT_E_CPUUSAGE_EXCEEDED";
                case AUDCLNT_E_BUFFER_ERROR: return "AUDCLNT_E_BUFFER_ERROR";
                case AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED: return "AUDCLNT_E_BUFFER_SIZE_NOT_ALIGNED";
                case AUDCLNT_E_INVALID_DEVICE_PERIOD: return "AUDCLNT_E_INVALID_DEVICE_PERIOD";
                case AUDCLNT_E_INVALID_STREAM_FLAG: return "AUDCLNT_E_INVALID_STREAM_FLAG";
                case AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE: return "AUDCLNT_E_ENDPOINT_OFFLOAD_NOT_CAPABLE";
                case AUDCLNT_E_OUT_OF_OFFLOAD_RESOURCES: return "AUDCLNT_E_OUT_OF_OFFLOAD_RESOURCES";
                case AUDCLNT_E_OFFLOAD_MODE_ONLY: return "AUDCLNT_E_OFFLOAD_MODE_ONLY";
                case AUDCLNT_E_NONOFFLOAD_MODE_ONLY: return "AUDCLNT_E_NONOFFLOAD_MODE_ONLY";
                case AUDCLNT_E_RESOURCES_INVALIDATED: return "AUDCLNT_E_RESOURCES_INVALIDATED";
                case AUDCLNT_E_RAW_MODE_UNSUPPORTED: return "AUDCLNT_E_RAW_MODE_UNSUPPORTED";
                case AUDCLNT_S_BUFFER_EMPTY: return "AUDCLNT_S_BUFFER_EMPTY";
                case AUDCLNT_S_THREAD_ALREADY_REGISTERED: return "AUDCLNT_S_THREAD_ALREADY_REGISTERED";
                case AUDCLNT_S_POSITION_STALLED: return "AUDCLNT_S_POSITION_STALLED";
                default: return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };
}
#endif

#endif // OUZEL_AUDIO_WASAPIERRORCATEGORY_HPP
