#include "UI/Render.h"

void requestFrame() {
  while (NGin::Graphics::windows.size()) {
    this_thread::sleep_for(chrono::milliseconds(30));
    NGin::Graphics::requestRedraw();
  }
}

bool recivePacket(DataElement* data, int id, Network* client, NetworkBinder* lplayer) {
  if (lplayer == NULL) {
    switch (id) {
      case PacketLogin:
        if (data->_children[0]->_core->toType<int>() == LoginErrorOk) {

          //vSFunc(userGUID, data->_children[1]);

          NetPlayer* me = new NetPlayer(userGUID);
          me->set(data->_children[1]);

          client->_binder = lplayer = me;
          user = me;

          client->_binder->connection = client;
          entities[userGUID] = user;
          user->loadChunks();

          NGin::Graphics::setElements(objectGameSubWindow, "html/game_screen.xml");

          objectMainGameCanvas = NGin::Graphics::createGUI_T<NGin::Graphics::Canvas>(
            "objectMainGameCanvas",
            NGin::Graphics::fullContainer,
            NGin::Graphics::IWindowManagers {
              MainGameCanvas::renderManager,
              MainGameCanvas::resizeManager,
              MainGameCanvas::guiEventManager,
              MainGameCanvas::mouseEntryManager,
              MainGameCanvas::mouseMoveManager,
            },
            (void*)NULL
          );
          NGin::Graphics::addElement(
            static_pointer_cast<NGin::Graphics::Panel>(NGin::Graphics::getElementById("objectMainGameCanvasContainer")),
            objectMainGameCanvas);

          bindGameScreenLabels();

          //glut_timer_CB(0);

          NGin::Graphics::requestRedraw();
        } else {
          //createMainMenu();
          createMessageScreen(data->_children[1]->_core->toType<string>(), "BACK", createMainMenu, true);
          NGin::Graphics::requestRedraw();
        }
        break;
    }
  } else {
    lplayer->recivePacket(data, id, client);
  }

  return 0;
}

int main() {
  NGin::Graphics::defaultMouseClickManager = NGin::Graphics::defaultMouseClickManagerNL;
  NGin::Graphics::defaultMouseEntryManager = NGin::Graphics::defaultMouseEntryManagerNL;
  NGin::Graphics::defaultMouseMoveManager = NGin::Graphics::defaultMouseMoveManagerNL;
  NGin::Graphics::defaultMouseWheelManager = NGin::Graphics::defaultMouseWheelManagerNL;
  NGin::Graphics::defaultRenderManager = NGin::Graphics::defaultRenderManagerNL;
  NGin::Graphics::defaultResizeManager = NGin::Graphics::defaultResizeManagerNL;
  NGin::Graphics::defaultGUIEventManager = NGin::Graphics::defaultGUIEventManagerNL;


  setlocale(LC_ALL, "");
  srand(time(NULL));
  ran1(time(NULL));

  NGin::Graphics::loadKeybinds();
  NGin::Graphics::loadColors();

  loadBlocks();

  InitGraphics();

  createMainMenu();

  thread t(requestFrame);

  //glutPostRedisplay();
  //glutMainLoop();
  NGin::Graphics::mainLoop();

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
