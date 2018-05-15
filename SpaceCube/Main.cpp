#include "UI/Render.h"

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

          Graphics::setElements(objectGameSubWindow, "html/game_screen.xml");

          objectMainGameCanvas = Graphics::createCanvas(
            "objectMainGameCanvas",
            fullContainer,
            IWindowManagers{
            MainGameCanvas::renderManager,
            MainGameCanvas::resizeManager,
            MainGameCanvas::guiEventManager,
            MainGameCanvas::mouseEntryManager,
            MainGameCanvas::mouseMoveManager,
          }
          );
          Graphics::addElement(
            reinterpret_cast<Graphics::PanelHwnd>(Graphics::getElementById("objectMainGameCanvasContainer")),
            objectMainGameCanvas);

          bindGameScreenLabels();

          glut_timer_CB(0);

          glutPostRedisplay();
        } else {
          createMainMenu();
          glutPostRedisplay();
        }
        break;
    }
  } else {
    lplayer->recivePacket(data, id, client);
  }

  return 0;
}

int main() {
  setlocale(LC_ALL, "");
  srand(time(NULL));
  ran1(time(NULL));

  loadKeybinds();
  loadColors();

  loadBlocks();

  InitGraphics();

  createMainMenu();

  glutPostRedisplay();
  glutMainLoop();
}

int CALLBACK WinMain(
  HINSTANCE   hInstance,
  HINSTANCE   hPrevInstance,
  LPSTR       lpCmdLine,
  int         nCmdShow
  ) {
  return main();
}
