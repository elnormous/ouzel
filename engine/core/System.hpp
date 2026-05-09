#ifndef FLUXION_CORE_SYSTEM_HPP
#define FLUXION_CORE_SYSTEM_HPP

#include <string>
#include <vector>

namespace fluxion::core
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

        System(const std::vector<std::string>& initArgs);
        virtual ~System() = default;

        auto& getArgs() const noexcept { return args; }

    private:
        std::vector<std::string> args;
    };
}

#endif // FLUXION_CORE_SYSTEM_HPP
