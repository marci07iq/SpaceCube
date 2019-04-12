#pragma once

#include "GameCanvas.h"

extern Graphics::WinHwnd objectMainWindow;
extern Graphics::PanelHwnd objectGameSubWindow;
extern Graphics::CanvasHwnd objectMainGameCanvas;
extern Graphics::PanelHwnd objectMenuSubWindow;


void InitGraphics();

void createMainMenu();
void createMainMenu(Graphics::ElemHwnd e, void* d);
void createMessageScreen(string message, string button = "", ClickCallback buttonFunc = NULL, bool showButton = false, string imgname = "", bool showImage = false);

void connectServer(string ip, string port);