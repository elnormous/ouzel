// Copyright (C) 2016 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include "Widget.h"

namespace ouzel
{
    namespace gui
    {
        class CheckBox: public Widget
        {
        public:
            static std::shared_ptr<Button> create(const std::string& normal, const std::string& selected, const std::string& pressed, const std::string& disabled, const std::string& tick);

            CheckBox();
            virtual ~CheckBox();

            virtual bool init(const std::string& normal, const std::string& selected, const std::string& pressed, const std::string& disabled, const std::string& tick);

            virtual void setEnabled(bool enabled) override;

            virtual void setChecked(bool newChecked);
            virtual bool isChecked() const { return checked; }

        protected:
            bool handleGamepad(const GamepadEventPtr& event, const VoidPtr& sender);
            bool handleUI(const UIEventPtr& event, const VoidPtr& sender);

            void updateSprite();

            scene::SpritePtr normalSprite;
            scene::SpritePtr selectedSprite;
            scene::SpritePtr pressedSprite;
            scene::SpritePtr disabledSprite;
            scene::TextDrawablePtr labelDrawable;

            EventHandlerPtr eventHandler;

            bool checked = false;
        };
    } // namespace gui
} // namespace ouzel
