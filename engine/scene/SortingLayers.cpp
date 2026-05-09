#include "SortingLayers.hpp"

#include <utility>

namespace fluxion::scene::sortingLayers
{
    namespace
    {
        // Backing store. Initialised on first access via the
        // function-local static — gives us deterministic order even
        // across translation units that read the registry during
        // their own static initialisation (Cache, Bundle, ...).
        std::vector<std::string>& mutableNames()
        {
            static std::vector<std::string> registry{
                "Background", "Default", "Foreground", "UI"};
            return registry;
        }
    }

    const std::vector<std::string>& names()
    {
        return mutableNames();
    }

    void setNames(std::vector<std::string> newNames)
    {
        mutableNames() = std::move(newNames);
    }

    std::size_t indexOf(std::string_view name) noexcept
    {
        const auto& reg = mutableNames();
        for (std::size_t i = 0; i < reg.size(); ++i)
            if (reg[i] == name) return i;
        return indexOfDefault();
    }

    std::size_t indexOfDefault() noexcept
    {
        const auto& reg = mutableNames();
        for (std::size_t i = 0; i < reg.size(); ++i)
            if (reg[i] == "Default") return i;
        return 0;
    }
}
