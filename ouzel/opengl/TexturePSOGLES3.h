// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

const uint8_t TEXTURE_PIXEL_SHADER_OGL3[] =
    "#version 300 es\n"
    "precision mediump float;\n"
    "uniform lowp vec4 color;\n"
    "uniform lowp sampler2D texture0;\n"
    "in lowp vec4 ex_Color;\n"
    "in vec2 ex_TexCoord;\n"
    "out vec4 out_Color;\n"
    "void main(void)\n"
    "{\n"
    "    out_Color = texture(texture0, ex_TexCoord) * ex_Color * color;\n"
    "}";
