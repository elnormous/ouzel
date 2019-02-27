// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include <cassert>
#include <algorithm>
#include "Camera.hpp"
#include "core/Engine.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/RenderDevice.hpp"
#include "Actor.hpp"
#include "Layer.hpp"
#include "math/Matrix.hpp"

namespace ouzel
{
    namespace scene
    {
        Camera::Camera(const Matrix4F& initProjection):
            Component(CLASS),
            projectionMode(ProjectionMode::CUSTOM), projection(initProjection)

        {
            calculateViewProjection();
        }

        Camera::Camera(const Size2F& initTargetContentSize, ScaleMode initScaleMode):
            Component(CLASS),
            projectionMode(ProjectionMode::ORTHOGRAPHIC), targetContentSize(initTargetContentSize),
            scaleMode(initScaleMode)
        {
            calculateViewProjection();
        }

        Camera::Camera(float initFov, float initNearPlane, float initFarPlane):
            Component(CLASS),
            projectionMode(ProjectionMode::PERSPECTIVE), fov(initFov),
            nearPlane(initNearPlane), farPlane(initFarPlane)
        {
            calculateViewProjection();
        }

        Camera::~Camera()
        {
            if (layer) layer->removeCamera(this);
        }

        void Camera::setActor(Actor* newActor)
        {
            Component::setActor(newActor);

            viewProjectionDirty = inverseViewProjectionDirty = true;
        }

        void Camera::setLayer(Layer* newLayer)
        {
            if (layer) layer->removeCamera(this);

            Component::setLayer(newLayer);

            if (layer) layer->addCamera(this);
        }

        void Camera::updateTransform()
        {
            Component::updateTransform();

            viewProjectionDirty = inverseViewProjectionDirty = true;
        }

        void Camera::recalculateProjection()
        {
            Size2U renderTargetSize;

            if (renderTarget)
            {
                if (!renderTarget->getColorTextures().empty())
                    renderTargetSize = renderTarget->getColorTextures()[0]->getSize();
                else if (renderTarget->getDepthTexture())
                    renderTargetSize = renderTarget->getDepthTexture()->getSize();
            }
            else
                renderTargetSize = engine->getRenderer()->getSize();

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
                    case ScaleMode::NONE:
                    {
                        break;
                    }
                    case ScaleMode::EXACT_FIT:
                    {
                        contentScale.v[0] = 1.0F;
                        contentScale.v[1] = 1.0F;
                        break;
                    }
                    case ScaleMode::NO_BORDER:
                    {
                        contentScale.v[0] = contentScale.v[1] = std::max(contentScale.v[0], contentScale.v[1]);
                        break;
                    }
                    case ScaleMode::SHOW_ALL:
                    {
                        contentScale.v[0] = contentScale.v[1] = std::min(contentScale.v[0], contentScale.v[1]);
                        break;
                    }
                    default:
                        return;
                }

                contentSize = Size2F(renderViewport.size.v[0] / contentScale.v[0], renderViewport.size.v[1] / contentScale.v[1]);
                contentPosition = Vector2F((contentSize.v[0] - targetContentSize.v[0]) / 2.0F,
                                          (contentSize.v[1] - targetContentSize.v[1]) / 2.0F);
            }
            else
            {
                contentScale = Vector2F(1.0F, 1.0F);
                contentSize = Size2F(renderViewport.size.v[0], renderViewport.size.v[1]);
                contentPosition = Vector2F(0.0F, 0.0F);
            }

            switch (projectionMode)
            {
                case ProjectionMode::CUSTOM:
                    // do nothing
                    break;
                case ProjectionMode::ORTHOGRAPHIC:
                    Matrix4F::createOrthographicFromSize(contentSize.v[0], contentSize.v[1], -1.0F, 1.0F, projection);
                    break;
                case ProjectionMode::PERSPECTIVE:
                    Matrix4F::createPerspective(fov, contentSize.v[0] / contentSize.v[1], nearPlane, farPlane, projection);
                    break;
                default:
                    return;
            }

            viewProjectionDirty = inverseViewProjectionDirty = true;
        }

        const Matrix4F& Camera::getViewProjection() const
        {
            if (viewProjectionDirty) calculateViewProjection();

            return viewProjection;
        }

        const Matrix4F& Camera::getRenderViewProjection() const
        {
            if (viewProjectionDirty) calculateViewProjection();

            return renderViewProjection;
        }

        const Matrix4F& Camera::getInverseViewProjection() const
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

                renderViewProjection = engine->getRenderer()->getDevice()->getProjectionTransform(renderTarget != nullptr) * viewProjection;

                viewProjectionDirty = false;
            }
        }

        Vector3F Camera::convertNormalizedToWorld(const Vector2F& normalizedPosition) const
        {
            // convert window normalized to viewport clip position
            Vector3F result = Vector3F(((normalizedPosition.v[0] - viewport.position.v[0]) / viewport.size.v[0] - 0.5F) * 2.0F,
                                     (((1.0F - normalizedPosition.v[1]) - viewport.position.v[1]) / viewport.size.v[1] - 0.5F) * 2.0F,
                                     0.0F);

            getInverseViewProjection().transformPoint(result);

            return result;
        }

        Vector2F Camera::convertWorldToNormalized(const Vector3F& normalizedPosition) const
        {
            Vector3F result = normalizedPosition;
            getViewProjection().transformPoint(result);

            // convert viewport clip position to window normalized
            return Vector2F((result.v[0] / 2.0F + 0.5F) * viewport.size.v[0] + viewport.position.v[0],
                           1.0F - ((result.v[1] / 2.0F + 0.5F) * viewport.size.v[1] + viewport.position.v[1]));
        }

        bool Camera::checkVisibility(const Matrix4F& boxTransform, const Box3F& box) const
        {
            if (projectionMode == ProjectionMode::ORTHOGRAPHIC)
            {
                // calculate center point of the box
                Vector2F diff = Vector2F(box.max - box.min);

                // offset the center point, so that it is relative to 0,0
                Vector3F v3p(box.min.v[0] + diff.v[0] / 2.0F, box.min.v[1] + diff.v[1] / 2.0F, 0.0F);

                // apply local transform to the center point
                boxTransform.transformPoint(v3p);

                // tranform the center to viewport's clip space
                Vector4F clipPos;
                getViewProjection().transformVector(Vector4F(v3p.v[0], v3p.v[1], v3p.v[2], 1.0F), clipPos);

                assert(clipPos.v[3] != 0.0F);

                // normalize position of the center point
                Vector2F v2p((clipPos.v[0] / clipPos.v[3] + 1.0F) * 0.5F,
                            (clipPos.v[1] / clipPos.v[3] + 1.0F) * 0.5F);

                // calculate half size
                Size2F halfSize(diff.v[0] / 2.0F, diff.v[1] / 2.0F);

                // convert content size to world coordinates
                Size2F halfWorldSize;

                halfWorldSize.v[0] = std::max(fabsf(halfSize.v[0] * boxTransform.m[0] + halfSize.v[1] * boxTransform.m[4]),
                                               fabsf(halfSize.v[0] * boxTransform.m[0] - halfSize.v[1] * boxTransform.m[4]));
                halfWorldSize.v[1] = std::max(fabsf(halfSize.v[0] * boxTransform.m[1] + halfSize.v[1] * boxTransform.m[5]),
                                                fabsf(halfSize.v[0] * boxTransform.m[1] - halfSize.v[1] * boxTransform.m[5]));

                // scale half size by camera projection to get the size in clip space coordinates
                halfWorldSize.v[0] *= (fabsf(viewProjection.m[0]) + fabsf(viewProjection.m[4])) / 2.0F;
                halfWorldSize.v[1] *= (fabsf(viewProjection.m[1]) + fabsf(viewProjection.m[5])) / 2.0F;

                // create visible rect in clip space
                RectF visibleRect(-halfWorldSize.v[0],
                                 -halfWorldSize.v[1],
                                 1.0F + halfWorldSize.v[0] * 2.0F,
                                 1.0F + halfWorldSize.v[1] * 2.0F);

                return visibleRect.containsPoint(v2p);
            }
            else
            {
                Matrix4F modelViewProjection = getViewProjection() * boxTransform;

                ConvexVolumeF frustum = modelViewProjection.getFrustum();
                return frustum.isBoxInside(box);
            }
        }

        void Camera::setViewport(const RectF& newViewport)
        {
            viewport = newViewport;
            recalculateProjection();
        }

        void Camera::setScaleMode(ScaleMode newScaleMode)
        {
            scaleMode = newScaleMode;
            recalculateProjection();
        }

        void Camera::setTargetContentSize(const Size2F& newTargetContentSize)
        {
            targetContentSize = newTargetContentSize;
            recalculateProjection();
        }

        void Camera::setRenderTarget(const std::shared_ptr<graphics::RenderTarget>& newRenderTarget)
        {
            renderTarget = newRenderTarget;
            recalculateProjection();
        }

        void Camera::setDepthTest(bool newDepthTest)
        {
            depthTest = newDepthTest;

            if (depthTest)
                depthStencilState = std::make_shared<graphics::DepthStencilState>(*engine->getRenderer(),
                                                                                  true, true,
                                                                                  graphics::DepthStencilState::CompareFunction::LESS_EQUAL,
                                                                                  false,
                                                                                  0xFFFFFFFF,
                                                                                  0xFFFFFFFF,
                                                                                  graphics::DepthStencilState::StencilDescriptor(),
                                                                                  graphics::DepthStencilState::StencilDescriptor());
            else
                depthStencilState.reset();
        }
    } // namespace scene
} // namespace ouzel
