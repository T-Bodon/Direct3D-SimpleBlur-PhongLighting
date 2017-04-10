#include "CSimpleInput.h"

CSimpleInput::CSimpleInput() {
  mLastPressed = SimpleControls::Controls_NONE;
}

CSimpleInput::~CSimpleInput() {
}

void CSimpleInput::KeyDown(unsigned int _key) {
  if (_key == SimpleControls::Controls_Exit) {
    mLastPressed = SimpleControls::Controls_Exit;
    return;
  }

  mLastPressed = SimpleControls::Controls_NONE;
  for (unsigned int i = SimpleControls::ControlDirectional; i < SimpleControls::Controls_MAX; i++) {
    if (_key == i) {
      mLastPressed = (SimpleControls)i;
    }
  }
}

unsigned int CSimpleInput::GetLastKeyDown() {
  return mLastPressed;
}
