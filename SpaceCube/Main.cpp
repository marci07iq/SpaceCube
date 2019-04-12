#include "UI/Render.h"

void requestFrame() {
  while (Graphics::windows.size()) {
    this_thread::sleep_for(chrono::milliseconds(30));
    Graphics::requestRedraw();
  }
}

bool recivePacket(DataElement* data, int id, NetworkC* client, NetBinder* lplayer) {
  if (lplayer == NULL) {
    switch (id) {
      case PacketLogin:
        if (data->_children[0]->_core->toType<int>() == LoginErrorOk) {

          //vSFunc(userGUID, data->_children[1]);

          NetBinder* me = new NetBinder(userGUID);
          me->set(data->_children[1]);

          user = client->ConnectedBinder = lplayer = me;
          client->ConnectedBinder->connection = client;
          entities[userGUID] = user;
          user->loadChunks();

          Graphics::setElements(objectGameSubWindow, "html/game_screen.xml");

          objectMainGameCanvas = Graphics::createCanvas(
            "objectMainGameCanvas",
            fullContainer,
            IWindowManagers {
              MainGameCanvas::renderManager,
              MainGameCanvas::resizeManager,
              MainGameCanvas::guiEventManager,
              MainGameCanvas::mouseEntryManager,
              MainGameCanvas::mouseMoveManager,
            },
            NULL
          );
          Graphics::addElement(
            reinterpret_cast<Graphics::PanelHwnd>(Graphics::getElementById("objectMainGameCanvasContainer")),
            objectMainGameCanvas);

          bindGameScreenLabels();

          //glut_timer_CB(0);

          Graphics::requestRedraw();
        } else {
          //createMainMenu();
          createMessageScreen(data->_children[1]->_core->toType<string>(), "BACK", createMainMenu, true);
          Graphics::requestRedraw();
        }
        break;
    }
  } else {
    lplayer->recivePacket(data, id, client);
  }

  return 0;
}

int main() {
  Graphics::defaultMouseClickManager = Graphics::defaultMouseClickManagerNL;
  Graphics::defaultMouseEntryManager = Graphics::defaultMouseEntryManagerNL;
  Graphics::defaultMouseMoveManager = Graphics::defaultMouseMoveManagerNL;
  Graphics::defaultMouseWheelManager = Graphics::defaultMouseWheelManagerNL;
  Graphics::defaultRenderManager = Graphics::defaultRenderManagerNL;
  Graphics::defaultResizeManager = Graphics::defaultResizeManagerNL;
  Graphics::defaultGUIEventManager = Graphics::defaultGUIEventManagerNL;


  setlocale(LC_ALL, "");
  srand(time(NULL));
  ran1(time(NULL));

  loadKeybinds();
  loadColors();

  loadBlocks();

  InitGraphics();

  createMainMenu();

  thread t(requestFrame);

  //glutPostRedisplay();
  //glutMainLoop();
  Graphics::mainLoop();

  t.join();
}

int CALLBACK WinMain(
  HINSTANCE   hInstance,
  HINSTANCE   hPrevInstance,
  LPSTR       lpCmdLine,
  int         nCmdShow
  ) {
  return main();
}
