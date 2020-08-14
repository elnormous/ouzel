// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_AUDIO_DSERRORCATEGORY_HPP
#define OUZEL_AUDIO_DSERRORCATEGORY_HPP

#include "../../core/Setup.h"

#if OUZEL_COMPILE_DIRECTSOUND

#include <system_error>
#pragma push_macro("WIN32_LEAN_AND_MEAN")
#pragma push_macro("NOMINMAX")
#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#  define NOMINMAX
#endif
#include <dsound.h>
#pragma pop_macro("WIN32_LEAN_AND_MEAN")
#pragma pop_macro("NOMINMAX")

namespace ouzel::audio::directsound
{
    class ErrorCategory final: public std::error_category
    {
    public:
        const char* name() const noexcept final
        {
            return "DirectSound";
        }

        std::string message(int condition) const final
        {
            switch (condition)
            {
                case DSERR_ACCESSDENIED: return "DSERR_ACCESSDENIED";
                case DSERR_ALLOCATED: return "DSERR_ALLOCATED";
                case DSERR_ALREADYINITIALIZED: return "DSERR_ALREADYINITIALIZED";
                case DSERR_BADFORMAT: return "DSERR_BADFORMAT";
                case DSERR_BADSENDBUFFERGUID: return "DSERR_BADSENDBUFFERGUID";
                case DSERR_BUFFERLOST: return "DSERR_BUFFERLOST";
                case DSERR_BUFFERTOOSMALL: return "DSERR_BUFFERTOOSMALL";
                case DSERR_CONTROLUNAVAIL: return "DSERR_CONTROLUNAVAIL";
                case DSERR_DS8_REQUIRED: return "DSERR_DS8_REQUIRED";
                case DSERR_FXUNAVAILABLE: return "DSERR_FXUNAVAILABLE";
                case DSERR_GENERIC: return "DSERR_GENERIC";
                case DSERR_INVALIDCALL: return "DSERR_INVALIDCALL";
                case DSERR_INVALIDPARAM: return "DSERR_INVALIDPARAM";
                case DSERR_NOAGGREGATION: return "DSERR_NOAGGREGATION";
                case DSERR_NODRIVER: return "DSERR_NODRIVER";
                case DSERR_NOINTERFACE: return "DSERR_NOINTERFACE";
                case DSERR_OBJECTNOTFOUND: return "DSERR_OBJECTNOTFOUND";
                case DSERR_OTHERAPPHASPRIO: return "DSERR_OTHERAPPHASPRIO";
                case DSERR_OUTOFMEMORY: return "DSERR_OUTOFMEMORY";
                case DSERR_PRIOLEVELNEEDED: return "DSERR_PRIOLEVELNEEDED";
                case DSERR_SENDLOOP: return "DSERR_SENDLOOP";
                case DSERR_UNINITIALIZED: return "DSERR_UNINITIALIZED";
                case DSERR_UNSUPPORTED: return "DSERR_UNSUPPORTED";
                default: return "Unknown error (" + std::to_string(condition) + ")";
            }
        }
    };
}
#endif

#endif // OUZEL_AUDIO_DSERRORCATEGORY_HPP
