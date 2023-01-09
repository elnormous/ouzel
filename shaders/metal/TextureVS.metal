// Ouzel by Elviss Strazdins

#include <simd/simd.h>

using namespace metal;

struct Uniforms
{
    float4x4 modelViewProj;
};

struct VSInput
{
    float3 position [[attribute(0)]];
    half4 color [[attribute(1)]];
    float2 texCoord [[attribute(2)]];
};

struct VS2PS
{
    float4 position [[position]];
    half4 color;
    float2 texCoord;
};

// Vertex shader function
vertex VS2PS mainVS(VSInput input [[stage_in]],
                    constant Uniforms& uniforms [[buffer(1)]])
{
    VS2PS output;
    output.position = uniforms.modelViewProj * float4(input.position, 1.0);
    output.color = input.color;
    output.texCoord = input.texCoord;
    return output;
}
