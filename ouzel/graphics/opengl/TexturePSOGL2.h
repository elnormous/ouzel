// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

const uint8_t TEXTURE_PIXEL_SHADER_OGL2[] =
    "#version 120\n"
    "uniform vec4 color;\n"
    "uniform sampler2D texture0;\n"
    "varying vec4 ex_Color;\n"
    "varying vec2 ex_TexCoord;\n"
    "void main(void)\n"
    "{\n"
    "    gl_FragColor = texture2D(texture0, ex_TexCoord) * ex_Color * color;\n"
    "}";
