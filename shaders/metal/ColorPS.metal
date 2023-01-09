// Ouzel by Elviss Strazdins

#include <metal_stdlib>
#include <simd/simd.h>

using namespace metal;

struct Uniforms
{
    float4 color;
};

struct VS2PS
{
    float4 position [[position]];
    half4 color;
};

// Fragment shader function
fragment half4 mainPS(VS2PS input [[stage_in]],
                      constant Uniforms& uniforms [[buffer(1)]])
{
    return input.color * half4(uniforms.color);
}
