#ifndef FLUXION_SCENE_SORTINGLAYERS_HPP
#define FLUXION_SCENE_SORTINGLAYERS_HPP

#include <cstddef>
#include <string>
#include <string_view>
#include <vector>

namespace fluxion::scene
{
    // Project-wide sorting-layer registry.
    //
    // Renderer components carry a sortingLayer name + an orderInLayer
    // int (see Component). When the engine sorts a draw queue it
    // resolves the name to an index in this list — names that don't
    // match the registry sort as the "Default" index, so a scene
    // saved with a future layer name still loads in an older binary
    // without crashing.
    //
    // The list is stored as a std::vector<std::string> so the editor
    // can reorder, rename or extend it. The scene serializer (phase
    // 12-future) round-trips this list in the scene JSON's root,
    // keeping each project's ordering self-contained.
    //
    // Threading: read-only callers (renderer sort) are fine on the
    // render thread. Mutation is editor-only and runs on the main
    // thread; callers must not interleave setNames() with a draw.
    namespace sortingLayers
    {
        // Names of all known sorting layers, in z-order (back-to-front).
        // Defaults to {"Background", "Default", "Foreground", "UI"}
        // — Unity's pattern, which keeps newcomers oriented.
        [[nodiscard]] const std::vector<std::string>& names();

        // Replace the registry. Called by the scene loader when it
        // finds a "sorting_layers" array at the JSON root, and by
        // editor UI when the user reorders or renames.
        void setNames(std::vector<std::string> newNames);

        // Resolve a name to an index. Returns indexOfDefault() when
        // the name isn't in the registry — same forwards-compat
        // policy the rest of the engine follows for unknown enum
        // values.
        [[nodiscard]] std::size_t indexOf(std::string_view name) noexcept;

        // Index of "Default" in the registry, or 0 if a project
        // explicitly removed the Default name. Used as the sort
        // fallback for unknown names.
        [[nodiscard]] std::size_t indexOfDefault() noexcept;
    }
}

#endif // FLUXION_SCENE_SORTINGLAYERS_HPP
