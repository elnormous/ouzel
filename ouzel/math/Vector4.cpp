// Copyright 2015-2018 Elviss Strazdins. All rights reserved.

#include "Vector4.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Color.hpp"

namespace ouzel
{
    template<class T>
    Vector4<T>::Vector4(Color color)
    {
        v[0] = color.normR();
        v[1] = color.normG();
        v[2] = color.normB();
        v[3] = color.normA();
    }

    template class Vector4<float>;
}
