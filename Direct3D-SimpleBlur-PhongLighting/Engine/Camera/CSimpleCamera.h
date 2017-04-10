#pragma once
#include "..\Render\helpers\Dx.h"
#include "..\Render\helpers\CAligned.h"

class CSimpleCamera {
public:
  CSimpleCamera();
  ~CSimpleCamera();

  void SetPosition(float _x, float _y, float _z);
  void SetRotation(float _xDeg, float _yDeg, float _zDeg);

  DirectX::XMFLOAT3 GetPosition();
  DirectX::XMFLOAT3 GetRotation();

  void GetViewMatrix(CAligned<DirectX::XMMATRIX, 16>& _view);

private:
  DirectX::XMFLOAT3 mPosition;
  DirectX::XMFLOAT3 mRotation;
  CAligned<DirectX::XMMATRIX, 16> mViewMatrix;

  bool mOutdated;

  void UpdateViewMatrix();
};