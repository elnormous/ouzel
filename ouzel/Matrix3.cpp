//
//  Matrix3.cpp
//  Ouzel
//
//  Created by Elviss Strazdins on 10/07/15.
//  Copyright (c) 2015 Elviss. All rights reserved.
//

#include "Matrix3.h"
#include "MathUtils.h"

namespace ouzel
{
    static const float MATRIX_IDENTITY[9] =
    {
        1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 1.0f,
    };
    
    Matrix3::Matrix3()
    {
        *this = Matrix3::identity();
    }
    
    Matrix3::Matrix3(float m11, float m12, float m13, float m21, float m22, float m23,
                     float m31, float m32, float m33)
    {
        set(m11, m12, m13, m21, m22, m23, m31, m32, m33);
    }
    
    Matrix3::Matrix3(const float* m)
    {
        set(m);
    }
    
    Matrix3::Matrix3(const Matrix3& copy)
    {
        memcpy(m, copy.m, sizeof(m));
    }
    
    Matrix3::~Matrix3()
    {
    }
    
    const Matrix3& Matrix3::identity()
    {
        static Matrix3 m(
                         1, 0, 0,
                         0, 1, 0,
                         0, 0, 1);
        return m;
    }
    
    const Matrix3& Matrix3::zero()
    {
        static Matrix3 m(
                         0, 0, 0,
                         0, 0, 0,
                         0, 0, 0);
        return m;
    }
    
    void Matrix3::createScale(const Vector2& scale, Matrix3* dst)
    {
        assert(dst);
        
        memcpy(dst, MATRIX_IDENTITY, sizeof(m));
        
        dst->m[0] = scale.x;
        dst->m[4] = scale.y;
    }
    
    void Matrix3::createScale(float xScale, float yScale, Matrix3* dst)
    {
        assert(dst);
        
        memcpy(dst, MATRIX_IDENTITY, sizeof(m));
        
        dst->m[0] = xScale;
        dst->m[4] = yScale;
    }
    
    void Matrix3::createRotation(float angle, Matrix3* dst)
    {
        assert(dst);
        
        float c = cos(angle);
        float s = sin(angle);
        
        dst->m[0] = c;
        dst->m[1] = -s;
        dst->m[2] = 0.0f;
        
        dst->m[3] = s;
        dst->m[4] = c;
        dst->m[5] = 0.0f;
        
        dst->m[6] = 0.0f;
        dst->m[7] = 0.0f;
        dst->m[8] = 1.0f;
    }
    
    void Matrix3::createTranslation(const Vector2& translation, Matrix3* dst)
    {
        assert(dst);
        
        memcpy(dst, MATRIX_IDENTITY, sizeof(m));
        
        dst->m[6] = translation.x;
        dst->m[7] = translation.y;
    }
    
    void Matrix3::createTranslation(float xTranslation, float yTranslation, Matrix3* dst)
    {
        assert(dst);
        
        memcpy(dst, MATRIX_IDENTITY, sizeof(m));
        
        dst->m[6] = xTranslation;
        dst->m[7] = yTranslation;
    }
    
    void Matrix3::add(float scalar)
    {
        add(scalar, this);
    }
    
    void Matrix3::add(float scalar, Matrix3* dst)
    {
        assert(dst);
        
        addMatrix3(m, scalar, dst->m);
    }
    
    void Matrix3::add(const Matrix3& m)
    {
        add(*this, m, this);
    }
    
    void Matrix3::add(const Matrix3& m1, const Matrix3& m2, Matrix3* dst)
    {
        assert(dst);
        
        addMatrix3(m1.m, m2.m, dst->m);
    }
    
    float Matrix3::determinant() const
    {
        float a0 = m[0] * m[4] * m[8];
        float a1 = m[1] * m[5] * m[6];
        float a2 = m[2] * m[3] * m[7];
        float a3 = m[2] * m[4] * m[6];
        float a4 = m[1] * m[3] * m[8];
        float a5 = m[0] * m[5] * m[7];
        // Calculate the determinant.
        return (a0 + a1 + a2 - a3 - a4 - a5);
    }
    
    bool Matrix3::invert()
    {
        return invert(this);
    }
    
    bool Matrix3::invert(Matrix3* dst) const
    {
        /*float a0 = m[0] * m[4] * m[8];
        float a1 = m[1] * m[5] * m[6];
        float a2 = m[2] * m[3] * m[7];
        float a3 = m[2] * m[4] * m[6];
        float a4 = m[1] * m[3] * m[8];
        float a5 = m[0] * m[5] * m[7];

        // Calculate the determinant.
        float det = a0 + a1 + a2 - a3 - a4 - a5;
        
        // Close to zero, can't invert.
        if (fabs(det) <= MATH_TOLERANCE)
            return false;
        
        // Support the case where m == dst.
        Matrix3 inverse;
        inverse.m[0]  = m[5] * b5 - m[6] * b4 + m[7] * b3;
        inverse.m[1]  = -m[1] * b5 + m[2] * b4 - m[3] * b3;
        inverse.m[2]  = m[13] * a5 - m[14] * a4 + m[15] * a3;
        inverse.m[3]  = -m[9] * a5 + m[10] * a4 - m[11] * a3;
        
        inverse.m[4]  = -m[4] * b5 + m[6] * b2 - m[7] * b1;
        inverse.m[5]  = m[0] * b5 - m[2] * b2 + m[3] * b1;
        inverse.m[6]  = -m[12] * a5 + m[14] * a2 - m[15] * a1;
        inverse.m[7]  = m[8] * a5 - m[10] * a2 + m[11] * a1;
        
        inverse.m[8]  = m[4] * b4 - m[5] * b2 + m[7] * b0;
        inverse.m[9]  = -m[0] * b4 + m[1] * b2 - m[3] * b0;
        inverse.m[10] = m[12] * a4 - m[13] * a2 + m[15] * a0;
        inverse.m[11] = -m[8] * a4 + m[9] * a2 - m[11] * a0;
        
        inverse.m[12] = -m[4] * b3 + m[5] * b1 - m[6] * b0;
        inverse.m[13] = m[0] * b3 - m[1] * b1 + m[2] * b0;
        inverse.m[14] = -m[12] * a3 + m[13] * a1 - m[14] * a0;
        inverse.m[15] = m[8] * a3 - m[9] * a1 + m[10] * a0;
        
        multiply(inverse, 1.0f / det, dst);*/
        
        return true;
    }
    
    bool Matrix3::isIdentity() const
    {
        return (memcmp(m, MATRIX_IDENTITY, sizeof(m)) == 0);
    }
    
    void Matrix3::multiply(float scalar)
    {
        multiply(scalar, this);
    }
    
    void Matrix3::multiply(float scalar, Matrix3* dst) const
    {
        multiply(*this, scalar, dst);
    }
    
    void Matrix3::multiply(const Matrix3& m, float scalar, Matrix3* dst)
    {
        assert(dst);
        
        multiplyMatrix3(m.m, scalar, dst->m);
    }
    
    void Matrix3::multiply(const Matrix3& m)
    {
        multiply(*this, m, this);
    }
    
    void Matrix3::multiply(const Matrix3& m1, const Matrix3& m2, Matrix3* dst)
    {
        assert(dst);
        
        multiplyMatrix3(m1.m, m2.m, dst->m);
    }
    
    void Matrix3::negate()
    {
        negate(this);
    }
    
    void Matrix3::negate(Matrix3* dst) const
    {
        assert(dst);
        
        negateMatrix3(m, dst->m);
    }
    
    void Matrix3::rotate(float angle)
    {
        rotate(angle, this);
    }
    
    void Matrix3::rotate(float angle, Matrix3* dst) const
    {
        Matrix3 r;
        createRotation(angle, &r);
        multiply(*this, r, dst);
    }
    
    void Matrix3::scale(float value)
    {
        scale(value, this);
    }
    
    void Matrix3::scale(float value, Matrix3* dst) const
    {
        scale(value, value, dst);
    }
    
    void Matrix3::scale(float xScale, float yScale)
    {
        scale(xScale, yScale, this);
    }
    
    void Matrix3::scale(float xScale, float yScale, Matrix3* dst) const
    {
        Matrix3 s;
        createScale(xScale, yScale, &s);
        multiply(*this, s, dst);
    }
    
    void Matrix3::scale(const Vector2& s)
    {
        scale(s.x, s.y, this);
    }
    
    void Matrix3::scale(const Vector2& s, Matrix3* dst) const
    {
        scale(s.x, s.y, dst);
    }
    
    void Matrix3::set(float m11, float m12, float m13, float m21, float m22, float m23,
                      float m31, float m32, float m33)
    {
        m[0]  = m11;
        m[1]  = m21;
        m[2]  = m31;
        m[3]  = m12;
        m[4]  = m22;
        m[5]  = m32;
        m[6]  = m13;
        m[7]  = m23;
        m[8]  = m33;
    }
    
    void Matrix3::set(const float* m)
    {
        assert(m);
        memcpy(this->m, m, sizeof(this->m));
    }
    
    void Matrix3::set(const Matrix3& m)
    {
        memcpy(this->m, m.m, sizeof(this->m));
    }
    
    void Matrix3::setIdentity()
    {
        memcpy(m, MATRIX_IDENTITY, sizeof(m));
    }
    
    void Matrix3::setZero()
    {
        memset(m, 0, sizeof(m));
    }
    
    void Matrix3::subtract(const Matrix3& m)
    {
        subtract(*this, m, this);
    }
    
    void Matrix3::subtract(const Matrix3& m1, const Matrix3& m2, Matrix3* dst)
    {
        assert(dst);
        
        subtractMatrix3(m1.m, m2.m, dst->m);
    }
    
    void Matrix3::transformPoint(Vector2* point) const
    {
        assert(point);
        //transformVector(point->x, point->y, 1.0f, point);
    }
    
    void Matrix3::transformPoint(const Vector2& point, Vector2* dst) const
    {
        //transformVector(point.x, point.y, 1.0f, dst);
    }
    
    void Matrix3::transformVector(Vector2* vector) const
    {
        assert(vector);
        //transformVector(vector->x, vector->y, 0.0f, vector);
    }
    
    void Matrix3::transformVector(const Vector2& vector, Vector2* dst) const
    {
        //transformVector(vector.x, vector.y, 0.0f, dst);
    }
    
    void Matrix3::transformVector(float x, float y, float z, float w, Vector3* dst) const
    {
        assert(dst);
        
        transformVector4(m, x, y, z, w, (float*)dst);
    }
    
    void Matrix3::translate(float x, float y)
    {
        translate(x, y, this);
    }
    
    void Matrix3::translate(float x, float y, Matrix3* dst) const
    {
        Matrix3 t;
        createTranslation(x, y, &t);
        multiply(*this, t, dst);
    }
    
    void Matrix3::translate(const Vector2& t)
    {
        translate(t.x, t.y, this);
    }
    
    void Matrix3::translate(const Vector2& t, Matrix3* dst) const
    {
        translate(t.x, t.y, dst);
    }
    
    void Matrix3::transpose()
    {
        transpose(this);
    }
    
    void Matrix3::transpose(Matrix3* dst) const
    {
        assert(dst);
        
        transposeMatrix3(m, dst->m);
    }
}
