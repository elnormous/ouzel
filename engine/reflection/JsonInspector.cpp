#include "JsonInspector.hpp"
#include "../assets/Asset.hpp"
#include "../scene/Component.hpp"

namespace fluxion::reflection
{
    void serializeComponent(scene::Component& component, json::Value& out)
    {
        JsonWriter writer{out};
        component.inspect(writer);
    }

    void deserializeComponent(scene::Component& component, const json::Value& in)
    {
        JsonReader reader{in};
        component.inspect(reader);
    }


    namespace
    {
        json::Value makeArrayOfFloats(const float* data, std::size_t count)
        {
            json::Value arr;
            arr = json::Array{};
            for (std::size_t i = 0; i < count; ++i)
                arr.pushBack(json::Value{static_cast<double>(data[i])});
            return arr;
        }

        // Returns true if `arr` is a JSON array of length `count` whose
        // elements are all numeric (int or double); fills `out` on success
        // and leaves it untouched on any mismatch.
        bool readFloatArray(const json::Value& arr, float* out, std::size_t count)
        {
            if (!arr.is<json::Array>()) return false;
            if (arr.getSize() != count) return false;
            for (std::size_t i = 0; i < count; ++i)
                if (!arr[i].is<float>()) return false;
            for (std::size_t i = 0; i < count; ++i)
                out[i] = arr[i].as<float>();
            return true;
        }
    }

    // ---- JsonWriter -------------------------------------------------------

    JsonWriter::JsonWriter(json::Value& root)
    {
        if (!root.is<json::Object>()) root = json::Object{};
        stack_.push_back(&root);
    }

    void JsonWriter::floatField(const char* label, float& value)
    {
        current()[label] = static_cast<double>(value);
    }

    void JsonWriter::floatRangeField(const char* label, float& value,
                                     float /*minValue*/, float /*maxValue*/)
    {
        // Range is editor-only metadata; the on-disk representation is
        // identical to a plain float. Validation on load is a separate
        // concern (and shouldn't silently clamp without warning).
        current()[label] = static_cast<double>(value);
    }

    void JsonWriter::intField(const char* label, int& value)
    {
        current()[label] = static_cast<std::int64_t>(value);
    }

    void JsonWriter::intRangeField(const char* label, int& value,
                                   int /*minValue*/, int /*maxValue*/)
    {
        current()[label] = static_cast<std::int64_t>(value);
    }

    void JsonWriter::boolField(const char* label, bool& value)
    {
        current()[label] = value;
    }

    void JsonWriter::stringField(const char* label, std::string& value)
    {
        current()[label] = value;
    }

    void JsonWriter::colorField(const char* label, math::Color& value)
    {
        // Persist as 4 floats in linear 0..1 range, matching the way the
        // editor presents it. Reading back rounds to bytes — a tiny
        // round-trip drift that is acceptable for a colour picker.
        const float rgba[4]{value.v[0] / 255.0F, value.v[1] / 255.0F,
                            value.v[2] / 255.0F, value.v[3] / 255.0F};
        current()[label] = makeArrayOfFloats(rgba, 4);
    }

    void JsonWriter::vector2Field(const char* label, math::Vector<float, 2>& value)
    {
        current()[label] = makeArrayOfFloats(value.v, 2);
    }

    void JsonWriter::vector3Field(const char* label, math::Vector<float, 3>& value)
    {
        current()[label] = makeArrayOfFloats(value.v, 3);
    }

    void JsonWriter::vector4Field(const char* label, math::Vector<float, 4>& value)
    {
        current()[label] = makeArrayOfFloats(value.v, 4);
    }

    void JsonWriter::enumField(const char* label, int& value,
                               const char* const* options, std::size_t count)
    {
        // Persist as the option NAME, not the raw int. This way, inserting
        // or reordering enum values in source code does not silently change
        // the meaning of saved scenes.
        if (value >= 0 && static_cast<std::size_t>(value) < count)
            current()[label] = std::string{options[static_cast<std::size_t>(value)]};
        else
            current()[label] = static_cast<std::int64_t>(value);
    }

    void JsonWriter::assetField(const char* label, assets::AssetReference& value)
    {
        // Persisted as { "id": "<uuid>", "type": "<typeName>" } so the
        // type round-trips alongside the id — handy when scene files
        // outlive the running editor and the AssetDatabase isn't
        // around to tell us what kind of asset the id should resolve
        // to. A null id encodes "no asset assigned" and produces
        // { "id": "00000000-...-...", "type": "<...>" }.
        json::Value sub;
        sub = json::Object{};
        sub["id"] = value.id.toString();
        sub["type"] = std::string{assets::typeName(value.type)};
        current()[label] = sub;
    }

    void JsonWriter::beginGroup(const char* label)
    {
        json::Value sub;
        sub = json::Object{};
        current()[label] = sub;
        stack_.push_back(&current()[label]);
    }

    void JsonWriter::endGroup()
    {
        if (stack_.size() > 1) stack_.pop_back();
    }

    // ---- JsonReader -------------------------------------------------------

    JsonReader::JsonReader(const json::Value& root)
    {
        stack_.push_back(root.is<json::Object>() ? &root : nullptr);
    }

    const json::Value* JsonReader::find(const char* label) const noexcept
    {
        if (stack_.empty() || stack_.back() == nullptr) return nullptr;
        const json::Value& obj = *stack_.back();
        if (!obj.hasMember(label)) return nullptr;
        return &obj[label];
    }

    void JsonReader::floatField(const char* label, float& value)
    {
        if (auto* v = find(label); v && v->is<float>()) value = v->as<float>();
    }

    void JsonReader::floatRangeField(const char* label, float& value,
                                     float /*minValue*/, float /*maxValue*/)
    {
        floatField(label, value);
    }

    void JsonReader::intField(const char* label, int& value)
    {
        if (auto* v = find(label); v && v->is<int>()) value = v->as<int>();
    }

    void JsonReader::intRangeField(const char* label, int& value,
                                   int /*minValue*/, int /*maxValue*/)
    {
        intField(label, value);
    }

    void JsonReader::boolField(const char* label, bool& value)
    {
        if (auto* v = find(label); v && v->is<bool>()) value = v->as<bool>();
    }

    void JsonReader::stringField(const char* label, std::string& value)
    {
        if (auto* v = find(label); v && v->is<std::string>())
            value = v->as<std::string>();
    }

    void JsonReader::colorField(const char* label, math::Color& value)
    {
        const auto* v = find(label);
        if (!v) return;
        float rgba[4]{value.v[0] / 255.0F, value.v[1] / 255.0F,
                      value.v[2] / 255.0F, value.v[3] / 255.0F};
        if (!readFloatArray(*v, rgba, 4)) return;
        for (int i = 0; i < 4; ++i)
        {
            const float c = rgba[i] < 0.0F ? 0.0F : (rgba[i] > 1.0F ? 1.0F : rgba[i]);
            value.v[static_cast<std::size_t>(i)] = static_cast<std::uint8_t>(c * 255.0F);
        }
    }

    void JsonReader::vector2Field(const char* label, math::Vector<float, 2>& value)
    {
        if (auto* v = find(label)) readFloatArray(*v, value.v, 2);
    }

    void JsonReader::vector3Field(const char* label, math::Vector<float, 3>& value)
    {
        if (auto* v = find(label)) readFloatArray(*v, value.v, 3);
    }

    void JsonReader::vector4Field(const char* label, math::Vector<float, 4>& value)
    {
        if (auto* v = find(label)) readFloatArray(*v, value.v, 4);
    }

    void JsonReader::enumField(const char* label, int& value,
                               const char* const* options, std::size_t count)
    {
        const auto* v = find(label);
        if (!v) return;
        if (v->is<std::string>())
        {
            const auto& name = v->as<std::string>();
            for (std::size_t i = 0; i < count; ++i)
                if (name == options[i]) { value = static_cast<int>(i); return; }
            // Name not in the current option set: leave value at its
            // current default; this is the forwards-compat case where a
            // newer file references an enum entry the running binary
            // does not yet know about.
        }
        else if (v->is<int>())
        {
            // Backwards-compat for anyone who saved as an integer.
            value = v->as<int>();
        }
    }

    void JsonReader::assetField(const char* label, assets::AssetReference& value)
    {
        // Tolerant on shape: missing object, missing id, or unparseable
        // id all leave the AssetReference at its current default. This
        // matches the rest of JsonReader's "old schema is fine" policy
        // — a scene saved before AssetReference existed simply has no
        // entry for this label, and the component sees value unchanged.
        const auto* v = find(label);
        if (!v || !v->is<json::Object>()) return;

        if (v->hasMember("id") && (*v)["id"].is<std::string>())
            if (auto parsed = Uuid::parse((*v)["id"].as<std::string>()))
                value.id = *parsed;

        if (v->hasMember("type") && (*v)["type"].is<std::string>())
            if (auto parsedType = assets::parseTypeName((*v)["type"].as<std::string>()))
                value.type = *parsedType;
    }

    void JsonReader::beginGroup(const char* label)
    {
        const auto* v = find(label);
        stack_.push_back((v && v->is<json::Object>()) ? v : nullptr);
    }

    void JsonReader::endGroup()
    {
        if (stack_.size() > 1) stack_.pop_back();
    }
}
