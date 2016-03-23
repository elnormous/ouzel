// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cassert>
#include "Camera.h"
#include "Engine.h"
#include "Renderer.h"
#include "Layer.h"
#include "Matrix4.h"
#include "Utils.h"

namespace ouzel
{
    namespace scene
    {
        Camera::Camera()
        {

        }

        Camera::~Camera()
        {

        }

        void Camera::setZoom(float zoom)
        {
            _zoom = zoom;

            if (_zoom < 0.1f)
            {
                _zoom = 0.1f;
            }

            _localTransformDirty = _transformDirty = _inverseTransformDirty = true;
        }

        void Camera::recalculateProjection()
        {
            Size2 screenSize = Engine::getInstance()->getRenderer()->getSize();

            if (screenSize.width == 0.0f || screenSize.height == 0.0f)
            {
                return;
            }

            if (_targetContentSize.width != 0.0f && _targetContentSize.height != 0.0f)
            {
                _contentSize = _targetContentSize;
                _contentScale.x = screenSize.width / _contentSize.width;
                _contentScale.y = screenSize.height / _contentSize.height;

                switch (_scaleMode)
                {
                    case ScaleMode::None:
                    {
                        break;
                    }
                    case ScaleMode::ExactFit:
                    {
                        _contentScale.x = 1.0f;
                        _contentScale.y = 1.0f;
                        break;
                    }
                    case ScaleMode::NoBorder:
                    {
                        _contentScale.x = _contentScale.y = fmaxf(_contentScale.x, _contentScale.y);
                        break;
                    }
                    case ScaleMode::ShowAll:
                    {
                        _contentScale.x = _contentScale.y = fminf(_contentScale.x, _contentScale.y);
                        break;
                    }
                }
            }
            else
            {
                _contentSize = screenSize;
                _contentScale = Vector2(1.0f, 1.0f);
            }

            Matrix4::createOrthographic(screenSize.width / _contentScale.x, screenSize.height / _contentScale.y, -1.0f, 1.0f, _projection);

            _inverseProjection = _projection;
            _inverseProjection.invert();

            _viewProjectionDirty = true;
        }

        const Matrix4& Camera::getViewProjection() const
        {
            if (_viewProjectionDirty || _transformDirty)
            {
                _viewProjection = _projection * getTransform();
            }

            return _viewProjection;
        }

        void Camera::calculateLocalTransform() const
        {
            Matrix4 translation;
            translation.translate(-_position.x, -_position.y, 0.0f);

            Matrix4 rotation;
            rotation.rotate(Vector3(0.0f, 0.0f, -1.0f), -_rotation);

            Matrix4 scale;
            scale.scale(_zoom);

            _localTransform = scale * rotation * translation;

            _localTransformDirty = false;
        }

        Vector2 Camera::convertScreenToWorld(const Vector2& position)
        {
            Matrix4 projViewMatrix = _projection * getTransform();
            Matrix4 inverseViewMatrix = projViewMatrix;
            inverseViewMatrix.invert();

            Vector3 result = Vector3(position.x, position.y, 0.0f);
            inverseViewMatrix.transformPoint(result);

            return Vector2(result.x, result.y);
        }

        Vector2 Camera::convertWorldToScreen(const Vector2& position)
        {
            Matrix4 projViewMatrix = _projection * getTransform();

            Vector3 result = Vector3(position.x, position.y, 0.0f);
            projViewMatrix.transformPoint(result);

            return Vector2(result.x, result.y);
        }

        Vector2 Camera::projectPoint(const Vector3& src) const
        {
            Vector2 screenPos;

            if (LayerPtr layer = _layer.lock())
            {
                auto viewport = Engine::getInstance()->getRenderer()->getSize();
                Vector4 clipPos;

                getViewProjection().transformVector(Vector4(src.x, src.y, src.z, 1.0f), clipPos);

                assert(clipPos.w != 0.0f);
                Vector2 ndc(clipPos.x / clipPos.w, clipPos.y / clipPos.w);

                screenPos.x = (ndc.x + 1.0f) * 0.5f * viewport.width;
                screenPos.y = (ndc.y + 1.0f) * 0.5f * viewport.height;
            }

            return screenPos;
        }

        void Camera::setScaleMode(ScaleMode scaleMode)
        {
            _scaleMode = scaleMode;
            recalculateProjection();
        }

        void Camera::setTargetContentSize(const Size2& targetContentSize)
        {
            _targetContentSize = targetContentSize;
            recalculateProjection();
        }
    } // namespace scene
} // namespace ouzel
