// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cassert>
#include <algorithm>
#include "Camera.h"
#include "core/Engine.h"
#include "graphics/Renderer.h"
#include "Layer.h"
#include "graphics/TextureResource.h"
#include "math/Matrix4.h"
#include "utils/Utils.h"

namespace ouzel
{
    namespace scene
    {
        Camera::Camera(Type aType, float aFov, float aNearPlane, float aFarPlane):
            type(aType), fov(aFov), nearPlane(aNearPlane), farPlane(aFarPlane)
        {
        }

        Camera::~Camera()
        {
            if (layer) layer->removeCamera(this);
        }

        void Camera::setLayer(Layer* newLayer)
        {
            if (layer)
            {
                layer->removeCamera(this);
            }

            layer = newLayer;

            if (layer)
            {
                layer->addCamera(this);
            }
        }

        void Camera::calculateTransform() const
        {
            Node::calculateTransform();

            viewProjectionDirty = inverseViewProjectionDirty = true;
        }

        void Camera::recalculateProjection()
        {
            Size2 renderTargetSize = renderTarget ?
                renderTarget->getSize() :
                sharedEngine->getRenderer()->getSize();

            renderViewport.position.v[0] = renderTargetSize.v[0] * viewport.position.v[0];
            renderViewport.position.v[1] = renderTargetSize.v[1] * viewport.position.v[1];
            renderViewport.size.v[0] = renderTargetSize.v[0] * viewport.size.v[0];
            renderViewport.size.v[1] = renderTargetSize.v[1] * viewport.size.v[1];

            assert(renderViewport.size.v[0] > 0.0f && renderViewport.size.v[1] > 0.0f);

            if (targetContentSize.v[0] > 0.0f && targetContentSize.v[1] > 0.0f)
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
                        contentScale.v[0] = 1.0f;
                        contentScale.v[1] = 1.0f;
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
                }

                contentSize = Size2(renderViewport.size.v[0] / contentScale.v[0], renderViewport.size.v[1] / contentScale.v[1]);
                contentPosition = Vector2((contentSize.v[0] - targetContentSize.v[0]) / 2.0f,
                                          (contentSize.v[1] - targetContentSize.v[1]) / 2.0f);
            }
            else
            {
                contentScale = Vector2(1.0f, 1.0f);
                contentSize = Size2(renderViewport.size.v[0], renderViewport.size.v[1]);
                contentPosition = Vector2(0.0f, 0.0f);
            }

            switch (type)
            {
                case Type::ORTHOGRAPHIC:
                    Matrix4::createOrthographicFromSize(contentSize.v[0], contentSize.v[1], -1.0f, 1.0f, projection);
                    break;
                case Type::PERSPECTIVE:
                    Matrix4::createPerspective(contentSize.v[0] / contentSize.v[1], fov, nearPlane, farPlane, projection);
                    break;
            }

            viewProjectionDirty = inverseViewProjectionDirty = true;
        }

        const Matrix4& Camera::getViewProjection() const
        {
            if (viewProjectionDirty || transformDirty)
            {
                calculateViewProjection();
            }

            return viewProjection;
        }

        const Matrix4& Camera::getRenderViewProjection() const
        {
            if (viewProjectionDirty || transformDirty)
            {
                calculateViewProjection();
            }

            return renderViewProjection;
        }

        const Matrix4& Camera::getInverseViewProjection() const
        {
            if (inverseViewProjectionDirty || transformDirty)
            {
                inverseViewProjection = getViewProjection();
                inverseViewProjection.invert();

                inverseViewProjectionDirty = false;
            }

            return inverseViewProjection;
        }

        void Camera::calculateViewProjection() const
        {
            viewProjection = projection * getInverseTransform();

            renderViewProjection = viewProjection;

            renderViewProjection = sharedEngine->getRenderer()->getProjectionTransform(renderTarget != nullptr) * renderViewProjection;

            viewProjectionDirty = false;
        }

        Vector3 Camera::convertNormalizedToWorld(const Vector2& position) const
        {
            // convert window normalized to viewport clip position
            Vector3 result = Vector3(((position.v[0] - viewport.position.v[0]) / viewport.size.v[0] - 0.5f) * 2.0f,
                                     ((position.v[1] - viewport.position.v[1]) / viewport.size.v[1] - 0.5f) * 2.0f,
                                     0.0f);

            getInverseViewProjection().transformPoint(result);

            return result;
        }

        Vector2 Camera::convertWorldToNormalized(const Vector3& position) const
        {
            Vector3 result = position;
            getViewProjection().transformPoint(result);

            // convert viewport clip position to window normalized
            return Vector2((result.v[0] / 2.0f + 0.5f) * viewport.size.v[0] + viewport.position.v[0],
                           (result.v[1] / 2.0f + 0.5f) * viewport.size.v[1] + viewport.position.v[1]);
        }

        bool Camera::checkVisibility(const Matrix4& boxTransform, const Box3& boundingBox) const
        {
            if (type == Type::PERSPECTIVE)
            {
                return true;
            }

            // calculate center point of the bounding box
            Vector2 diff = boundingBox.max - boundingBox.min;

            // offset the center point, so that it is relative to 0,0
            Vector3 v3p(boundingBox.min.v[0] + diff.v[0] / 2.0f, boundingBox.min.v[1] + diff.v[1] / 2.0f, 0.0f);

            // apply local transform to the center point
            boxTransform.transformPoint(v3p);

            // tranform the center to viewport's clip space
            Vector4 clipPos;
            getViewProjection().transformVector(Vector4(v3p.v[0], v3p.v[1], v3p.v[2], 1.0f), clipPos);

            assert(clipPos.v[3] != 0.0f);

            // normalize position of the center point
            Vector2 v2p((clipPos.v[0] / clipPos.v[3] + 1.0f) * 0.5f,
                        (clipPos.v[1] / clipPos.v[3] + 1.0f) * 0.5f);

            // calculate half size
            Size2 halfSize(diff.v[0] / 2.0f, diff.v[1] / 2.0f);

            // convert content size to world coordinates
            Size2 halfWorldSize;

            halfWorldSize.v[0] = std::max(fabsf(halfSize.v[0] * boxTransform.m[0] + halfSize.v[1] * boxTransform.m[4]),
                                           fabsf(halfSize.v[0] * boxTransform.m[0] - halfSize.v[1] * boxTransform.m[4]));
            halfWorldSize.v[1] = std::max(fabsf(halfSize.v[0] * boxTransform.m[1] + halfSize.v[1] * boxTransform.m[5]),
                                            fabsf(halfSize.v[0] * boxTransform.m[1] - halfSize.v[1] * boxTransform.m[5]));

            // scale half size by camera projection to get the size in clip space coordinates
            halfWorldSize.v[0] *= (fabsf(viewProjection.m[0]) + fabsf(viewProjection.m[4])) / 2.0f;
            halfWorldSize.v[1] *= (fabsf(viewProjection.m[1]) + fabsf(viewProjection.m[5])) / 2.0f;

            // create visible rect in clip space
            Rectangle visibleRect(-halfWorldSize.v[0],
                                  -halfWorldSize.v[1],
                                  1.0f + halfWorldSize.v[0] * 2.0f,
                                  1.0f + halfWorldSize.v[1] * 2.0f);

            return visibleRect.containsPoint(v2p);
        }

        void Camera::setViewport(const Rectangle& newViewport)
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
