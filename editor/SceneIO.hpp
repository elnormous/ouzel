#ifndef FLUXION_EDITOR_SCENEIO_HPP
#define FLUXION_EDITOR_SCENEIO_HPP

#include <string>

namespace fluxion::scene
{
    class Layer;
}

namespace fluxion::storage
{
    class Path;
}

namespace fluxion::editor::sceneIO
{
    // Editor-side save/load that operates on the user's working
    // layer ONLY. The editor's internal helper layer (which holds
    // the back-buffer-clear camera) and the viewport panel's camera
    // actor never make it into the file — they're editor
    // infrastructure, not user-authored content.
    //
    // The on-disk schema matches engine sceneToJson exactly: a top-
    // level object with `fluxion_scene_version` and a `layers` array
    // of one entry. A runtime application can therefore load these
    // files via scene::serialization::loadFromFile and get just the
    // user content without any editor cruft.
    //
    // Cross-platform: storage::atomicWriteFile and storage::readTextFile
    // already abstract the per-OS file APIs (Win32 CreateFile +
    // MoveFileEx, POSIX open + rename + fsync), so the editor needs
    // no extra plumbing here.

    // Throws scene::serialization::SerializerError on schema problems
    // and std::system_error on I/O failure (matching the engine helper
    // it composes with).
    void saveUserScene(const scene::Layer& userLayer,
                       const storage::Path& path);

    // Replaces the user layer's contents with the actors stored in
    // `path`. Existing children are dropped (the engine's
    // layerFromJson clears before it populates).
    void loadUserScene(scene::Layer& userLayer,
                       const storage::Path& path);
}

#endif // FLUXION_EDITOR_SCENEIO_HPP
