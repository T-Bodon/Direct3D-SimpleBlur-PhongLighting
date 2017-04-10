#pragma once
#include "Dx.h"
#include "Macros.h"

class CD2DTargetState {
public:
  NO_COPY(CD2DTargetState);

  CD2DTargetState(ID2D1DeviceContext* _d2dCtx);
  CD2DTargetState(CD2DTargetState&& _other);
  ~CD2DTargetState();

  CD2DTargetState &operator=(CD2DTargetState&& _other);

  const DirectX::XMFLOAT2 &GetDPI() const;

private:
  ID2D1DeviceContext* mD2dCtx;
  DirectX::XMFLOAT2 mDpi;
  Microsoft::WRL::ComPtr<ID2D1Image> mRt;
};