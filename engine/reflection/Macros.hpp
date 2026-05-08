#ifndef FLUXION_REFLECTION_MACROS_HPP
#define FLUXION_REFLECTION_MACROS_HPP

#include "TypeRegistry.hpp"

// Inject the boilerplate that links a Component subclass to the runtime type
// registry: a stable typeName() override and a default-constructing factory
// hook usable by TypeRegistry::create. Place inside the public section of a
// component class declaration (header).
//
// Usage in the header:
//     class SpriteRenderer : public scene::Component
//     {
//     public:
//         FLUXION_COMPONENT_DECL(SpriteRenderer, "scene.SpriteRenderer")
//         ...
//     };
//
// And exactly once in the matching .cpp file, INSIDE the same namespace as
// the class declaration:
//     namespace fluxion::scene {
//         FLUXION_COMPONENT_REGISTER(SpriteRenderer)
//     }
//
// The register macro instantiates a static AutoRegistrar at namespace scope,
// so the type appears in the registry by the time main() runs. It is placed
// inside an anonymous namespace to avoid ODR conflicts when the same class
// is registered from multiple translation units (it isn't normally, but the
// extra safety is free).

#define FLUXION_COMPONENT_DECL(ClassName, TypeName_Literal)                  \
    [[nodiscard]] std::string_view typeName() const noexcept override        \
    {                                                                        \
        return TypeName_Literal;                                             \
    }                                                                        \
    [[nodiscard]] static std::string_view staticTypeName() noexcept          \
    {                                                                        \
        return TypeName_Literal;                                             \
    }                                                                        \
    [[nodiscard]] static std::unique_ptr<::fluxion::scene::Component>        \
    reflectionCreate()                                                       \
    {                                                                        \
        return std::make_unique<ClassName>();                                \
    }

// Naming the registrar at namespace scope (rather than the
// previous anonymous-namespace placement) gives it external linkage,
// which is what callers like the editor's force-link table need to
// reference to keep the linker from dropping the .obj from a static
// libfluxion archive. The volatile bool sibling is the actual
// reference handle: a 1-byte symbol the editor's force-link table
// can read without colliding with the AutoRegistrar's typed name.
#define FLUXION_COMPONENT_REGISTER(ClassName)                                \
    const ::fluxion::reflection::AutoRegistrar                               \
        ClassName##AutoRegistrar{ClassName::staticTypeName(),                \
                                 &ClassName::reflectionCreate};              \
    extern const volatile bool ClassName##ForceLink;                         \
    const volatile bool ClassName##ForceLink = false;

#endif // FLUXION_REFLECTION_MACROS_HPP
