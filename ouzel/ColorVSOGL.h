// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

static const char COLOR_VERTEX_SHADER_OGL[] =
    "#version 400\n"
    "layout(location=0) in vec4 in_Position;\n"
    "layout(location=1) in vec4 in_Color;\n"
    "uniform mat4 model;\n"
    "uniform mat4 view;\n"
    "uniform mat4 proj;\n"
    "out vec4 ex_Color;\n"
    "void main(void)\n"
    "{\n"
    "    gl_Position = proj * view * model * in_Position;\n"
    "    ex_Color = in_Color;\n"
    "}";
