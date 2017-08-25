// Copyright (C) 2017 Elviss Strazdins
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
        Camera::Camera(Matrix4 aProjection):
            type(Type::CUSTOM), projection(aProjection)
        {
            calculateViewProjection();
        }

        Camera::Camera(const Size2& aTargetContentSize, ScaleMode aScaleMode):
            type(Type::ORTHOGRAPHIC), targetContentSize(aTargetContentSize),
            scaleMode(aScaleMode)
        {
            calculateViewProjection();
        }

        Camera::Camera(float aFov, float aNearPlane, float aFarPlane):
            type(Type::PERSPECTIVE), fov(aFov),
            nearPlane(aNearPlane), farPlane(aFarPlane)
        {
            calculateViewProjection();
        }

        Camera::~Camera()
        {
            if (addedToLayer) addedToLayer->removeCamera(this);
        }

        void Camera::setLayer(Layer* newLayer)
        {
            Node::setLayer(newLayer);

            if (addedToLayer)
            {
                addedToLayer->removeCamera(this);
                addedToLayer = nullptr;
            }

            if (layer)
            {
                layer->addCamera(this);
                addedToLayer = layer;
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

            renderViewport.position.x = renderTargetSize.width * viewport.position.x;
            renderViewport.position.y = renderTargetSize.height * viewport.position.y;
            renderViewport.size.width = renderTargetSize.width * viewport.size.width;
            renderViewport.size.height = renderTargetSize.height * viewport.size.height;

            assert(renderViewport.size.width > 0.0f && renderViewport.size.height > 0.0f);

            if (targetContentSize.width > 0.0f && targetContentSize.height > 0.0f)
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
                        contentScale.x = 1.0f;
                        contentScale.y = 1.0f;
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
                }

                contentSize = Size2(renderViewport.size.width / contentScale.x, renderViewport.size.height / contentScale.y);
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
                case Type::CUSTOM:
                    // do nothing
                    break;
                case Type::ORTHOGRAPHIC:
                    Matrix4::createOrthographicFromSize(contentSize.width, contentSize.height, -1.0f, 1.0f, projection);
                    break;
                case Type::PERSPECTIVE:
                    Matrix4::createPerspective(fov, contentSize.width / contentSize.height, nearPlane, farPlane, projection);
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

            renderViewProjection = sharedEngine->getRenderer()->getDevice()->getProjectionTransform(renderTarget != nullptr) * renderViewProjection;

            viewProjectionDirty = false;
        }

        Vector3 Camera::convertNormalizedToWorld(const Vector2& normalizedPosition) const
        {
            // convert window normalized to viewport clip position
            Vector3 result = Vector3(((normalizedPosition.x - viewport.position.x) / viewport.size.width - 0.5f) * 2.0f,
                                     ((normalizedPosition.y - viewport.position.y) / viewport.size.height - 0.5f) * 2.0f,
                                     0.0f);

            getInverseViewProjection().transformPoint(result);

            return result;
        }

        Vector2 Camera::convertWorldToNormalized(const Vector3& normalizedPosition) const
        {
            Vector3 result = normalizedPosition;
            getViewProjection().transformPoint(result);

            // convert viewport clip position to window normalized
            return Vector2((result.x / 2.0f + 0.5f) * viewport.size.width + viewport.position.x,
                           (result.y / 2.0f + 0.5f) * viewport.size.height + viewport.position.y);
        }

        bool Camera::checkVisibility(const Matrix4& boxTransform, const Box3& boundingBox) const
        {
            if (type == Type::ORTHOGRAPHIC)
            {
                // calculate center point of the bounding box
                Vector2 diff = boundingBox.max - boundingBox.min;

                // offset the center point, so that it is relative to 0,0
                Vector3 v3p(boundingBox.min.x + diff.x / 2.0f, boundingBox.min.y + diff.y / 2.0f, 0.0f);

                // apply local transform to the center point
                boxTransform.transformPoint(v3p);

                // tranform the center to viewport's clip space
                Vector4 clipPos;
                getViewProjection().transformVector(Vector4(v3p.x, v3p.y, v3p.z, 1.0f), clipPos);

                assert(clipPos.w != 0.0f);

                // normalize position of the center point
                Vector2 v2p((clipPos.x / clipPos.w + 1.0f) * 0.5f,
                            (clipPos.y / clipPos.w + 1.0f) * 0.5f);

                // calculate half size
                Size2 halfSize(diff.x / 2.0f, diff.y / 2.0f);

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
            else
            {
                Matrix4 modelViewProjection = getViewProjection() * boxTransform;

                ConvexVolume frustum;

                if (!modelViewProjection.getFrustum(frustum)) return false;

                //if (!frustum.isPointInside(Vector4(0.0f, 0.0f, 0.0f, 1.0f))) return false;
                if (!frustum.isBoxInside(boundingBox)) return false;

                return true;
            }
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
