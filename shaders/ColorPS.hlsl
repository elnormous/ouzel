// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

struct VS2PS
{
    float4 Position : SV_POSITION;
    float4 Color : COLOR;
};

SamplerState Sampler0;

void main(in VS2PS input, out float4 output0 : SV_TARGET0)
{
    output0 = input.Color;
}
