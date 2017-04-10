#pragma once

enum SimpleControls : unsigned int {
  ControlDirectional = 49,
  ControlPoint = 50,
  ControlSpot = 51,

  Controls_MAX,
  Controls_NONE = 0,
  Controls_Exit = 27
};

class CSimpleInput {
public:
  CSimpleInput ();
  ~CSimpleInput ();

  void KeyDown(const unsigned int _key);

  unsigned int GetLastKeyDown();

private:
  SimpleControls mLastPressed;
};