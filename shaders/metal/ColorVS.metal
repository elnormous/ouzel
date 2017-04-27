// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <simd/simd.h>

using namespace metal;

typedef struct __attribute__((__aligned__(256)))
{
    matrix_float4x4 modelViewProj;
} uniforms_t;

typedef struct
{
    float3 position [[attribute(0)]];
    half4 color [[attribute(1)]];
} VSInput;

typedef struct
{
    float4 position [[position]];
    half4 color;
} VS2PS;

// Vertex shader function
vertex VS2PS mainVS(VSInput input [[stage_in]],
                    constant uniforms_t& uniforms [[buffer(1)]])
{
    VS2PS out;
    out.position = uniforms.modelViewProj * float4(input.position, 1.0);
    out.color = input.color;
    return out;
}
