#pragma once
#include "Dx.h"
#include "Macros.h"

#include <utility>

class COMRenderTargetState {
public:
  NO_COPY(COMRenderTargetState);

  COMRenderTargetState(ID3D11DeviceContext* _d3dCtx)
    :mD3dCtx(_d3dCtx) {
    mD3dCtx->OMGetRenderTargets(1, mRtv.GetAddressOf(), mDsv.GetAddressOf());
  }

  COMRenderTargetState(COMRenderTargetState&& _other)
    : mD3dCtx(std::move(_other.mD3dCtx)),
    mRtv(std::move(_other.mRtv)),
    mDsv(std::move(_other.mDsv)) {
    _other.mD3dCtx = nullptr;
  }

  ~COMRenderTargetState() {
    if (mD3dCtx) {
      mD3dCtx->OMSetRenderTargets(1, mRtv.GetAddressOf(), mDsv.Get());
    }
  }

  COMRenderTargetState &operator=(COMRenderTargetState&& _other) {
    if (this != &_other) {
      mD3dCtx = std::move(_other.mD3dCtx);
      mRtv = std::move(_other.mRtv);
      mDsv = std::move(_other.mDsv);

      _other.mD3dCtx = nullptr;
    }

    return *this;
  }

private:
  ID3D11DeviceContext * mD3dCtx;

  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRtv;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDsv;
};