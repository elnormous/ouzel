#include "TypeRegistry.hpp"
#include <algorithm>
#include "../scene/Component.hpp"
#include "../utils/Log.hpp"

namespace fluxion::reflection
{
    TypeRegistry& TypeRegistry::instance()
    {
        static TypeRegistry registry;
        return registry;
    }

    void TypeRegistry::registerType(std::string_view name, Factory factory)
    {
        const std::string key{name};
        const auto [it, inserted] = types_.try_emplace(key, TypeInfo{key, std::move(factory)});
        if (!inserted)
            log(Log::Level::warning) << "Type " << key << " already registered, ignoring duplicate";
    }

    bool TypeRegistry::has(std::string_view name) const noexcept
    {
        return types_.find(std::string{name}) != types_.end();
    }

    std::unique_ptr<scene::Component> TypeRegistry::create(std::string_view name) const
    {
        const auto it = types_.find(std::string{name});
        if (it == types_.end())
        {
            log(Log::Level::warning) << "TypeRegistry: unknown type " << name;
            return {};
        }
        return it->second.factory();
    }

    std::vector<std::string> TypeRegistry::listTypes() const
    {
        std::vector<std::string> result;
        result.reserve(types_.size());
        for (const auto& [key, info] : types_) result.push_back(info.name);
        std::sort(result.begin(), result.end());
        return result;
    }
}
