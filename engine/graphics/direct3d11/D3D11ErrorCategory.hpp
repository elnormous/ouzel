// Ouzel by Elviss Strazdins

#ifndef OUZEL_GRAPHICS_D3D11ERRORCATEGORY_HPP
#define OUZEL_GRAPHICS_D3D11ERRORCATEGORY_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_DIRECT3D11

#include <system_error>
#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <d3d11.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

namespace ouzel::graphics::d3d11
{
    enum ErrorCode {
        ErrorFileNotFound,
        ErrorTooManyUniqueStateObjects,
        ErrorTooManyUniqueViewObjects,
        ErrorDeferredContextMapWithoutInitialDiscard,
        ErrorInvalidCall,
        ErrorWasStillDrawing,
        ErrorNotCurrentlyAvailable,
        ErrorDeviceRemoved,
        ErrorFail,
        ErrorInvalidArg,
        ErrorOutOfMemory,
        ErrorNotImpl
    };

    class ErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept override
        {
            return "Direct3D11";
        }

        std::string message(const int condition) const override
        {
            switch (condition)
            {
                case ErrorCode::ErrorFileNotFound: return "D3D11_ERROR_FILE_NOT_FOUND";
                case ErrorCode::ErrorTooManyUniqueStateObjects: return "D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS";
                case ErrorCode::ErrorTooManyUniqueViewObjects: return "D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS";
                case ErrorCode::ErrorDeferredContextMapWithoutInitialDiscard: return "D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD";
                case ErrorCode::ErrorInvalidCall: return "DXGI_ERROR_INVALID_CALL";
                case ErrorCode::ErrorWasStillDrawing: return "DXGI_ERROR_WAS_STILL_DRAWING";
                case ErrorCode::ErrorNotCurrentlyAvailable: return "DXGI_ERROR_NOT_CURRENTLY_AVAILABLE";
                case ErrorCode::ErrorDeviceRemoved: return "DXGI_ERROR_DEVICE_REMOVED";
                case ErrorCode::ErrorFail: return "E_FAIL";
                case ErrorCode::ErrorInvalidArg: return "E_INVALIDARG";
                case ErrorCode::ErrorOutOfMemory: return "E_OUTOFMEMORY";
                case ErrorCode::ErrorNotImpl: return "E_NOTIMPL";
                default: return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };

    inline constexpr ErrorCode toErrorCode(HRESULT hr) {
        switch (hr) {
            case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
                return ErrorCode::ErrorFileNotFound;
            case D3D11_ERROR_FILE_NOT_FOUND:
                return ErrorCode::ErrorTooManyUniqueStateObjects;
            case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS:
                return ErrorCode::ErrorTooManyUniqueViewObjects;
            case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD:
                return ErrorCode::ErrorDeferredContextMapWithoutInitialDiscard;
            case DXGI_ERROR_INVALID_CALL:
                return ErrorCode::ErrorInvalidCall;
            case DXGI_ERROR_WAS_STILL_DRAWING:
                return ErrorCode::ErrorWasStillDrawing;
            case DXGI_ERROR_NOT_CURRENTLY_AVAILABLE:
                return ErrorCode::ErrorNotCurrentlyAvailable;
            case DXGI_ERROR_DEVICE_REMOVED:
                return ErrorCode::ErrorDeviceRemoved;
            case E_FAIL:
                return ErrorCode::ErrorFail;
            case E_INVALIDARG:
                return ErrorCode::ErrorInvalidArg;
            case E_OUTOFMEMORY:
                return ErrorCode::ErrorOutOfMemory;
            case E_NOTIMPL:
                return ErrorCode::ErrorNotImpl;
            
            default:
                return static_cast<int>(hr);
        }
    }

    inline const ErrorCategory errorCategory;
}
#endif

#endif // OUZEL_GRAPHICS_D3D11ERRORCATEGORY_HPP
