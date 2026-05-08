#include "SceneIO.hpp"

#include <cstdint>
#include "../engine/formats/Json.hpp"
#include "../engine/scene/Layer.hpp"
#include "../engine/scene/SceneSerializer.hpp"
#include "../engine/storage/AtomicFile.hpp"
#include "../engine/storage/Path.hpp"

namespace fluxion::editor::sceneIO
{
    namespace
    {
        // Schema version probe identical to the engine's sceneFromJson.
        // Re-implemented here (instead of going through sceneFromJson)
        // because the editor wants to populate ONE specific layer
        // rather than mapping the JSON layers array onto a Scene's
        // pre-existing layer list. Letting the engine helper see our
        // 2-layer Scene would either restore content into editorLayer_
        // (wrong) or silently drop the second layer (also wrong).
        bool hasInt(const json::Value& v, const std::string& key)
        {
            return v.is<json::Object>() && v.hasMember(key) && v[key].is<int>();
        }
        bool hasArray(const json::Value& v, const std::string& key)
        {
            return v.is<json::Object>() && v.hasMember(key) && v[key].is<json::Array>();
        }
    }

    void saveUserScene(const scene::Layer& userLayer,
                       const storage::Path& path)
    {
        json::Value root;
        root = json::Object{};
        root["fluxion_scene_version"] =
            static_cast<std::int64_t>(scene::serialization::currentSchemaVersion);

        json::Value layers;
        layers = json::Array{};
        layers.pushBack(scene::serialization::layerToJson(userLayer));
        root["layers"] = layers;

        const auto text = json::encode(root, /*whitespace*/ true);
        storage::atomicWriteFile(path, text);
    }

    void loadUserScene(scene::Layer& userLayer,
                       const storage::Path& path)
    {
        const auto text = storage::readTextFile(path);
        const auto root = json::parse(text);

        if (!root.is<json::Object>())
            throw scene::serialization::SerializerError{
                "Scene root must be an object"};
        if (!hasInt(root, "fluxion_scene_version"))
            throw scene::serialization::SerializerError{
                "Missing fluxion_scene_version"};
        const auto v = root["fluxion_scene_version"].as<int>();
        if (v != scene::serialization::currentSchemaVersion)
            throw scene::serialization::SerializerError{
                "Unsupported scene schema version " + std::to_string(v)};

        if (!hasArray(root, "layers")) return;
        const auto& layers = root["layers"].as<json::Array>();
        if (layers.empty()) return;

        // Always feed the first JSON layer into the user layer. Files
        // saved by the editor only ever contain a single layer; if a
        // future multi-layer feature lands the editor will iterate
        // over its own layer list and pair them up explicitly.
        scene::serialization::layerFromJson(userLayer, layers[0]);
    }
}
