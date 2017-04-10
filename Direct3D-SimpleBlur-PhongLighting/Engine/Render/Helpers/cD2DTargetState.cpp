#include "cD2DTargetState.h"

#include <utility>

CD2DTargetState::CD2DTargetState(ID2D1DeviceContext* _d2dCtx)
  : mD2dCtx(_d2dCtx) {
  mD2dCtx->GetDpi(&mDpi.x, &mDpi.y);
  mD2dCtx->GetTarget(mRt.GetAddressOf());
}

CD2DTargetState::CD2DTargetState(CD2DTargetState&& _other)
  : mD2dCtx(std::move(_other.mD2dCtx)), mDpi(std::move(_other.mDpi)),
  mRt(std::move(_other.mRt)) {
  _other.mD2dCtx = nullptr;
}

CD2DTargetState::~CD2DTargetState() {
  if (mD2dCtx) {
    mD2dCtx->SetDpi(mDpi.x, mDpi.y);
    mD2dCtx->SetTarget(mRt.Get());
  }
}

CD2DTargetState &CD2DTargetState::operator=(CD2DTargetState && _other) {
  if (this != &_other) {
    mD2dCtx = std::move(_other.mD2dCtx);
    mDpi = std::move(_other.mDpi);
    mRt = std::move(_other.mRt);

    _other.mD2dCtx = nullptr;
  }

  return *this;
}

const DirectX::XMFLOAT2 &CD2DTargetState::GetDPI() const {
  return mDpi;
}