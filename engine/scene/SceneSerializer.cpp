#include "SceneSerializer.hpp"

#include "Actor.hpp"
#include "Component.hpp"
#include "Layer.hpp"
#include "Scene.hpp"
#include "../reflection/JsonInspector.hpp"
#include "../reflection/TypeRegistry.hpp"
#include "../storage/AtomicFile.hpp"
#include "../utils/Log.hpp"

namespace fluxion::scene::serialization
{
    namespace
    {
        json::Value floatArray(const float* v, std::size_t n)
        {
            json::Value arr;
            arr = json::Array{};
            for (std::size_t i = 0; i < n; ++i)
                arr.pushBack(json::Value{static_cast<double>(v[i])});
            return arr;
        }

        bool readFloatArray(const json::Value& v, float* out, std::size_t n)
        {
            if (!v.is<json::Array>() || v.getSize() != n) return false;
            for (std::size_t i = 0; i < n; ++i)
                if (!v[i].is<float>()) return false;
            for (std::size_t i = 0; i < n; ++i) out[i] = v[i].as<float>();
            return true;
        }

        // Forward decl for recursion (children).
        void writeActorBody(const Actor& actor, json::Value& obj);
    }

    json::Value componentToJson(const Component& component)
    {
        json::Value obj;
        obj = json::Object{};
        obj["type"] = std::string{component.typeName()};
        json::Value properties;
        properties = json::Object{};
        // const_cast: inspect() takes a non-const Component because the
        // Inspector callbacks take non-const refs to the component's
        // fields (so the same body can also load). For serialization we
        // are only reading, but we have to satisfy the type signature.
        reflection::serializeComponent(const_cast<Component&>(component), properties);
        obj["properties"] = properties;
        return obj;
    }

    std::unique_ptr<Component> componentFromJson(const json::Value& obj)
    {
        if (!obj.is<json::Object>() || !obj.hasMember("type")) return {};
        const auto& typeNode = obj["type"];
        if (!typeNode.is<std::string>()) return {};
        const auto& typeName = typeNode.as<std::string>();

        auto comp = reflection::TypeRegistry::instance().create(typeName);
        if (!comp)
        {
            fluxion::log(Log::Level::warning)
                << "SceneSerializer: skipping unknown component type \"" << typeName << '"';
            return {};
        }
        if (obj.hasMember("properties") && obj["properties"].is<json::Object>())
            reflection::deserializeComponent(*comp, obj["properties"]);
        return comp;
    }

    namespace
    {
        void writeActorBody(const Actor& actor, json::Value& obj)
        {
            obj["id"] = actor.getId().toString();
            obj["name"] = actor.getName();
            obj["position"] = floatArray(actor.getPosition().v, 3);
            obj["rotation"] = floatArray(actor.getRotation().v, 4);
            obj["scale"] = floatArray(actor.getScale().v, 3);
            obj["opacity"] = static_cast<double>(actor.getOpacity());
            obj["flipX"] = actor.getFlipX();
            obj["flipY"] = actor.getFlipY();
            obj["pickable"] = actor.isPickable();
            obj["hidden"] = actor.isHidden();
            obj["order"] = static_cast<std::int64_t>(actor.getOrder());

            json::Value comps;
            comps = json::Array{};
            for (const auto* c : actor.getComponents())
                if (c) comps.pushBack(componentToJson(*c));
            obj["components"] = comps;

            json::Value kids;
            kids = json::Array{};
            for (const auto* child : actor.getChildren())
                if (child) kids.pushBack(actorToJson(*child));
            obj["children"] = kids;
        }
    }

    json::Value actorToJson(const Actor& actor)
    {
        json::Value obj;
        obj = json::Object{};
        writeActorBody(actor, obj);
        return obj;
    }

    namespace
    {
        // Returns true if the Object has a member named `key` whose value
        // satisfies `is<T>()`. Used to keep the deserializer call sites
        // free of repetitive null/type guards.
        template <class T>
        bool has(const json::Value& obj, const char* key)
        {
            return obj.is<json::Object>() && obj.hasMember(key) && obj[key].is<T>();
        }

        void readActorBody(Actor& actor, const json::Value& obj)
        {
            if (has<std::string>(obj, "id"))
            {
                if (auto parsed = Uuid::parse(obj["id"].as<std::string>()))
                    actor.setId(*parsed);
            }
            if (has<std::string>(obj, "name"))
                actor.setName(obj["name"].as<std::string>());

            if (obj.hasMember("position"))
            {
                math::Vector<float, 3> pos = actor.getPosition();
                if (readFloatArray(obj["position"], pos.v, 3))
                    actor.setPosition(pos);
            }
            if (obj.hasMember("rotation"))
            {
                math::Quaternion<float> rot = actor.getRotation();
                if (readFloatArray(obj["rotation"], rot.v, 4))
                    actor.setRotation(rot);
            }
            if (obj.hasMember("scale"))
            {
                math::Vector<float, 3> sc = actor.getScale();
                if (readFloatArray(obj["scale"], sc.v, 3))
                    actor.setScale(sc);
            }
            if (has<float>(obj, "opacity"))
                actor.setOpacity(obj["opacity"].as<float>());
            if (has<bool>(obj, "flipX"))
                actor.setFlipX(obj["flipX"].as<bool>());
            if (has<bool>(obj, "flipY"))
                actor.setFlipY(obj["flipY"].as<bool>());
            if (has<bool>(obj, "pickable"))
                actor.setPickable(obj["pickable"].as<bool>());
            if (has<bool>(obj, "hidden"))
                actor.setHidden(obj["hidden"].as<bool>());
            if (has<int>(obj, "order"))
                actor.setOrder(static_cast<Actor::Order>(obj["order"].as<int>()));

            if (has<json::Array>(obj, "components"))
            {
                const auto& arr = obj["components"].as<json::Array>();
                for (const auto& c : arr)
                {
                    auto comp = componentFromJson(c);
                    if (comp) actor.addComponent(std::move(comp));
                }
            }

            if (has<json::Array>(obj, "children"))
            {
                const auto& arr = obj["children"].as<json::Array>();
                for (const auto& c : arr)
                {
                    auto child = actorFromJson(c);
                    if (child) actor.addChild(std::move(child));
                }
            }
        }
    }

    std::unique_ptr<Actor> actorFromJson(const json::Value& obj)
    {
        auto actor = std::make_unique<Actor>();
        readActorBody(*actor, obj);
        return actor;
    }

    json::Value layerToJson(const Layer& layer)
    {
        json::Value obj;
        obj = json::Object{};
        json::Value actors;
        actors = json::Array{};
        for (const auto* child : layer.getChildren())
            if (child) actors.pushBack(actorToJson(*child));
        obj["actors"] = actors;
        return obj;
    }

    void layerFromJson(Layer& layer, const json::Value& obj)
    {
        if (!has<json::Array>(obj, "actors")) return;
        const auto& arr = obj["actors"].as<json::Array>();
        for (const auto& a : arr)
        {
            auto actor = actorFromJson(a);
            if (actor) layer.addChild(std::move(actor));
        }
    }

    json::Value sceneToJson(const Scene& scene)
    {
        json::Value root;
        root = json::Object{};
        root["fluxion_scene_version"] = static_cast<std::int64_t>(currentSchemaVersion);

        json::Value layers;
        layers = json::Array{};
        for (const auto* layer : scene.getLayers())
            if (layer) layers.pushBack(layerToJson(*layer));
        root["layers"] = layers;

        return root;
    }

    void sceneFromJson(Scene& scene, const json::Value& root)
    {
        if (!root.is<json::Object>())
            throw SerializerError{"Scene root must be an object"};
        if (!has<int>(root, "fluxion_scene_version"))
            throw SerializerError{"Missing fluxion_scene_version"};

        const int v = root["fluxion_scene_version"].as<int>();
        if (v != currentSchemaVersion)
            throw SerializerError{"Unsupported scene schema version " + std::to_string(v)};

        if (!has<json::Array>(root, "layers")) return;

        const auto& layers = root["layers"].as<json::Array>();
        const auto& sceneLayers = scene.getLayers();
        const std::size_t commonCount = std::min(layers.size(), sceneLayers.size());

        for (std::size_t i = 0; i < commonCount; ++i)
            layerFromJson(*sceneLayers[i], layers[i]);

        if (layers.size() > sceneLayers.size())
            fluxion::log(Log::Level::warning)
                << "SceneSerializer: file has " << layers.size()
                << " layer(s) but scene only has " << sceneLayers.size()
                << "; extra layers ignored";
    }

    void saveToFile(const Scene& scene, const storage::Path& path)
    {
        const auto root = sceneToJson(scene);
        const auto text = json::encode(root, /*whiteSpaces*/ true);
        storage::atomicWriteFile(path, text);
    }

    void loadFromFile(Scene& scene, const storage::Path& path)
    {
        const auto text = storage::readTextFile(path);
        const auto root = json::parse(text);
        sceneFromJson(scene, root);
    }
}
