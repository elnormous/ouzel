#ifndef FLUXION_REFLECTION_TYPEREGISTRY_HPP
#define FLUXION_REFLECTION_TYPEREGISTRY_HPP

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace fluxion::scene
{
    class Component;
}

namespace fluxion::reflection
{
    // Process-wide registry mapping a stable, user-facing type name (e.g.
    // "scene.SpriteRenderer") to a factory that produces a default-constructed
    // instance. Used by the editor for "Add Component" menus and by the
    // deserializer (phase 3) to materialize a scene from a JSON document.
    //
    // Registration is one-time, normally driven by a static initializer
    // declared via the FLUXION_COMPONENT macro in each component's .cpp,
    // so the runtime cost of reflection is only paid in editor / loader
    // code paths.
    class TypeRegistry
    {
    public:
        using Factory = std::function<std::unique_ptr<scene::Component>()>;

        struct TypeInfo
        {
            std::string name;
            Factory factory;
        };

        static TypeRegistry& instance();

        // Idempotent on the same (name, factory) pair; logs a warning if a
        // different factory is being registered for an existing name.
        void registerType(std::string_view name, Factory factory);

        [[nodiscard]] bool has(std::string_view name) const noexcept;
        [[nodiscard]] std::unique_ptr<scene::Component> create(std::string_view name) const;

        // Sorted alphabetically; safe to call from any thread (returns a copy).
        [[nodiscard]] std::vector<std::string> listTypes() const;

    private:
        TypeRegistry() = default;
        std::unordered_map<std::string, TypeInfo> types_;
    };

    // RAII-style auto-registrar; instantiate as a `static` in a component's
    // .cpp via the FLUXION_COMPONENT macro to register at static-init time.
    struct AutoRegistrar
    {
        AutoRegistrar(std::string_view name, TypeRegistry::Factory factory)
        {
            TypeRegistry::instance().registerType(name, std::move(factory));
        }
    };
}

#endif // FLUXION_REFLECTION_TYPEREGISTRY_HPP
