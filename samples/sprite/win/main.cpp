// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <windows.h>
#include "../Application.h"

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	ouzel::Application application;
	
	while (application.getEngine()->getRenderer()->getView()->processEvents())
	{
		application.getEngine()->run();
	}
	
	return 0;
}