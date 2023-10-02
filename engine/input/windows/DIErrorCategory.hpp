// Ouzel by Elviss Strazdins

#ifndef OUZEL_INPUT_DIERRORCATEGORY_HPP
#define OUZEL_INPUT_DIERRORCATEGORY_HPP

#include <system_error>
#include <stdint.h>

#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

namespace ouzel::input::windows
{
	enum ErrorCode {
		ErrorAcquired,
		ErrorAlreadyInitialized,
		ErrorBadDriverVer,
		ErrorBetaDirectInputVersion,
		ErrorDeviceFull,
		ErrorDeviceNotReg,
		ErrorEffectPlaying,
		ErrorGeneric,
		ErrorHandleExists,
		ErrorHasEffects,
		ErrorInCompleteEffect,
		ErrorInputLost,
		ErrorInvalidParam,
		ErrorMapFileFail,
		ErrorMoreData,
		ErrorNoAggregation,
		ErrorNoInterface,
		ErrorNotAcquired,
		ErrorNotBuffered,
		ErrorNotDownloaded,
		ErrorNotExclusiveAcquired,
		ErrorNotfound,
		ErrorNotInitialized,
		ErrorOldDirectInputvVersion,
		ErrorOutOfMemory,
		ErrorReportFull,
		ErrorUnplugged,
		ErrorUnsupported,
		ErrorHandle,
		ErrorPending,
		ErrorPointer,
	};

    class ErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept override
        {
            return "DirectInput";
        }

        std::string message(const int condition) const override
        {
            switch (condition)
            {
                case ErrorCode::ErrorAcquired: return "DIERR_ACQUIRED";
                case ErrorCode::ErrorAlreadyInitialized: return "DIERR_ALREADYINITIALIZED";
                case ErrorCode::ErrorBadDriverVer: return "DIERR_BADDRIVERVER";
                case ErrorCode::ErrorBetaDirectInputVersion: return "DIERR_BETADIRECTINPUTVERSION";
                case ErrorCode::ErrorDeviceFull: return "DIERR_DEVICEFULL";
                case ErrorCode::ErrorDeviceNotReg: return "DIERR_DEVICENOTREG";
                case ErrorCode::ErrorEffectPlaying: return "DIERR_EFFECTPLAYING";
                case ErrorCode::ErrorGeneric: return "DIERR_GENERIC";
                case ErrorCode::ErrorHandleExists: return "DIERR_HANDLEEXISTS";
                case ErrorCode::ErrorHasEffects: return "DIERR_HASEFFECTS";
                case ErrorCode::ErrorInCompleteEffect: return "DIERR_INCOMPLETEEFFECT";
                case ErrorCode::ErrorInputLost: return "DIERR_INPUTLOST";
                case ErrorCode::ErrorInvalidParam: return "DIERR_INVALIDPARAM";
                case ErrorCode::ErrorMapFileFail: return "DIERR_MAPFILEFAIL";
                case ErrorCode::ErrorMoreData: return "DIERR_MOREDATA";
                case ErrorCode::ErrorNoAggregation: return "DIERR_NOAGGREGATION";
                case ErrorCode::ErrorNoInterface: return "DIERR_NOINTERFACE";
                case ErrorCode::ErrorNotAcquired: return "DIERR_NOTACQUIRED";
                case ErrorCode::ErrorNotBuffered: return "DIERR_NOTBUFFERED";
                case ErrorCode::ErrorNotDownloaded: return "DIERR_NOTDOWNLOADED";
                case ErrorCode::ErrorNotExclusiveAcquired: return "DIERR_NOTEXCLUSIVEACQUIRED";
                case ErrorCode::ErrorNotfound: return "DIERR_NOTFOUND";
                case ErrorCode::ErrorNotInitialized: return "DIERR_NOTINITIALIZED";
                case ErrorCode::ErrorOldDirectInputvVersion: return "DIERR_OLDDIRECTINPUTVERSION";
                case ErrorCode::ErrorOutOfMemory: return "DIERR_OUTOFMEMORY";
                case ErrorCode::ErrorReportFull: return "DIERR_REPORTFULL";
                case ErrorCode::ErrorUnplugged: return "DIERR_UNPLUGGED";
                case ErrorCode::ErrorUnsupported: return "DIERR_UNSUPPORTED";
                case ErrorCode::ErrorHandle: return "E_HANDLE";
                case ErrorCode::ErrorPending: return "E_PENDING";
                case ErrorCode::ErrorPointer: return "E_POINTER";
                default: return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };

    inline constexpr ErrorCode toErrorCode(int64_t hr) {
        switch (hr) {
            case DIERR_ACQUIRED:
                return ErrorCode::ErrorAcquired;
            case DIERR_ALREADYINITIALIZED:
                return ErrorCode::ErrorAlreadyInitialized;
            case DIERR_BADDRIVERVER:
                return ErrorCode::ErrorBadDriverVer;
            case DIERR_BETADIRECTINPUTVERSION:
                return ErrorCode::ErrorBetaDirectInputVersion;
            case DIERR_DEVICEFULL:
                return ErrorCode::ErrorDeviceFull;
            case DIERR_DEVICENOTREG:
                return ErrorCode::ErrorDeviceNotReg;
            case DIERR_EFFECTPLAYING:
                return ErrorCode::ErrorEffectPlaying;
            case DIERR_GENERIC:
                return ErrorCode::ErrorGeneric;
            case DIERR_HANDLEEXISTS:
                return ErrorCode::ErrorHandleExists;
            case DIERR_HASEFFECTS:
                return ErrorCode::ErrorHasEffects;
            case DIERR_INCOMPLETEEFFECT:
                return ErrorCode::ErrorInCompleteEffect;
            case DIERR_INPUTLOST:
                return ErrorCode::ErrorInputLost;
            case DIERR_INVALIDPARAM:
                return ErrorCode::ErrorInvalidParam;
            case DIERR_MAPFILEFAIL:
                return ErrorCode::ErrorMapFileFail;
            case DIERR_MOREDATA:
                return ErrorCode::ErrorMoreData;
            case DIERR_NOAGGREGATION:
                return ErrorCode::ErrorNoAggregation;
            case DIERR_NOINTERFACE:
                return ErrorCode::ErrorNoInterface;
            case DIERR_NOTACQUIRED:
                return ErrorCode::ErrorNotAcquired;
            case DIERR_NOTBUFFERED:
                return ErrorCode::ErrorNotBuffered;
            case DIERR_NOTDOWNLOADED:
                return ErrorCode::ErrorNotDownloaded;
            case DIERR_NOTEXCLUSIVEACQUIRED:
                return ErrorCode::ErrorNotExclusiveAcquired;
            case DIERR_NOTFOUND:
                return ErrorCode::ErrorNotfound;
            case DIERR_NOTINITIALIZED:
                return ErrorCode::ErrorNotInitialized;
            case DIERR_OLDDIRECTINPUTVERSION:
                return ErrorCode::ErrorOldDirectInputvVersion;
            case DIERR_OUTOFMEMORY:
                return ErrorCode::ErrorOutOfMemory;
            case DIERR_REPORTFULL:
                return ErrorCode::ErrorReportFull;
            case DIERR_UNPLUGGED:
                return ErrorCode::ErrorUnplugged;
            case DIERR_UNSUPPORTED:
                return ErrorCode::ErrorUnsupported;
            case E_HANDLE:
                return ErrorCode::ErrorHandle;
            case E_PENDING:
                return ErrorCode::ErrorPending;
            case E_POINTER:
                return ErrorCode::ErrorPointer;
            
            default:
                return static_cast<ErrorCode>(hr);
        }
    }

    inline const ErrorCategory errorCategory;
}

#endif // OUZEL_INPUT_DIERRORCATEGORY_HPP
