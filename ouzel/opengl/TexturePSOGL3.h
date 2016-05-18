// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

const uint8_t TEXTURE_PIXEL_SHADER_OGL3[] =
    "#version 330\n"
    "uniform vec4 color;\n"
    "uniform sampler2D texture0;\n"
    "in vec4 ex_Color;\n"
    "in vec2 ex_TexCoord;\n"
    "out vec4 out_Color;\n"
    "void main(void)\n"
    "{\n"
    "    out_Color = texture(texture0, ex_TexCoord) * ex_Color * color;\n"
    "}";
