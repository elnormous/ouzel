// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdint>
#include "math/Vector2.h"
#include "math/Size2.h"

#pragma once

namespace ouzel
{
    /**
     * Axis Aligned Bounding Box (AABB), usually calculate some rough but fast collision detection.
     */
    class AABB2
    {

    public:
        /**
         * Constructor.
         * @lua new
         */
        AABB2();

        /**
         * Constructor.
         * @lua new
         */
        AABB2(const Vector2& pMin, const Vector2& pMax);

        /**
         * Constructor.
         */
        AABB2(const AABB2& box);

        /**
         * Gets the center point of the bounding box.
         */
        Vector2 getCenter();

        /* Near face, specified counter-clockwise looking towards the origin from the positive z-axis.
         * verts[0] : left top
         * verts[1] : left bottom
         * verts[2] : right bottom
         * verts[3] : right top
         */
        void getCorners(Vector2* dst) const;

        /**
         * Tests whether this bounding box intersects the specified bounding object.
         */
        bool intersects(const AABB2& aabb) const;

        /**
         * check whether the point is in.
         */
        bool containPoint(const Vector2& point) const;

        /**
         * Sets this bounding box to the smallest bounding box
         * that contains both this bounding object and the specified bounding box.
         */
        void merge(const AABB2& box);

        /**
         * Sets this bounding box to the specified values.
         */
        void set(const Vector2& pMin, const Vector2& pMax);

        /**
         * Reset min and max value.If you invoke this method, isEmpty() shall return true.
         */
        void reset();

        /**
         * check the AABB object is empty(reset).
         */
        bool isEmpty() const;

        /**
         * update the min and max from the given points.
         */
        void updateMinMax(const Vector2* points, uint32_t num);

        void insertPoint(const Vector2& point);

        inline const AABB2 operator+(const Vector2& v) const
        {
            AABB2 result(*this);
            result += v;
            return result;
        }

        inline AABB2& operator+=(const Vector2& v)
        {
            min += v;
            max += v;
            return *this;
        }

        inline const AABB2 operator-(const Vector2& v) const
        {
            AABB2 result(*this);
            result -= v;
            return result;
        }

        inline AABB2& operator-=(const Vector2& v)
        {
            min -= v;
            max -= v;
            return *this;
        }

        inline Size2 getSize() const
        {
            return Size2(max.x - min.x, max.y - min.y);
        }

        Vector2 min;
        Vector2 max;
    };
}
