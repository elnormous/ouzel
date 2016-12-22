// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

typedef struct __attribute__((__aligned__(256)))
{
    float4 color;
} uniforms_t;

typedef struct
{
    float4 position [[position]];
    half4  color;
    float2 texCoords;
} ColorInOut;

// Fragment shader function
fragment half4 main_ps(ColorInOut in [[stage_in]],
                       texture2d<float> tex2D [[texture(0)]],
                       sampler sampler2D [[sampler(0)]],
                       constant uniforms_t& uniforms [[buffer(1)]])
{
    return in.color * half4(tex2D.sample(sampler2D, in.texCoords)) * half4(uniforms.color);
}
