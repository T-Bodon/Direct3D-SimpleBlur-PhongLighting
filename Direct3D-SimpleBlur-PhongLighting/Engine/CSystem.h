#pragma once
#define WIN32_LEAN_AND_MEAN

#include "Input\CSimpleInput.h"
#include "Render\CSimpleGraphics.h"

#include <Windows.h>

class CSystem {
public:
  CSystem();
  ~CSystem();

  void Run();

  LRESULT CALLBACK MessageHandler(HWND _hwnd, const UINT _umsg, WPARAM _wparam, LPARAM _lparam);

private:
  bool ProcessFrame();
  void InitializeWindows(int& _screenWidth, int& _screenHeight);
  void ShutdownWindows();

  LPCWSTR mApplicationName;
  HINSTANCE mHinstance;
  HWND mHwnd;

  std::unique_ptr<CSimpleInput> mInput;
  std::unique_ptr<CSimpleGraphics> mGraphics;
};

LRESULT CALLBACK WndProc(HWND _hwnd, UINT _umessage, WPARAM _wparam, LPARAM _lparam);

static CSystem * gApplicationHandle = 0;