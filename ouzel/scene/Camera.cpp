// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cassert>
#include <algorithm>
#include "Camera.h"
#include "core/Engine.h"
#include "graphics/Renderer.h"
#include "Layer.h"
#include "graphics/Texture.h"
#include "graphics/RenderTarget.h"
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

        void Camera::calculateTransform() const
        {
            Node::calculateTransform();

            viewProjectionDirty = inverseViewProjectionDirty = true;
        }

        void Camera::recalculateProjection()
        {
            Size2 renderTargetSize = renderTarget ?
                renderTarget->getTexture()->getSize() :
                sharedEngine->getRenderer()->getSize();

            renderViewport.position.v[0] = renderTargetSize.width * viewport.position.v[0];
            renderViewport.position.v[1] = renderTargetSize.height * viewport.position.v[1];
            renderViewport.size.width = renderTargetSize.width * viewport.size.width;
            renderViewport.size.height = renderTargetSize.height * viewport.size.height;

            assert(renderViewport.size.width > 0.0f && renderViewport.size.height > 0.0f);

            if (targetContentSize.width > 0.0f && targetContentSize.height > 0.0f)
            {
                contentScale.v[0] = renderViewport.size.width / targetContentSize.width;
                contentScale.v[1] = renderViewport.size.height / targetContentSize.height;

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

                contentSize = Size2(renderViewport.size.width / contentScale.v[0], renderViewport.size.height / contentScale.v[1]);
                contentPosition = Vector2((contentSize.width - targetContentSize.width) / 2.0f,
                                          (contentSize.height - targetContentSize.height) / 2.0f);
            }
            else
            {
                contentScale = Vector2(1.0f, 1.0f);
                contentSize = Size2(renderViewport.size.width, renderViewport.size.height);
                contentPosition = Vector2(0.0f, 0.0f);
            }

            switch (type)
            {
                case Type::ORTHOGRAPHIC:
                    Matrix4::createOrthographicFromSize(contentSize.width, contentSize.height, -1.0f, 1.0f, projection);
                    break;
                case Type::PERSPECTIVE:
                    Matrix4::createPerspective(contentSize.width / contentSize.height, fov, nearPlane, farPlane, projection);
                    break;
            }

            viewProjectionDirty = true;
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

            if (renderTarget)
            {
                renderViewProjection *= renderTarget->getProjectionTransform();
            }
            else
            {
                renderViewProjection *= sharedEngine->getRenderer()->getProjectionTransform();
            }

            viewProjectionDirty = false;
        }

        Vector3 Camera::convertNormalizedToWorld(const Vector2& position) const
        {
            // convert window normalized to viewport clip position
            Vector3 result = Vector3(((position.v[0] - viewport.position.v[0]) / viewport.size.width - 0.5f) * 2.0f,
                                     ((position.v[1] - viewport.position.v[1]) / viewport.size.height - 0.5f) * 2.0f,
                                     0.0f);

            getInverseViewProjection().transformPoint(result);

            return result;
        }

        Vector2 Camera::convertWorldToNormalized(const Vector3& position) const
        {
            Vector3 result = position;
            getViewProjection().transformPoint(result);

            // convert viewport clip position to window normalized
            return Vector2((result.v[0] / 2.0f + 0.5f) * viewport.size.width + viewport.position.v[0],
                           (result.v[1] / 2.0f + 0.5f) * viewport.size.height + viewport.position.v[1]);
        }

        bool Camera::checkVisibility(const Matrix4& boxTransform, const AABB2& boundingBox) const
        {
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

            halfWorldSize.width = std::max(fabsf(halfSize.width * boxTransform.m[0] + halfSize.height * boxTransform.m[4]),
                                           fabsf(halfSize.width * boxTransform.m[0] - halfSize.height * boxTransform.m[4]));
            halfWorldSize.height = std::max(fabsf(halfSize.width * boxTransform.m[1] + halfSize.height * boxTransform.m[5]),
                                            fabsf(halfSize.width * boxTransform.m[1] - halfSize.height * boxTransform.m[5]));

            // scale half size by camera projection to get the size in clip space coordinates
            halfWorldSize.width *= (fabsf(viewProjection.m[0]) + fabsf(viewProjection.m[4])) / 2.0f;
            halfWorldSize.height *= (fabsf(viewProjection.m[1]) + fabsf(viewProjection.m[5])) / 2.0f;

            // create visible rect in clip space
            Rectangle visibleRect(-halfWorldSize.width,
                                  -halfWorldSize.height,
                                  1.0f + halfWorldSize.width * 2.0f,
                                  1.0f + halfWorldSize.height * 2.0f);

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

        void Camera::setRenderTarget(const graphics::RenderTargetPtr& newRenderTarget)
        {
            renderTarget = newRenderTarget;

            recalculateProjection();
        }
    } // namespace scene
} // namespace ouzel
