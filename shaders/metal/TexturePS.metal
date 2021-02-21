// Ouzel by Elviss Strazdins

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
    float2 texCoord;
} VS2PS;

// Fragment shader function
fragment half4 mainPS(VS2PS input [[stage_in]],
                      texture2d<float> tex2D [[texture(0)]],
                      sampler sampler2D [[sampler(0)]],
                      constant uniforms_t& uniforms [[buffer(1)]])
{
    return input.color * half4(tex2D.sample(sampler2D, input.texCoord)) * half4(uniforms.color);
}
