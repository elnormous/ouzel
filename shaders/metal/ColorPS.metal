// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

typedef struct
{
    float4 position [[position]];
    half4  color;
} ColorInOut;

// Fragment shader function
fragment half4 main_ps(ColorInOut in [[stage_in]])
{
    return in.color;
}
