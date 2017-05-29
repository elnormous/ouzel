// Copyright (C) 2017 Elviss Strazdins
// This file is part of the Ouzel engine.

#pragma once

#include <dispatch/dispatch.h>
#include "core/Application.h"

namespace ouzel
{
    class ApplicationMacOS: public Application
    {
    public:
        ApplicationMacOS(int aArgc, char* aArgv[]);
        virtual ~ApplicationMacOS();

        virtual int run() override;
        virtual void exit() override;

        virtual void execute(const std::function<void(void)>& func) override;

        virtual bool openURL(const std::string& url) override;

        virtual void setScreenSaverEnabled(bool newScreenSaverEnabled) override;

    protected:
        virtual void activateCursorResource(CursorResource* resource) override;
        virtual CursorResource* createCursorResource() override;

        dispatch_queue_t mainQueue;
        uint32_t noSleepAssertionID = 0;
    };
}
