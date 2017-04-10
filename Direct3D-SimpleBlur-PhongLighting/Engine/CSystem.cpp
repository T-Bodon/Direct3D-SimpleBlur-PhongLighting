#include "CSystem.h"

CSystem::CSystem() {
  int screenWidth = 0;
  int screenHeight = 0;

  // Initialize the windows api.
  InitializeWindows(screenWidth, screenHeight);

  mInput = std::make_unique<CSimpleInput>();

  mGraphics = std::make_unique<CSimpleGraphics>(screenWidth, screenHeight, mHwnd);

}

CSystem::~CSystem() {
  // Shutdown the window.
  ShutdownWindows();
}

void CSystem::Run() {
  MSG msg;
  bool done;
  bool result;

  ZeroMemory(&msg, sizeof(MSG));

  // Loop until there is a quit message from the window or the user.
  done = false;
  while (!done) {
    if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }

    if (msg.message == WM_QUIT) {
      done = true;
    }
    else {
      result = ProcessFrame();
      if (!result) {
        done = true;
      }
    }
  }
}

LRESULT CALLBACK CSystem::MessageHandler(HWND _hwnd, const UINT _umsg, 
  WPARAM _wparam, LPARAM _lparam) {
  switch (_umsg) {
  case WM_KEYDOWN:
    mInput->KeyDown((unsigned int)_wparam);
    return 0;

 /* case WM_KEYUP:
    mInput->KeyUp((unsigned int)_wparam);
    return 0;*/

  default:
    return DefWindowProc(_hwnd, _umsg, _wparam, _lparam);

  }
}

bool CSystem::ProcessFrame() {
  if (mInput->GetLastKeyDown() == VK_ESCAPE) {
    return false;
  }

  return mGraphics->ProcessFrame(mInput->GetLastKeyDown());
}

void CSystem::InitializeWindows(int& _screenWidth, int& _screenHeight) {
  WNDCLASSEX wc;
  DEVMODE dmScreenSettings;
  int posX;
  int posY;

  // Get an external pointer to this object.
  gApplicationHandle = this;

  // Get the instance of this application.
  mHinstance = GetModuleHandle(NULL);

  // Give the application a name.
  mApplicationName = L"Simple Rendering Engine";

  // Setup the windows class with default settings.
  wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
  wc.lpfnWndProc = WndProc;
  wc.cbClsExtra = 0;
  wc.cbWndExtra = 0;
  wc.hInstance = mHinstance;
  wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
  wc.hIconSm = wc.hIcon;
  wc.hCursor = LoadCursor(NULL, IDC_ARROW);
  wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wc.lpszMenuName = NULL;
  wc.lpszClassName = mApplicationName;
  wc.cbSize = sizeof(WNDCLASSEX);

  RegisterClassEx(&wc);

  _screenWidth = GetSystemMetrics(SM_CXSCREEN);
  _screenHeight = GetSystemMetrics(SM_CYSCREEN);

  if (global::gFullScreen) {
    // If full screen set the screen to maximum size of the users desktop and 32bit.
    memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));

    dmScreenSettings.dmSize = sizeof(dmScreenSettings);
    dmScreenSettings.dmPelsWidth  = (unsigned long)_screenWidth;
    dmScreenSettings.dmPelsHeight = (unsigned long)_screenHeight;
    dmScreenSettings.dmBitsPerPel = 32;
    dmScreenSettings.dmFields  = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

    // Change the display settings to full screen.
    ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

    posX = posY = 0;
  }
  else {
    _screenWidth  = 512;
    _screenHeight = 512;

    posX = (GetSystemMetrics(SM_CXSCREEN) - _screenWidth) / 2;
    posY = (GetSystemMetrics(SM_CYSCREEN) - _screenHeight) / 2;
  }

  mHwnd = CreateWindowEx(WS_EX_APPWINDOW, mApplicationName, mApplicationName,
    WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
    posX, posY, _screenWidth, _screenHeight, NULL, NULL, mHinstance, NULL);

  ShowWindow(mHwnd, SW_SHOW);
  SetForegroundWindow(mHwnd);
  SetFocus(mHwnd);

  ShowCursor(false);
}

void CSystem::ShutdownWindows() {
  ShowCursor(true);

  if (global::gFullScreen) {
    ChangeDisplaySettings(NULL, 0);
  }

  DestroyWindow(mHwnd);
  mHwnd = nullptr;

  UnregisterClass(mApplicationName, mHinstance);
  mHinstance = nullptr;

  gApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc(HWND _hwnd, UINT _umessage, WPARAM _wparam, LPARAM _lparam) {
  switch (_umessage) {
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;

  case WM_CLOSE:
    PostQuitMessage(0);
    return 0;

  default:
    return gApplicationHandle->MessageHandler(_hwnd, _umessage, _wparam, _lparam);
  
  }
}
