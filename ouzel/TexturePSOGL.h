// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

static const char TEXTURE_PIXEL_SHADER_OGL[] =
    "#version 400\n"
    "uniform sampler2D tex;\n"
    "in vec4 ex_Color;\n"
    "in vec2 ex_TexCoord;\n"
    "out vec4 out_Color;\n"
    "void main(void)\n"
    "{\n"
    "    out_Color = texture(tex, ex_TexCoord) * ex_Color;\n"
    "}";
