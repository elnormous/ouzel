// Copyright 2015-2020 Elviss Strazdins. All rights reserved.

#ifndef OUZEL_CORE_SYSTEM_HPP
#define OUZEL_CORE_SYSTEM_HPP

namespace ouzel::core
{
    class System
    {
    public:
        class Command final
        {
        public:
            enum class Type
            {
                setScreensaverEnabled,
                openUrl
            };
        };

        class Event final
        {
        public:
            enum class Type
            {
                launch,
                deviceOrientationChange,
                lowMemory,
                openFile
            };
        };

        System();
        virtual ~System() = default;
    };
}

#endif // OUZEL_CORE_SYSTEM_HPP
