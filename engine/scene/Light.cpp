#include <cstdio>
#include "Light.hpp"
#include "Camera.hpp"
#include "Layer.hpp"
#include "../reflection/Inspector.hpp"

namespace fluxion::scene
{
    FLUXION_COMPONENT_REGISTER(Light)

    Light::Light(Type initType):
        type{initType}
    {
    }

    Light::~Light()
    {
        if (layer) layer->removeLight(*this);
    }

    void Light::setLayer(Layer* newLayer)
    {
        if (layer) layer->addLight(*this);

        Component::setLayer(newLayer);

        if (layer) layer->removeLight(*this);
    }

    void Light::inspect(reflection::Inspector& inspector)
    {
        bool hiddenCopy = isHidden();
        inspector.boolField("Hidden", hiddenCopy);
        if (hiddenCopy != isHidden()) setHidden(hiddenCopy);

        static const char* lightTypes[] = {"point", "spot", "directional"};
        int typeIdx = static_cast<int>(type);
        inspector.enumField("Type", typeIdx, lightTypes,
                            sizeof(lightTypes) / sizeof(lightTypes[0]));
        if (typeIdx != static_cast<int>(type))
            type = static_cast<Type>(typeIdx);

        math::Color colorCopy = color;
        inspector.colorField("Color", colorCopy);
        if (colorCopy.v != color.v) color = colorCopy;

        float intensityCopy = intensity;
        inspector.floatField("Intensity", intensityCopy);
        if (intensityCopy != intensity) intensity = intensityCopy;

        // Range applies only to point + spot lights; directional is
        // infinite. We surface it always (no read-only gating) so the
        // value isn't lost when toggling between types — the renderer
        // ignores it for directional.
        float rangeCopy = range;
        inspector.floatField("Range", rangeCopy);
        if (rangeCopy != range) range = rangeCopy;

        // Spot cone half-angle in radians. Same "always-shown" policy
        // as range: keeps the value when the user toggles types.
        float angleCopy = angle;
        inspector.floatField("Angle", angleCopy);
        if (angleCopy != angle) angle = angleCopy;

        // Direction is a quaternion; raw 4-float editing is unfriendly
        // and we don't want to flip the existing actor-rotation idiom.
        // Read-only summary for now; a future inspector can add Euler
        // editing when the same need applies to actor rotation
        // beyond the gizmo.
        char dirSummary[80];
        std::snprintf(dirSummary, sizeof(dirSummary),
                      "(%.2f, %.2f, %.2f, %.2f)",
                      direction.v[0], direction.v[1],
                      direction.v[2], direction.v[3]);
        inspector.readOnlyText("Direction (quat)", dirSummary);
    }
}
