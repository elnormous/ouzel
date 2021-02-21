// Copyright 2015-2021 Elviss Strazdins. All rights reserved.

cbuffer Constants: register(b0)
{
    float4 color;
};

struct VS2PS
{
    float4 position: SV_POSITION;
    float4 color: COLOR;
};

void main(in VS2PS input, out float4 output0: SV_TARGET0)
{
    output0 = input.color * color;
}
