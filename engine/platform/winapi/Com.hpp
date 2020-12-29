// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_PLATFORM_WINAPI_COM_HPP
#define OUZEL_PLATFORM_WINAPI_COM_HPP

#include <system_error>
#include <Combaseapi.h>

namespace ouzel::platform::winapi
{
    class Com final
    {
    public:
        Com()
        {
            if (const auto hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED); FAILED(hr))
                throw std::system_error(hr, std::system_category(), "Failed to initialize COM");

            initialized = true;
        }

        ~Com()
        {
            if (initialized) CoUninitialize();
        }

        Com(Com&& other) noexcept :
            initialized(other.initialized)
        {
            other.initialized = false;
        }

        Com& operator=(Com&& other) noexcept
        {
            if (&other == this) return *this;
            if (initialized) CoUninitialize();
            initialized = other.initialized;
            other.initialized = false;
            return *this;
        }

    private:
        bool initialized = false;
    };
}

#endif // OUZEL_PLATFORM_WINAPI_COM_HPP
