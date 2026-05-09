#include "SceneIO.hpp"

#include <algorithm>
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

    namespace
    {
        // Drop editor-internal actors from a serialised layer's
        // "actors" array. The convention is that any actor whose
        // name starts with "<" (e.g. "<viewport camera>") is owned
        // by the editor's panels rather than the user — saving them
        // and re-loading would duplicate the panel's instance, and
        // re-anchoring on load is fragile (the new actor's pointer
        // wouldn't match the panel's exclude-actor pointer until the
        // scene-tree filter's name-based check kicks in too).
        // Keeps the .flux file free of editor scaffolding.
        void stripEditorActors(json::Value& layerJson)
        {
            if (!layerJson.is<json::Object>()) return;
            if (!layerJson.hasMember("actors")) return;
            auto& actors = layerJson["actors"];
            if (!actors.is<json::Array>()) return;

            auto& arr = actors.as<json::Array>();
            arr.erase(std::remove_if(arr.begin(), arr.end(),
                [](const json::Value& a) {
                    if (!a.is<json::Object>()) return false;
                    if (!a.hasMember("name")) return false;
                    if (!a["name"].is<std::string>()) return false;
                    const auto& n = a["name"].as<std::string>();
                    return !n.empty() && n.front() == '<';
                }), arr.end());
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
        auto layerJson = scene::serialization::layerToJson(userLayer);
        stripEditorActors(layerJson);
        layers.pushBack(std::move(layerJson));
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
