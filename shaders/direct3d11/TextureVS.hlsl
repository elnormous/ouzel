// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

cbuffer Constants: register(b0)
{
    float4x4 modelViewProj;
};

struct VSInput
{
    float3 position: POSITION;
    float4 color: COLOR;
    float2 texCoord0: TEXCOORD0;
};

struct VS2PS
{
    float4 position: SV_POSITION;
    float4 color: COLOR;
    float2 texCoord: TEXCOORD;
};

void main(in VSInput input, out VS2PS output)
{
    output.position = mul(modelViewProj, float4(input.position, 1));
    output.color = input.color;
    output.texCoord = input.texCoord0;
}
