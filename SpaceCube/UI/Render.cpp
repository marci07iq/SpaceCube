#include "Render.h"

NGin::Graphics::WinHwnd objectMainWindow;
NGin::Graphics::PanelHwnd objectGameSubWindow;
NGin::Graphics::CanvasHwnd objectMainGameCanvas;
NGin::Graphics::PanelHwnd objectMenuSubWindow;

bool recivePacket(DataElement* data, int id, Network* client, NetworkBinder* player);

void createMainMenu() {
  NGin::Graphics::setElements(objectMenuSubWindow, "html/main_menu.xml");

  static_pointer_cast<NGin::Graphics::LabelBind>(NGin::Graphics::getElementById("objectMainMenuVersion"))->text =
    new TextBind<
    TextBindVar<int>,
    TextBindVar<int>,
    TextBindVar<int>
    >("V%.%.%",
    TextBindVar<int>(&VersionA),
    TextBindVar<int>(&VersionB),
    TextBindVar<int>(&VersionC)
    );

  NGin::Graphics::deleteElements(objectGameSubWindow);
}
void createMainMenu(NGin::Graphics::ElemHwnd e) {
  createMainMenu();
}
void createMessageScreen(string message, string button, NGin::Graphics::ClickCallback buttonFunc,  bool showButton, string imgname, bool showImage) {
  NGin::Graphics::deleteElements(objectMenuSubWindow);
  NGin::Graphics::deleteElements(objectGameSubWindow);

  NGin::Graphics::LabelHwnd lab = NGin::Graphics::createGUI_T<NGin::Graphics::Label>("objectMessageText", NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0.5), NGin::Graphics::LinearScale(0.6), NGin::Graphics::LinearScale(0), NGin::Graphics::LinearScale(1)), NGin::Graphics::getColor("label","bg"), NGin::Graphics::getColor("label", "active"), NGin::Graphics::getColor("label", "text"), (void*)NULL, message, 1);
  NGin::Graphics::addElement(objectMenuSubWindow, lab);

  if(showButton) {
    NGin::Graphics::ButtonHwnd but = NGin::Graphics::createGUI_T<NGin::Graphics::Button>("objectMessageButton", NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0.4), NGin::Graphics::LinearScale(0.5), NGin::Graphics::LinearScale(0), NGin::Graphics::LinearScale(1)), NGin::Graphics::getColor("label", "bg"), NGin::Graphics::getColor("label", "active"), NGin::Graphics::getColor("label", "text"), (void*)NULL, button, -1, buttonFunc);
    NGin::Graphics::addElement(objectMenuSubWindow, but);
  }

  if (showImage) {
    NGin::Graphics::ImageHwnd img = NGin::Graphics::createGUI_T<NGin::Graphics::Image>("objectMessageImage", NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0.6), NGin::Graphics::LinearScale(0.9), NGin::Graphics::LinearScale(0), NGin::Graphics::LinearScale(1)), NGin::Graphics::getColor("label", "bg"), NGin::Graphics::getColor("label", "active"), NGin::Graphics::getColor("label", "text"), (void*)NULL, imgname, 1);
    NGin::Graphics::addElement(objectMenuSubWindow, img);
  }
}
void createSettings(NGin::Graphics::TableHwnd& table) {
  NGin::Graphics::deleteElements(table);
  int i = 0;
  for (auto&& it : NGin::Graphics::keybinds) {
    NGin::Graphics::TablerowHwnd row = NGin::Graphics::createGUI_T< NGin::Graphics::TableRow>("objectKeybindRow" + to_string(i), NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0, 30), NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(1, 0)), NGin::Graphics::getColor("tablerow", "bgcolor"), NGin::Graphics::getColor("tablerow", "activecolor"), (void*)NULL);
    NGin::Graphics::LabelHwnd    name = NGin::Graphics::createGUI_T< NGin::Graphics::Label>("objectKeybindLabel" + to_string(i), NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0, 5), NGin::Graphics::LinearScale(0, 25), NGin::Graphics::LinearScale(0, 0), NGin::Graphics::LinearScale(0.9, -50)), NGin::Graphics::getColor("label", "bgcolor"), NGin::Graphics::getColor("label", "activecolor"), NGin::Graphics::getColor("label", "textcolor"), (void*)NULL, it.second.display, 1);
    NGin::Graphics::ControlHwnd  ctrl = NGin::Graphics::createGUI_T< NGin::Graphics::ControlSetting>("objectKeybindInput" + to_string(i), NGin::Graphics::LocationData(NGin::Graphics::LinearScale(0, 5), NGin::Graphics::LinearScale(0, 25), NGin::Graphics::LinearScale(0.9, -45), NGin::Graphics::LinearScale(1, -5)), NGin::Graphics::getColor("control", "bgcolor"), NGin::Graphics::getColor("control", "activecolor"), NGin::Graphics::getColor("control", "textcolor"), (void*)NULL, it.second, it.first, NGin::Graphics::keybindReply);
    NGin::Graphics::addElement(row, name);
    NGin::Graphics::addElement(row, ctrl);
    NGin::Graphics::addElement(table, row);
    ++i;
  }
}
void connectServer(string ip, string port) {
  NGin::Graphics::deleteElements(objectMenuSubWindow);

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
void mainMenuSingleplayerPlayButton(NGin::Graphics::ElemHwnd e) {

}
void mainMenuMultiplayerPlayButton(NGin::Graphics::ElemHwnd e) {
  NGin::Graphics::setElements(objectMenuSubWindow, "html/multi_menu.xml");
}
void mainMenuSettingsButton(NGin::Graphics::ElemHwnd e) {
  NGin::Graphics::setElements(objectMenuSubWindow, "html/game_settings.xml");
  NGin::Graphics::TableHwnd table = static_pointer_cast<NGin::Graphics::Table>(NGin::Graphics::getElementById("objectSettingsMenuTable"));
  createSettings(table);
}
void mainMenuExitButton(NGin::Graphics::ElemHwnd e) {
  NGin::Graphics::shutdownGraphics();
}

//Main / Multi
void multiMenuInput(NGin::Graphics::ElemHwnd e, string& s) {

}
void multiMenuInputButton(NGin::Graphics::ElemHwnd e) {

  string ip = static_pointer_cast<NGin::Graphics::TextInput>(NGin::Graphics::getElementById("objectMultiMenuIpInput"))->text;
  string port = static_pointer_cast<NGin::Graphics::TextInput>(NGin::Graphics::getElementById("objectMultiMenuPortInput"))->text;

  connectServer(ip, port);
}
void multiMenuBackButton(NGin::Graphics::ElemHwnd e) {
  createMainMenu();
}

//Main / Settings
/*void settingsMenuInput(key nkey, int id) {
  keybindReply(nkey, id);
}*/
void settingsMenuBackButton(NGin::Graphics::ElemHwnd e) {
  createMainMenu();
  NGin::Graphics::saveKeybinds();
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

void mainWindowSetup(NGin::Graphics::WinHwnd win) {
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

  //glEnable(GL_CULL_FACE);
  //glCullFace(GL_FRONT);
  //glFrontFace(GL_CCW);

  NGin::Gll::gllInit("NGin/GUI/GLL_Res/");

  win->autoRedraw = true;

  objectMenuSubWindow = NGin::Graphics::createGUI_T< NGin::Graphics::Panel>("objectMenuSubWindow", NGin::Graphics::fullContainer, NGin::Graphics::getColor("div", "bgcolor"), (void*)NULL);
  NGin::Graphics::addElement(objectMainWindow, objectMenuSubWindow);
  objectGameSubWindow = NGin::Graphics::createGUI_T< NGin::Graphics::Panel>("objectGameSubWindow", NGin::Graphics::fullContainer, NGin::Graphics::getColor("div", "bgcolor"), (void*)NULL);
  NGin::Graphics::addElement(objectMainWindow, objectGameSubWindow);

  chunkShader = make_shared<NGin::Graphics::Shader_Raw>("Shaders/Chunk");
  entityShader = make_shared<NGin::Graphics::Shader_Raw>("Shaders/Entity");

  int tw, th;
  textures = NGin::Graphics::png_texture_load("Textures/All.png", tw, th);
  cout << "Loaded texture " << textures << endl;

  //createMainMenu();
}
void InitGraphics() {
  //Main
  NGin::Graphics::setName<NGin::Graphics::ClickCallback>("mainMenuSingleplayerPlayButton", mainMenuSingleplayerPlayButton);
  NGin::Graphics::setName<NGin::Graphics::ClickCallback>("mainMenuMultiplayerPlayButton", mainMenuMultiplayerPlayButton);
  NGin::Graphics::setName<NGin::Graphics::ClickCallback>("mainMenuSettingsButton", mainMenuSettingsButton);
  NGin::Graphics::setName<NGin::Graphics::ClickCallback>("mainMenuExitButton", mainMenuExitButton);

  //Main / Multiplayer
  NGin::Graphics::setName<NGin::Graphics::ClickCallback>("multiMenuInputButton", multiMenuInputButton);
  NGin::Graphics::setName<NGin::Graphics::TextInputFunc>("multiMenuInput", multiMenuInput);
  NGin::Graphics::setName<NGin::Graphics::ClickCallback>("multiMenuBackButton", multiMenuBackButton);

  //Main / Settings
  NGin::Graphics::setName<NGin::Graphics::ClickCallback>("settingsMenuBackButton", settingsMenuBackButton);


  //Validators
  NGin::Graphics::setName<NGin::Graphics::TextValidatorFunc>("textValidator", NGin::Graphics::textValidator);
  NGin::Graphics::setName<NGin::Graphics::TextValidatorFunc>("numericalValidator", NGin::Graphics::numericalValidator);
  NGin::Graphics::setName<NGin::Graphics::TextValidatorFunc>("floatValidator", NGin::Graphics::floatValidator);


  NGin::Graphics::initGraphics();
  glfwSetErrorCallback(glfwErrorCb);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 4.3
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

  NGin::Graphics::CreateMainWindow("SpaceCube", NGin::Graphics::defaultWindowManagers, 1080, 768, true, 0, 0, false, 0, NULL, mainWindowSetup);

  NGin::Graphics::cleanQueues();
}
