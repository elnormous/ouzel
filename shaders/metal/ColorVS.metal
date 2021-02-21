// Ouzel by Elviss Strazdins

#include <simd/simd.h>

using namespace metal;

typedef struct
{
    float4x4 modelViewProj;
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
    VS2PS output;
    output.position = uniforms.modelViewProj * float4(input.position, 1.0);
    output.color = input.color;
    return output;
}
