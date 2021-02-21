// Ouzel by Elviss Strazdins

cbuffer Constants: register(b0)
{
    float4 color;
};

struct VS2PS
{
    float4 position: SV_POSITION;
    float4 color: COLOR;
    float2 texCoord: TEXCOORD;
};

SamplerState sampler0;
Texture2D texture0;

void main(in VS2PS input, out float4 output0: SV_TARGET0)
{
    output0 = texture0.Sample(sampler0, input.texCoord) * input.color * color;
}
