// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

struct VS2PS
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
    float2 TexCoord : TEXCOORD;
};

SamplerState Sampler0;
Texture2D Texture0;

void main(in VS2PS input, out float4 output0 : SV_TARGET0)
{
    output0 = Texture0.Sample(Sampler0, input.TexCoord) * input.Color;
}
