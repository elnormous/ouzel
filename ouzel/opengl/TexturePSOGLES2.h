// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

const uint8_t TEXTURE_PIXEL_SHADER_OGL2[] =
    "precision mediump float;\n"
    "uniform lowp vec4 color;\n"
    "uniform lowp sampler2D texture0;\n"
    "varying lowp vec4 ex_Color;\n"
    "varying vec2 ex_TexCoord;\n"
    "void main(void)\n"
    "{\n"
    "    gl_FragColor = texture2D(texture0, ex_TexCoord) * ex_Color * color;\n"
    "}";
