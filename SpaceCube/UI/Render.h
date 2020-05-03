#pragma once

#include "GameCanvas.h"

extern NGin::Graphics::WinHwnd objectMainWindow;
extern NGin::Graphics::PanelHwnd objectGameSubWindow;
extern NGin::Graphics::CanvasHwnd objectMainGameCanvas;
extern NGin::Graphics::PanelHwnd objectMenuSubWindow;


void InitGraphics();

void createMainMenu();
void createMainMenu(NGin::Graphics::ElemHwnd e);
void createMessageScreen(string message, string button = "", NGin::Graphics::ClickCallback buttonFunc = NULL, bool showButton = false, string imgname = "", bool showImage = false);

void connectServer(string ip, string port);