// Copyright (C) 2018 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cassert>
#include <algorithm>
#include "Camera.hpp"
#include "core/Engine.hpp"
#include "graphics/Renderer.hpp"
#include "graphics/RenderDevice.hpp"
#include "Layer.hpp"
#include "graphics/TextureResource.hpp"
#include "math/Matrix4.hpp"

namespace ouzel
{
    namespace scene
    {
        Camera::Camera(Matrix4 initProjection):
            Component(TYPE),
            projectionMode(ProjectionMode::CUSTOM), projection(initProjection)

        {
            calculateViewProjection();
        }

        Camera::Camera(const Size2& initTargetContentSize, ScaleMode initScaleMode):
            Component(Component::Type::CAMERA),
            projectionMode(ProjectionMode::ORTHOGRAPHIC), targetContentSize(initTargetContentSize),
            scaleMode(initScaleMode)
        {
            calculateViewProjection();
        }

        Camera::Camera(float initFov, float initNearPlane, float initFarPlane):
            Component(Component::Type::CAMERA),
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
            Size2 renderTargetSize = renderTarget ?
                renderTarget->getSize() :
                engine->getRenderer()->getSize();

            renderViewport.position.x = renderTargetSize.width * viewport.position.x;
            renderViewport.position.y = renderTargetSize.height * viewport.position.y;
            renderViewport.size.width = renderTargetSize.width * viewport.size.width;
            renderViewport.size.height = renderTargetSize.height * viewport.size.height;

            assert(renderViewport.size.width > 0.0F && renderViewport.size.height > 0.0F);

            if (targetContentSize.width > 0.0F && targetContentSize.height > 0.0F)
            {
                contentScale.x = renderViewport.size.width / targetContentSize.width;
                contentScale.y = renderViewport.size.height / targetContentSize.height;

                switch (scaleMode)
                {
                    case ScaleMode::NONE:
                    {
                        break;
                    }
                    case ScaleMode::EXACT_FIT:
                    {
                        contentScale.x = 1.0F;
                        contentScale.y = 1.0F;
                        break;
                    }
                    case ScaleMode::NO_BORDER:
                    {
                        contentScale.x = contentScale.y = std::max(contentScale.x, contentScale.y);
                        break;
                    }
                    case ScaleMode::SHOW_ALL:
                    {
                        contentScale.x = contentScale.y = std::min(contentScale.x, contentScale.y);
                        break;
                    }
                    default:
                        return;
                }

                contentSize = Size2(renderViewport.size.width / contentScale.x, renderViewport.size.height / contentScale.y);
                contentPosition = Vector2((contentSize.width - targetContentSize.width) / 2.0F,
                                          (contentSize.height - targetContentSize.height) / 2.0F);
            }
            else
            {
                contentScale = Vector2(1.0F, 1.0F);
                contentSize = Size2(renderViewport.size.width, renderViewport.size.height);
                contentPosition = Vector2(0.0F, 0.0F);
            }

            switch (projectionMode)
            {
                case ProjectionMode::CUSTOM:
                    // do nothing
                    break;
                case ProjectionMode::ORTHOGRAPHIC:
                    Matrix4::createOrthographicFromSize(contentSize.width, contentSize.height, -1.0F, 1.0F, projection);
                    break;
                case ProjectionMode::PERSPECTIVE:
                    Matrix4::createPerspective(fov, contentSize.width / contentSize.height, nearPlane, farPlane, projection);
                    break;
                default:
                    return;
            }

            viewProjectionDirty = inverseViewProjectionDirty = true;
        }

        const Matrix4& Camera::getViewProjection() const
        {
            if (viewProjectionDirty)
            {
                calculateViewProjection();
            }

            return viewProjection;
        }

        const Matrix4& Camera::getRenderViewProjection() const
        {
            if (viewProjectionDirty)
            {
                calculateViewProjection();
            }

            return renderViewProjection;
        }

        const Matrix4& Camera::getInverseViewProjection() const
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

                renderViewProjection = viewProjection;

                renderViewProjection = engine->getRenderer()->getDevice()->getProjectionTransform(renderTarget != nullptr) * renderViewProjection;

                viewProjectionDirty = false;
            }
        }

        Vector3 Camera::convertNormalizedToWorld(const Vector2& normalizedPosition) const
        {
            // convert window normalized to viewport clip position
            Vector3 result = Vector3(((normalizedPosition.x - viewport.position.x) / viewport.size.width - 0.5F) * 2.0F,
                                     ((normalizedPosition.y - viewport.position.y) / viewport.size.height - 0.5F) * 2.0F,
                                     0.0F);

            getInverseViewProjection().transformPoint(result);

            return result;
        }

        Vector2 Camera::convertWorldToNormalized(const Vector3& normalizedPosition) const
        {
            Vector3 result = normalizedPosition;
            getViewProjection().transformPoint(result);

            // convert viewport clip position to window normalized
            return Vector2((result.x / 2.0F + 0.5F) * viewport.size.width + viewport.position.x,
                           (result.y / 2.0F + 0.5F) * viewport.size.height + viewport.position.y);
        }

        bool Camera::checkVisibility(const Matrix4& boxTransform, const Box3& box) const
        {
            if (projectionMode == ProjectionMode::ORTHOGRAPHIC)
            {
                // calculate center point of the box
                Vector2 diff = box.max - box.min;

                // offset the center point, so that it is relative to 0,0
                Vector3 v3p(box.min.x + diff.x / 2.0F, box.min.y + diff.y / 2.0F, 0.0F);

                // apply local transform to the center point
                boxTransform.transformPoint(v3p);

                // tranform the center to viewport's clip space
                Vector4 clipPos;
                getViewProjection().transformVector(Vector4(v3p.x, v3p.y, v3p.z, 1.0F), clipPos);

                assert(clipPos.w != 0.0F);

                // normalize position of the center point
                Vector2 v2p((clipPos.x / clipPos.w + 1.0F) * 0.5F,
                            (clipPos.y / clipPos.w + 1.0F) * 0.5F);

                // calculate half size
                Size2 halfSize(diff.x / 2.0F, diff.y / 2.0F);

                // convert content size to world coordinates
                Size2 halfWorldSize;

                halfWorldSize.width = std::max(fabsf(halfSize.width * boxTransform.m[0] + halfSize.height * boxTransform.m[4]),
                                               fabsf(halfSize.width * boxTransform.m[0] - halfSize.height * boxTransform.m[4]));
                halfWorldSize.height = std::max(fabsf(halfSize.width * boxTransform.m[1] + halfSize.height * boxTransform.m[5]),
                                                fabsf(halfSize.width * boxTransform.m[1] - halfSize.height * boxTransform.m[5]));

                // scale half size by camera projection to get the size in clip space coordinates
                halfWorldSize.width *= (fabsf(viewProjection.m[0]) + fabsf(viewProjection.m[4])) / 2.0F;
                halfWorldSize.height *= (fabsf(viewProjection.m[1]) + fabsf(viewProjection.m[5])) / 2.0F;

                // create visible rect in clip space
                Rect visibleRect(-halfWorldSize.width,
                                 -halfWorldSize.height,
                                 1.0F + halfWorldSize.width * 2.0F,
                                 1.0F + halfWorldSize.height * 2.0F);

                return visibleRect.containsPoint(v2p);
            }
            else
            {
                Matrix4 modelViewProjection = getViewProjection() * boxTransform;

                ConvexVolume frustum;

                if (!modelViewProjection.getFrustum(frustum)) return false;

                if (!frustum.isBoxInside(box)) return false;

                return true;
            }
        }

        void Camera::setViewport(const Rect& newViewport)
        {
            viewport = newViewport;
            recalculateProjection();
        }

        void Camera::setScaleMode(ScaleMode newScaleMode)
        {
            scaleMode = newScaleMode;
            recalculateProjection();
        }

        void Camera::setTargetContentSize(const Size2& newTargetContentSize)
        {
            targetContentSize = newTargetContentSize;
            recalculateProjection();
        }

        void Camera::setRenderTarget(const std::shared_ptr<graphics::Texture>& newRenderTarget)
        {
            renderTarget = newRenderTarget;
            recalculateProjection();
        }
    } // namespace scene
} // namespace ouzel
