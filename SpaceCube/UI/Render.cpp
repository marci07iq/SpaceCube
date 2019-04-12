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
void createMainMenu(Graphics::ElemHwnd e, void* data) {
  createMainMenu();
}
void createMessageScreen(string message, string button, ClickCallback buttonFunc,  bool showButton, string imgname, bool showImage) {
  Graphics::deleteElements(objectMenuSubWindow);
  Graphics::deleteElements(objectGameSubWindow);

  Graphics::LabelHwnd lab = Graphics::createLabel("objectMessageText", LocationData(LinearScale(0.5),LinearScale(0.6),LinearScale(0),LinearScale(1)),getColor("label","bg"), getColor("label", "active"), getColor("label", "text"), NULL, message, 1);
  Graphics::addElement(objectMenuSubWindow, lab);

  if(showButton) {
    Graphics::ButtonHwnd but = Graphics::createButton("objectMessageButton", LocationData(LinearScale(0.4), LinearScale(0.5), LinearScale(0), LinearScale(1)), getColor("label", "bg"), getColor("label", "active"), getColor("label", "text"), NULL, button, -1, buttonFunc);
    Graphics::addElement(objectMenuSubWindow, but);
  }

  if (showImage) {
    Graphics::ImageHwnd img = Graphics::createImage("objectMessageImage", LocationData(LinearScale(0.6), LinearScale(0.9), LinearScale(0), LinearScale(1)), getColor("label", "bg"), getColor("label", "active"), getColor("label", "text"), NULL, imgname, 1);
    Graphics::addElement(objectMenuSubWindow, img);
  }
}
void createSettings(Graphics::TableHwnd& table) {
  Graphics::deleteElements(table, true);
  int i = 0;
  for (auto&& it : keybinds) {
    Graphics::TablerowHwnd row = Graphics::createTableRow("objectKeybindRow" + to_string(i), LocationData(LinearScale(0, 0), LinearScale(0, 30), LinearScale(0, 0), LinearScale(1, 0)), getColor("tablerow", "bgcolor"), getColor("tablerow", "activecolor"), NULL);
    Graphics::LabelHwnd    name = Graphics::createLabel("objectKeybindLabel" + to_string(i), LocationData(LinearScale(0, 5), LinearScale(0, 25), LinearScale(0, 0), LinearScale(0.9, -50)), getColor("label", "bgcolor"), getColor("label", "activecolor"), getColor("label", "textcolor"), NULL, it.second.display, 1);
    Graphics::ControlHwnd  ctrl = Graphics::createControl("objectKeybindInput" + to_string(i), LocationData(LinearScale(0, 5), LinearScale(0, 25), LinearScale(0.9, -45), LinearScale(1, -5)), getColor("control", "bgcolor"), getColor("control", "activecolor"), getColor("control", "textcolor"), NULL, it.second, it.first, keybindReply);
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
void mainMenuSingleplayerPlayButton(Graphics::ElemHwnd e, void* data) {

}
void mainMenuMultiplayerPlayButton(Graphics::ElemHwnd e, void* data) {
  Graphics::setElements(objectMenuSubWindow, "html/multi_menu.xml");
}
void mainMenuSettingsButton(Graphics::ElemHwnd e, void* data) {
  Graphics::setElements(objectMenuSubWindow, "html/game_settings.xml");
  Graphics::TableHwnd table = (Graphics::TableHwnd)Graphics::getElementById("objectSettingsMenuTable");
  createSettings(table);
}
void mainMenuExitButton(Graphics::ElemHwnd e, void* data) {
  Graphics::shutdownGraphics();
}

//Main / Multi
void multiMenuInput(Graphics::ElemHwnd e, void* data, string& s) {

}
void multiMenuInputButton(Graphics::ElemHwnd e, void* data) {

  string ip = reinterpret_cast<Graphics::TextInputHwnd>(Graphics::getElementById("objectMultiMenuIpInput"))->text;
  string port = reinterpret_cast<Graphics::TextInputHwnd>(Graphics::getElementById("objectMultiMenuPortInput"))->text;

  connectServer(ip, port);
}
void multiMenuBackButton(Graphics::ElemHwnd e, void* data) {
  createMainMenu();
}

//Main / Settings
/*void settingsMenuInput(key nkey, int id) {
  keybindReply(nkey, id);
}*/
void settingsMenuBackButton(Graphics::ElemHwnd e, void* data) {
  createMainMenu();
  saveKeybinds();
}


void GLAPIENTRY MessageCallback(GLenum source,
  GLenum type,
  GLuint id,
  GLenum severity,
  GLsizei length,
  const GLchar* message,
  const void* userParam) {
  if(severity != GL_DEBUG_SEVERITY_NOTIFICATION) {
  fprintf(stderr, "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
    "** GL ERROR **",
    type, severity, message);
    }
}

void glfwErrorCb(int i, const char* c) {
  cout << i << c << endl;
}

void mainWindowSetup(Graphics::WinHwnd win) {
  objectMainWindow = win;
  glewExperimental = GL_TRUE;

  glewInit();

  // get version info
  const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
  const GLubyte* version = glGetString(GL_VERSION); // version as a string
  printf("Renderer: %s\n", renderer);
  printf("OpenGL version supported %s\n", version);

  // During init, enable debug output
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  glDisable(GL_DITHER);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);
  //glDisable(GL_POINT_SMOOTH);
  glDisable(GL_LINE_SMOOTH);
  glDisable(GL_POLYGON_SMOOTH);

  Gll::gllInit("NGin/GUI/GLL_Res/");

  win->autoRedraw = true;

  objectMenuSubWindow = Graphics::createPanel("objectMenuSubWindow", fullContainer, getColor("div", "bgcolor"), NULL);
  Graphics::addElement(objectMainWindow, objectMenuSubWindow);
  objectGameSubWindow = Graphics::createPanel("objectGameSubWindow", fullContainer, getColor("div", "bgcolor"), NULL);
  Graphics::addElement(objectMainWindow, objectGameSubWindow);

  chunkShader.create("Shaders/Chunk");
  entityShader.create("Shaders/Entity");

  int tw, th;
  textures = png_texture_load("Textures/All.png", tw, th);
  cout << "Loaded texture " << textures << endl;

  //createMainMenu();
}
void InitGraphics() {
  //Main
  Graphics::setName<ClickCallback>("mainMenuSingleplayerPlayButton", mainMenuSingleplayerPlayButton);
  Graphics::setName<ClickCallback>("mainMenuMultiplayerPlayButton", mainMenuMultiplayerPlayButton);
  Graphics::setName<ClickCallback>("mainMenuSettingsButton", mainMenuSettingsButton);
  Graphics::setName<ClickCallback>("mainMenuExitButton", mainMenuExitButton);

  //Main / Multiplayer
  Graphics::setName<ClickCallback>("multiMenuInputButton", multiMenuInputButton);
  Graphics::setName<TextInputFunc>("multiMenuInput", multiMenuInput);
  Graphics::setName<ClickCallback>("multiMenuBackButton", multiMenuBackButton);

  //Main / Settings
  Graphics::setName<ClickCallback>("settingsMenuBackButton", settingsMenuBackButton);


  //Validators
  Graphics::setName<TextValidatorFunc>("textValidator", textValidator);
  Graphics::setName<TextValidatorFunc>("numericalValidator", numericalValidator);
  Graphics::setName<TextValidatorFunc>("floatValidator", floatValidator);


  Graphics::initGraphics();
  glfwSetErrorCallback(glfwErrorCb);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

  Graphics::CreateMainWindow("SpaceCube", Graphics::defaultWindowManagers, 1080, 768, true, 0, 0, false, 0, NULL, mainWindowSetup);

  Graphics::cleanQueues();
}
