// Ouzel by Elviss Strazdins

#include <cassert>
#include <algorithm>
#include "Camera.hpp"
#include "Actor.hpp"
#include "Layer.hpp"
#include "../core/Engine.hpp"
#include "../graphics/Graphics.hpp"
#include "../graphics/RenderDevice.hpp"

namespace ouzel::scene
{
    Camera::Camera(const Matrix<float, 4>& initProjection):
        projectionMode(ProjectionMode::custom), projection(initProjection)

    {
        calculateViewProjection();
    }

    Camera::Camera(const Size<float, 2>& initTargetContentSize, ScaleMode initScaleMode):
        projectionMode(ProjectionMode::orthographic), targetContentSize(initTargetContentSize),
        scaleMode(initScaleMode)
    {
        calculateViewProjection();
    }

    Camera::Camera(float initFov, float initNearPlane, float initFarPlane):
        projectionMode(ProjectionMode::perspective), fov(initFov),
        nearPlane(initNearPlane), farPlane(initFarPlane)
    {
        calculateViewProjection();
    }

    Camera::~Camera()
    {
        if (layer) layer->removeCamera(*this);
    }

    void Camera::setActor(Actor* newActor)
    {
        Component::setActor(newActor);

        viewProjectionDirty = inverseViewProjectionDirty = true;
    }

    void Camera::setLayer(Layer* newLayer)
    {
        if (layer) layer->removeCamera(*this);

        Component::setLayer(newLayer);

        if (layer) layer->addCamera(*this);
    }

    void Camera::updateTransform()
    {
        Component::updateTransform();

        viewProjectionDirty = inverseViewProjectionDirty = true;
    }

    void Camera::recalculateProjection()
    {
        const Size<std::uint32_t, 2> renderTargetSize = renderTarget ?
            !renderTarget->getColorTextures().empty() ?
                renderTarget->getColorTextures()[0]->getSize() :
                renderTarget->getDepthTexture() ?
                    renderTarget->getDepthTexture()->getSize() :
                    Size<std::uint32_t, 2>() :
            engine->getGraphics()->getSize();

        renderViewport.position.v[0] = renderTargetSize.v[0] * viewport.position.v[0];
        renderViewport.position.v[1] = renderTargetSize.v[1] * viewport.position.v[1];
        renderViewport.size.v[0] = renderTargetSize.v[0] * viewport.size.v[0];
        renderViewport.size.v[1] = renderTargetSize.v[1] * viewport.size.v[1];

        assert(renderViewport.size.v[0] > 0.0F && renderViewport.size.v[1] > 0.0F);

        if (targetContentSize.v[0] > 0.0F && targetContentSize.v[1] > 0.0F)
        {
            contentScale.v[0] = renderViewport.size.v[0] / targetContentSize.v[0];
            contentScale.v[1] = renderViewport.size.v[1] / targetContentSize.v[1];

            switch (scaleMode)
            {
                case ScaleMode::noScale:
                {
                    break;
                }
                case ScaleMode::exactFit:
                {
                    contentScale.v[0] = 1.0F;
                    contentScale.v[1] = 1.0F;
                    break;
                }
                case ScaleMode::noBorder:
                {
                    contentScale.v[0] = contentScale.v[1] = std::max(contentScale.v[0], contentScale.v[1]);
                    break;
                }
                case ScaleMode::showAll:
                {
                    contentScale.v[0] = contentScale.v[1] = std::min(contentScale.v[0], contentScale.v[1]);
                    break;
                }
                default:
                    return;
            }

            contentSize = Size<float, 2>(renderViewport.size.v[0] / contentScale.v[0], renderViewport.size.v[1] / contentScale.v[1]);
            contentPosition = Vector<float, 2>((contentSize.v[0] - targetContentSize.v[0]) / 2.0F,
                                       (contentSize.v[1] - targetContentSize.v[1]) / 2.0F);
        }
        else
        {
            contentScale = Vector<float, 2>{1.0F, 1.0F};
            contentSize = Size<float, 2>(renderViewport.size.v[0], renderViewport.size.v[1]);
            contentPosition = Vector<float, 2>{0.0F, 0.0F};
        }

        switch (projectionMode)
        {
            case ProjectionMode::custom:
                // do nothing
                break;
            case ProjectionMode::orthographic:
                projection.setOrthographic(contentSize.v[0], contentSize.v[1], -1.0F, 1.0F);
                break;
            case ProjectionMode::perspective:
                projection.setPerspective(fov, contentSize.v[0] / contentSize.v[1], nearPlane, farPlane);
                break;
            default:
                return;
        }

        viewProjectionDirty = inverseViewProjectionDirty = true;
    }

    const Matrix<float, 4>& Camera::getViewProjection() const
    {
        if (viewProjectionDirty) calculateViewProjection();

        return viewProjection;
    }

    const Matrix<float, 4>& Camera::getRenderViewProjection() const
    {
        if (viewProjectionDirty) calculateViewProjection();

        return renderViewProjection;
    }

    const Matrix<float, 4>& Camera::getInverseViewProjection() const
    {
        if (inverseViewProjectionDirty)
        {
            inverseViewProjection = getViewProjection();
            inverseViewProjection.invert();

            inverseViewProjectionDirty = false;
        }

        return inverseViewProjection;
    }

    void Camera::calculateViewProjection() const
    {
        if (actor)
        {
            viewProjection = projection * actor->getInverseTransform();

            renderViewProjection = engine->getGraphics()->getDevice()->getProjectionTransform(renderTarget != nullptr) * viewProjection;

            viewProjectionDirty = false;
        }
    }

    Vector<float, 3> Camera::convertNormalizedToWorld(const Vector<float, 2>& normalizedPosition) const
    {
        // convert window normalized to viewport clip position
        auto result = Vector<float, 3>{((normalizedPosition.v[0] - viewport.position.v[0]) / viewport.size.v[0] - 0.5F) * 2.0F,
                               (((1.0F - normalizedPosition.v[1]) - viewport.position.v[1]) / viewport.size.v[1] - 0.5F) * 2.0F,
                               0.0F};

        getInverseViewProjection().transformPoint(result);

        return result;
    }

    Vector<float, 2> Camera::convertWorldToNormalized(const Vector<float, 3>& normalizedPosition) const
    {
        Vector<float, 3> result = normalizedPosition;
        getViewProjection().transformPoint(result);

        // convert viewport clip position to window normalized
        return Vector<float, 2>((result.v[0] / 2.0F + 0.5F) * viewport.size.v[0] + viewport.position.v[0],
                        1.0F - ((result.v[1] / 2.0F + 0.5F) * viewport.size.v[1] + viewport.position.v[1]));
    }

    bool Camera::checkVisibility(const Matrix<float, 4>& boxTransform, const Box<float, 3>& box) const
    {
        if (projectionMode == ProjectionMode::orthographic)
        {
            // calculate center point of the box
            const auto diff = Vector<float, 2>(box.max - box.min);

            // offset the center point, so that it is relative to 0,0
            Vector<float, 3> v3p{
                box.min.v[0] + diff.v[0] / 2.0F,
                box.min.v[1] + diff.v[1] / 2.0F, 0.0F
            };

            // apply local transform to the center point
            boxTransform.transformPoint(v3p);

            // tranform the center to viewport's clip space
            Vector<float, 4> clipPos;
            getViewProjection().transformVector(Vector<float, 4>(v3p.v[0], v3p.v[1], v3p.v[2], 1.0F), clipPos);

            assert(clipPos.v[3] != 0.0F);

            // normalize position of the center point
            const Vector<float, 2> v2p{
                (clipPos.v[0] / clipPos.v[3] + 1.0F) * 0.5F,
                (clipPos.v[1] / clipPos.v[3] + 1.0F) * 0.5F
            };

            // calculate half size
            const Size<float, 2> halfSize(diff.v[0] / 2.0F, diff.v[1] / 2.0F);

            // convert content size to world coordinates
            Size<float, 2> halfWorldSize{
                std::max(std::fabs(halfSize.v[0] * boxTransform.m[0] + halfSize.v[1] * boxTransform.m[4]),
                         std::fabs(halfSize.v[0] * boxTransform.m[0] - halfSize.v[1] * boxTransform.m[4])),
                std::max(std::fabs(halfSize.v[0] * boxTransform.m[1] + halfSize.v[1] * boxTransform.m[5]),
                         std::fabs(halfSize.v[0] * boxTransform.m[1] - halfSize.v[1] * boxTransform.m[5]))
            };

            // scale half size by camera projection to get the size in clip space coordinates
            halfWorldSize.v[0] *= (std::fabs(viewProjection.m[0]) + std::fabs(viewProjection.m[4])) / 2.0F;
            halfWorldSize.v[1] *= (std::fabs(viewProjection.m[1]) + std::fabs(viewProjection.m[5])) / 2.0F;

            // create visible rect in clip space
            const Rect<float> visibleRect(-halfWorldSize.v[0],
                                    -halfWorldSize.v[1],
                                    1.0F + halfWorldSize.v[0] * 2.0F,
                                    1.0F + halfWorldSize.v[1] * 2.0F);

            return visibleRect.containsPoint(v2p);
        }
        else
        {
            const auto modelViewProjection = getViewProjection() * boxTransform;
            const auto frustum = modelViewProjection.getFrustum();
            return frustum.isBoxInside(box);
        }
    }

    void Camera::setViewport(const Rect<float>& newViewport)
    {
        viewport = newViewport;
        recalculateProjection();
    }

    void Camera::setScaleMode(ScaleMode newScaleMode)
    {
        scaleMode = newScaleMode;
        recalculateProjection();
    }

    void Camera::setTargetContentSize(const Size<float, 2>& newTargetContentSize)
    {
        targetContentSize = newTargetContentSize;
        recalculateProjection();
    }

    void Camera::setRenderTarget(graphics::RenderTarget* newRenderTarget)
    {
        renderTarget = newRenderTarget;
        recalculateProjection();
    }

    void Camera::setDepthTest(bool newDepthTest)
    {
        depthTest = newDepthTest;

        if (depthTest)
            depthStencilState = std::make_unique<graphics::DepthStencilState>(*engine->getGraphics(),
                                                                              true, true,
                                                                              graphics::CompareFunction::lessEqual,
                                                                              false,
                                                                              0xFFFFFFFF,
                                                                              0xFFFFFFFF,
                                                                              graphics::StencilOperation::keep,
                                                                              graphics::StencilOperation::keep,
                                                                              graphics::StencilOperation::keep,
                                                                              graphics::CompareFunction::always,
                                                                              graphics::StencilOperation::keep,
                                                                              graphics::StencilOperation::keep,
                                                                              graphics::StencilOperation::keep,
                                                                              graphics::CompareFunction::always);
        else
            depthStencilState.reset();
    }
}
