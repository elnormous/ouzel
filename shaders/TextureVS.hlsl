// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

cbuffer Constants : register(b0)
{
    float4x4 modelViewProj;
}

struct VSInput
{
    float3 Position : POSITION;
    float4 Color : COLOR;
    float2 TexCoord0 : TEXCOORD0;
};

struct VS2PS
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
};

void main(in VSInput input, out VS2PS output)
{
    output.Position = mul(modelViewProj, float4(input.Position, 1));
    output.Color = input.Color;
    output.TexCoord = input.TexCoord0;
}
