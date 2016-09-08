// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

const uint8_t COLOR_PIXEL_SHADER_OGL2[] =
    "precision mediump float;\n"
    "uniform lowp vec4 color;\n"
    "varying lowp vec4 ex_Color;\n"
    "void main(void)\n"
    "{\n"
    "    gl_FragColor = ex_Color * color;\n"
    "}";
