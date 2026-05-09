#include "SkinnedMeshRenderer.hpp"
#include "../core/Engine.hpp"
#include "../reflection/Inspector.hpp"

namespace fluxion::scene
{
    FLUXION_COMPONENT_REGISTER(SkinnedMeshRenderer)

    SkinnedMeshRenderer::SkinnedMeshRenderer()
    {
    }

    SkinnedMeshRenderer::SkinnedMeshRenderer(const SkinnedMeshData& meshData)
    {
        init(meshData);
    }

    void SkinnedMeshRenderer::init(const SkinnedMeshData& meshData)
    {
        boundingBox = meshData.boundingBox;
        material = meshData.material;
    }

    void SkinnedMeshRenderer::draw(const math::Matrix<float, 4>& transformMatrix,
                                   float opacity,
                                   const math::Matrix<float, 4>& renderViewProjection,
                                   bool wireframe)
    {
        Component::draw(transformMatrix,
                        opacity,
                        renderViewProjection,
                        wireframe);
    }

    void SkinnedMeshRenderer::inspect(reflection::Inspector& inspector)
    {
        bool hiddenCopy = isHidden();
        inspector.boolField("Hidden", hiddenCopy);
        if (hiddenCopy != isHidden()) setHidden(hiddenCopy);

        // Material is a shared_ptr; once the asset browser lands
        // (phase 11) this becomes an asset-reference field. For now
        // the inspector just shows the load status.
        inspector.readOnlyText("Material",
                               material ? "<assigned>" : "(none)");
    }
}
