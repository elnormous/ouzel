// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

typedef struct
{
    float4 color;
} uniforms_t;

typedef struct
{
    float4 position [[position]];
    half4 color;
} VS2PS;

// Fragment shader function
fragment half4 mainPS(VS2PS input [[stage_in]],
                      constant uniforms_t& uniforms [[buffer(1)]])
{
    return input.color * half4(uniforms.color);
}
