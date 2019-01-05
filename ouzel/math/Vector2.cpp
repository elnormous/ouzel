// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"

namespace ouzel
{
    template<class T>
    Vector2<T>::Vector2(const Vector3<T>& vec):
        v{vec.v[0], vec.v[1]}
    {
    }

    template<class T>
    Vector2<T>& Vector2<T>::operator=(const Vector3<T>& vec)
    {
        v[0] = vec.v[0];
        v[1] = vec.v[1];
        return *this;
    }

    template<class T>
    Vector2<T>::Vector2(const Vector4<T>& vec):
        v{vec.v[0], vec.v[1]}
    {
    }

    template<class T>
    Vector2<T>& Vector2<T>::operator=(const Vector4<T>& vec)
    {
        v[0] = vec.v[0];
        v[1] = vec.v[1];
        return *this;
    }

    template class Vector2<float>;
}
