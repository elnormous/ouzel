// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

const uint8_t TEXTURE_PIXEL_SHADER_OGL[] =
    "precision mediump float;\n"
    "uniform sampler2D tex;\n"
    "varying vec4 ex_Color;\n"
    "varying vec2 ex_TexCoord;\n"
    "void main(void)\n"
    "{\n"
    "    gl_FragColor = texture2D(tex, ex_TexCoord) * ex_Color;\n"
    "}";
