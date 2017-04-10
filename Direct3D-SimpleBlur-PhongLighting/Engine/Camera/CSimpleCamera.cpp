#include "CSimpleCamera.h"

CSimpleCamera::CSimpleCamera()
  : mOutdated(false), mPosition(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)),
  mRotation(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)) {
}

CSimpleCamera::~CSimpleCamera() {
}

void CSimpleCamera::SetPosition(float _x, float _y, float _z) {
  mPosition = DirectX::XMFLOAT3(_x, _y, _z);
  mOutdated = true;
}

void CSimpleCamera::SetRotation(float _xDeg, float _yDeg, float _zDeg) {
  mRotation = DirectX::XMFLOAT3(_xDeg, _yDeg, _zDeg);
  mOutdated = true;
}

DirectX::XMFLOAT3 CSimpleCamera::GetPosition() {
  return mPosition;
}

DirectX::XMFLOAT3 CSimpleCamera::GetRotation() {
  return mRotation;
}

void CSimpleCamera::UpdateViewMatrix() {
  DirectX::XMVECTOR up;
  DirectX::XMVECTOR lookAt;
  DirectX::XMVECTOR position;
  float yaw;
  float pitch;
  float roll;
  DirectX::XMMATRIX rotationMatrix;

  position = DirectX::XMLoadFloat3(&mPosition);
  up = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
  lookAt = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f));

  // Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
  pitch = mRotation.x * 0.0174532925f;
  yaw = mRotation.y * 0.0174532925f;
  roll = mRotation.z * 0.0174532925f;

  rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

  lookAt = DirectX::XMVector3TransformCoord(lookAt, rotationMatrix);
  up = DirectX::XMVector3TransformCoord(up, rotationMatrix);
  lookAt = DirectX::XMVectorAdd(position, lookAt);

  mViewMatrix = DirectX::XMMatrixLookAtLH(position, lookAt, up);
}

void CSimpleCamera::GetViewMatrix(CAligned<DirectX::XMMATRIX, 16>& _view) {
  if (mOutdated) {
    UpdateViewMatrix();
  }
  _view = *mViewMatrix;
}
