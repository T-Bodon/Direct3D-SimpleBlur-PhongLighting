#include "Engine\CSystem.h"

int WINAPI WinMain(HINSTANCE _hInstance, HINSTANCE _hPrevInstance, PSTR _pScmdline, int _iCmdshow) {
  std::unique_ptr<CSystem> system;

  system = std::make_unique<CSystem>();
  if (!system)
    return 0;

  system->Run();
  
  return 0;
}