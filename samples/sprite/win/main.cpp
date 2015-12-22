// Copyright (C) 2015 Elviss Strazdins
// This file is part of the Ouzel engine.

#include <Windows.h>
#include "Application.h"

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow)
{
	ouzel::Application* application = new ouzel::Application();
	//TODO: create main window
	//TODO: attach Renderer's view to this window
	//TODO: run main loop

	return 0;
}