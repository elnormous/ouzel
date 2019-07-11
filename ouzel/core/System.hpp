// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_SYSTEM_HPP
#define OUZEL_CORE_SYSTEM_HPP

namespace ouzel
{
    class System
    {
    public:
        class Command final
        {
        public:
            enum class Type
            {
                SetScreensaverEnabled,
                OpenUrl
            };
        };

        class Event
        {
        public:
            enum class Type
            {
                Launch,
                DeviceOrientationChange,
                LowMemory,
                OpenFile
            };

        };

        System();
        virtual ~System() = default;
    };
}

#endif // OUZEL_CORE_SYSTEM_HPP
