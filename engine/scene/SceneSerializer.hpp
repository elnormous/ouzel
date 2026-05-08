#ifndef FLUXION_SCENE_SCENESERIALIZER_HPP
#define FLUXION_SCENE_SCENESERIALIZER_HPP

#include <memory>
#include <string>
#include "../formats/Json.hpp"
#include "../storage/Path.hpp"

namespace fluxion::scene
{
    class Actor;
    class Component;
    class Layer;
    class Scene;

    // Cross-platform Scene <-> JSON / file conversion.
    //
    // File format (version 1):
    //   {
    //     "fluxion_scene_version": 1,
    //     "layers": [
    //       {
    //         "actors": [ <actor-object>, ... ]
    //       }, ...
    //     ]
    //   }
    //
    //   <actor-object> = {
    //     "id":       "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx",
    //     "name":     "...",
    //     "position": [x, y, z],
    //     "rotation": [x, y, z, w],   // quaternion
    //     "scale":    [x, y, z],
    //     "opacity":  1.0,
    //     "flipX":    false,
    //     "flipY":    false,
    //     "pickable": false,
    //     "hidden":   false,
    //     "order":    0,
    //     "components": [ { "type": "scene.SpriteRenderer", "properties": {...} }, ... ],
    //     "children":   [ <actor-object>, ... ]
    //   }
    //
    // Compatibility policy:
    //   * Loaders accept newer-than-known fields and ignore them; missing
    //     fields keep their constructor defaults (the same policy as the
    //     JsonReader Inspector).
    //   * The top-level "fluxion_scene_version" is hard-checked: an
    //     unknown major version throws SceneSerializerError, refusing to
    //     guess the schema.
    //   * Components whose "type" is not in the TypeRegistry are silently
    //     skipped with a warning logged; the rest of the scene loads.
    //
    // Threading:
    //   * Pure CPU work; safe to call from any thread that owns the Scene.
    //   * Loading mutates the Scene; the caller must ensure no render
    //     pass is reading it concurrently (typically: pause the engine
    //     before loadFromFile, resume after).
    namespace serialization
    {
        constexpr int currentSchemaVersion = 1;

        struct SerializerError : public std::runtime_error
        {
            using std::runtime_error::runtime_error;
        };

        json::Value componentToJson(const Component& component);
        json::Value actorToJson(const Actor& actor);
        json::Value layerToJson(const Layer& layer);
        json::Value sceneToJson(const Scene& scene);

        // Component creation goes through TypeRegistry by default. Returns
        // null if the type is unknown — caller chooses to warn or error.
        std::unique_ptr<Component> componentFromJson(const json::Value& obj);

        // Constructs a fresh Actor (with restored id, name, transform)
        // and recursively all of its children + components.
        std::unique_ptr<Actor> actorFromJson(const json::Value& obj);

        // Populates an EXISTING layer with actors from the JSON. We do not
        // construct Layers from JSON because Layer carries engine-bound
        // resources (cameras, render-pass state) that the caller is
        // expected to set up before loading.
        void layerFromJson(Layer& layer, const json::Value& obj);

        // Populates an EXISTING scene. If the scene already has layers,
        // the loaded layer N goes into the scene's layer N; if there are
        // more loaded layers than existing ones, the extras are ignored
        // (with a logged warning). This keeps Layer ownership / construction
        // out of the serializer's hands.
        //
        // Throws SerializerError if the schema version is unknown or the
        // root is malformed.
        void sceneFromJson(Scene& scene, const json::Value& root);

        // File I/O wrappers. saveToFile uses the cross-platform atomic
        // writer (storage::AtomicFile); loadFromFile reads the bytes and
        // delegates to sceneFromJson. Both throw std::system_error on
        // I/O failure and SerializerError on schema problems.
        void saveToFile(const Scene& scene, const storage::Path& path);
        void loadFromFile(Scene& scene, const storage::Path& path);
    }
}

#endif // FLUXION_SCENE_SCENESERIALIZER_HPP
