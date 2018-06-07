#pragma once

#include "GameCanvas.h"

extern Graphics::WinHwnd objectMainWindow;
extern Graphics::PanelHwnd objectGameSubWindow;
extern Graphics::CanvasHwnd objectMainGameCanvas;
extern Graphics::PanelHwnd objectMenuSubWindow;


void InitGraphics();

void createMainMenu();