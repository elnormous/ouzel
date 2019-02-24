// Copyright 2015-2019 Elviss Strazdins. All rights reserved.

#include "Vector.hpp"
#include "Color.hpp"

namespace ouzel
{
    template<class T>
    Vector2<T>::Vector2(const Vector3<T>& vec):
        v{vec.v[0], vec.v[1]}
    {
    }

    template<class T>
    Vector2<T>::Vector2(const Vector4<T>& vec):
    v{vec.v[0], vec.v[1]}
    {
    }

    template class Vector2<float>;

    template<class T>
    Vector3<T>::Vector3(const Vector4<T>& vec):
        v{vec.v[0], vec.v[1], vec.v[2]}
    {
    }

    template<class T>
    Vector3<T>::Vector3(Color color):
        v{color.normR(), color.normG(), color.normB()}
    {
    }

    template class Vector3<float>;

    template<class T>
    Vector4<T>::Vector4(Color color):
        v{color.normR(), color.normG(), color.normB(), color.normA()}
    {
    }

    template class Vector4<float>;
}
