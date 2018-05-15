#include "Render.h"

Graphics::WinHwnd objectMainWindow;
Graphics::PanelHwnd objectGameSubWindow;
Graphics::CanvasHwnd objectMainGameCanvas;
Graphics::PanelHwnd objectMenuSubWindow;

bool recivePacket(DataElement* data, int id, NetworkC* client, NetBinder* player);

void createMainMenu() {
  Graphics::setElements(objectMenuSubWindow, "html/main_menu.xml");

  reinterpret_cast<Graphics::LabelBindHwnd>(Graphics::getElementById("objectMainMenuVersion"))->text =
    new TextBind<
    TextBindVar<int>,
    TextBindVar<int>,
    TextBindVar<int>
    >("V%.%.%",
    TextBindVar<int>(&VersionA),
    TextBindVar<int>(&VersionB),
    TextBindVar<int>(&VersionC)
    );

  Graphics::deleteElements(objectGameSubWindow);

}
void createSettings(Graphics::TableHwnd& table) {
  for (auto&& it : table->data) {
    it->toDelete = true;
  }
  int i = 0;
  for (auto&& it : keybinds) {
    Graphics::TablerowHwnd row = Graphics::createTableRow("objectKeybindRow" + to_string(i), LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(1, 0)), getColor("tablerow", "bgcolor"));
    Graphics::LabelHwnd    name = Graphics::createLabel("objectKeybindLabel" + to_string(i), LocationData(LinearScale(0, 5), LinearScale(0, 25), LinearScale(0, 0), LinearScale(0.9, -50)), getColor("label", "bgcolor"), getColor("label", "activecolor"), getColor("label", "textcolor"), it.second, 1);
    Graphics::ControlHwnd  ctrl = Graphics::createControl("objectKeybindInput" + to_string(i), LocationData(LinearScale(0, 5), LinearScale(0, 25), LinearScale(0.9, -45), LinearScale(1, -5)), getColor("control", "bgcolor"), getColor("control", "activecolor"), getColor("control", "textcolor"), it.first, i, keybindReply);
    Graphics::addElement(row, name);
    Graphics::addElement(row, ctrl);
    Graphics::addElement(table, row);
    ++i;
  }
}
void connectServer(string ip, string port) {
  Graphics::deleteElements(objectMenuSubWindow);

  Connection = new NetworkC(
    ip,
    port,
    recivePacket
    );

  if (Connection->error != 0) {
    delete Connection;
    createMainMenu();
  } else {
    DataElement* data = new DataElement();

    DataElement* ae = new DataElement();
    ae->_core->fromTypeV<int>(VersionA);
    data->addChild(ae);

    DataElement* be = new DataElement();
    be->_core->fromTypeV<int>(VersionB);
    data->addChild(be);

    DataElement* ce = new DataElement();
    ce->_core->fromTypeV<int>(VersionC);
    data->addChild(ce);

    DataElement* ue = new DataElement();
    ue->_core->fromType<guid_t>(userGUID);
    data->addChild(ue);

    Connection->SendData(data, PacketLogin);
  }
}

//Main
void mainMenuSingleplayerPlayButton() {

}
void mainMenuMultiplayerPlayButton() {
  Graphics::setElements(objectMenuSubWindow, "html/multi_menu.xml");
}
void mainMenuSettingsButton() {
  Graphics::setElements(objectMenuSubWindow, "html/game_settings.xml");
  Graphics::TableHwnd table = (Graphics::TableHwnd)Graphics::getElementById("objectSettingsMenuTable");
  createSettings(table);
}
void mainMenuExitButton() {
  glutExit();
}

//Main / Multi
void multiMenuInput(string inp) {

}
void multiMenuInputButton() {

  string ip = reinterpret_cast<Graphics::TextInputHwnd>(Graphics::getElementById("objectMultiMenuIpInput"))->text;
  string port = reinterpret_cast<Graphics::TextInputHwnd>(Graphics::getElementById("objectMultiMenuPortInput"))->text;

  connectServer(ip, port);
}
void multiMenuBackButton() {
  createMainMenu();
}

//Main / Settings
void settingsMenuInput(key nkey, int id) {
  keybindReply(nkey, id);
}
void settingsMenuBackButton() {
  createMainMenu();
  saveKeybinds();
}


void GLAPIENTRY
MessageCallback(GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar* message,
  const void* userParam) {
  fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
    (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
    type, severity, message);
}


void InitGraphics() {
  //Main
  Graphics::setName("mainMenuSingleplayerPlayButton", mainMenuSingleplayerPlayButton);
  Graphics::setName("mainMenuMultiplayerPlayButton", mainMenuMultiplayerPlayButton);
  Graphics::setName("mainMenuSettingsButton", mainMenuSettingsButton);
  Graphics::setName("mainMenuExitButton", mainMenuExitButton);

  //Main / Multiplayer
  Graphics::setName("multiMenuInputButton", multiMenuInputButton);
  Graphics::setName("multiMenuInput", multiMenuInput);
  Graphics::setName("multiMenuBackButton", multiMenuBackButton);

  //Main / Settings
  Graphics::setName("settingsMenuBackButton", settingsMenuBackButton);


  //Validators
  Graphics::setName("textValidator", textValidator);
  Graphics::setName("numericalValidator", numericalValidator);
  Graphics::setName("floatValidator", floatValidator);



  glutInitContextVersion(4, 2);
  //glutInitContextFlags(GLUT_FORWARD_COMPATIBLE);
  glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);

  int argc = 0;
  char **argv = new char*[0];
  glutInit(&argc, argv);

  glutSetOption(
    GLUT_ACTION_ON_WINDOW_CLOSE,
    GLUT_ACTION_GLUTMAINLOOP_RETURNS
    );
  //Subwindows
  objectMainWindow = Graphics::CreateMainWindow("Game", Graphics::defaultWindowManagers, 640, 480, true);

  glewExperimental = GL_TRUE;
  GLenum err = glewInit();
  
  if (err != GLEW_OK) {
    cerr << "GLEW init error: " << glewGetErrorString(err) << endl;
  }
  cout << "Using GLEW " << glewGetString(GLEW_VERSION) << endl;

  if (GLEW_VERSION_4_2) {
    cout << "OpenGL 4.2 supported." << endl;
  }

  // During init, enable debug output
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);



  objectMenuSubWindow = Graphics::createPanel("objectMenuSubWindow", fullContainer, getColor("div", "bgcolor"));
  Graphics::addElement(objectMainWindow, objectMenuSubWindow);
  objectGameSubWindow = Graphics::createPanel("objectGameSubWindow", fullContainer, getColor("div", "bgcolor"));
  Graphics::addElement(objectMainWindow, objectGameSubWindow);

  chunkShader.create("Shaders/Chunk");

  int tw, th;
  textures = png_texture_load("Textures/All.png", tw, th);
  cout << "Loaded texture " << textures << endl;
}