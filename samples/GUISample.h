// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

class GUISample: public ouzel::scene::Scene
{
public:
    GUISample();
    virtual ~GUISample() {}
private:
    ouzel::gui::ButtonPtr button;
    ouzel::gui::CheckBoxPtr checkBox;
};
