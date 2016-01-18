// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <cstdint>
#include "Vector2.h"

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
        AABB2(const Vector2& min, const Vector2& max);
        
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
        void getCorners(Vector2 *dst) const;
        
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
        void set(const Vector2& min, const Vector2& max);
        
        /**
         * Reset min and max value.If you invoke this method, isEmpty() shall return true.
         */
        void reset();
        
        /**
         * check the AABB object is empty(reset).
         */
        bool isEmpty() const;
        
        /**
         * update the _min and _max from the given point.
         */
        void updateMinMax(const Vector2* point, uint32_t num);
        
    public:
        Vector2 min;
        Vector2 max;
    };

    // end of 3d group
    /// @}
}
