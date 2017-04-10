#include "Texture.h"

CTexture::CTexture(STextureType _type, bool _isRtv, bool _bitmap2D)
  : mType(_type), mSize(0, 0), mByteSize(0), mIsRtv(_isRtv), mBitmap2D(_bitmap2D), 
  mMipCount(1), mSizePadding(0, 0), mByteSizePadding(0) {
}

CTexture::~CTexture() {
}

bool CTexture::IsEmpty() const {
  bool empty = mSize.x == 0 || mSize.y == 0;
  return empty;
}

STextureType CTexture::GetType() const {
  return mType;
}

DirectX::XMUINT2 CTexture::GetSize() const {
  return mSize;
}

void CTexture::Resize(ID3D11Device* _d3dDev, ID2D1DeviceContext* _d2dCtx, 
  const DirectX::XMUINT2& _size, uint32_t _mipCount, float _dpiX, float _dpiY) {
  if (mSize.x != _size.x || mSize.y != _size.y || mMipCount != _mipCount || mPlaneSrv.empty()) {
    mSize = _size;
    mMipCount = _mipCount;

    mByteSize = 0;
    mPlaneSrv.clear();
    mPlaneRtv.clear();
    mBimaps.clear();

    if (!IsEmpty()) {
      size_t planeCount = 1;
      HRESULT hr = S_OK;

      for (size_t p = 0; p < planeCount; p++) {
        auto planeInfo = 1;
        Microsoft::WRL::ComPtr<ID3D11Texture2D> tex;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
        D3D11_TEXTURE2D_DESC texDesc;
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

        texDesc.Width = mSize.x;
        texDesc.Height  = mSize.y;
        texDesc.MipLevels  = mMipCount;
        texDesc.ArraySize  = 1;

        DXGI_FORMAT format;
        if (mType == STextureType::ABGR32) {
          format = DXGI_FORMAT_B8G8R8A8_UNORM;
        }
        if (mType == STextureType::R8) {
          format = DXGI_FORMAT_R8_UNORM;
        }

        texDesc.Format  = format;
        texDesc.Usage = D3D11_USAGE_DEFAULT;
        texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | (mIsRtv || mMipCount > 1 ? D3D11_BIND_RENDER_TARGET : 0);
        texDesc.CPUAccessFlags = 0;
        texDesc.MiscFlags = mMipCount > 1 ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;

        hr = _d3dDev->CreateTexture2D(&texDesc, nullptr, tex.GetAddressOf());

        srvDesc.Format = texDesc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = texDesc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;

        hr = _d3dDev->CreateShaderResourceView(tex.Get(), &srvDesc, srv.GetAddressOf());

        mPlaneSrv.push_back(srv);

        if (mIsRtv) {
          D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
          Microsoft::WRL::ComPtr<ID3D11RenderTargetView> rtv;

          rtvDesc.Format = texDesc.Format;
          rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
          rtvDesc.Texture2D.MipSlice = 0;

          hr = _d3dDev->CreateRenderTargetView(tex.Get(), &rtvDesc, rtv.GetAddressOf());

          mPlaneRtv.push_back(rtv);
        }

        if (mBitmap2D) {
          Microsoft::WRL::ComPtr<IDXGISurface2> surface;
          Microsoft::WRL::ComPtr<ID2D1Bitmap1>  bitmap;

          hr = tex.As(&surface);

          D2D1_BITMAP_PROPERTIES1 bitmapProperties =
            D2D1::BitmapProperties1(
              D2D1_BITMAP_OPTIONS_TARGET,
              D2D1::PixelFormat(texDesc.Format, D2D1_ALPHA_MODE_PREMULTIPLIED),
              _dpiX,
              _dpiY
            );

          hr = _d2dCtx->CreateBitmapFromDxgiSurface(surface.Get(), &bitmapProperties, bitmap.GetAddressOf());

          mBimaps.push_back(bitmap);
        }
      }
    }
  }
}

void CTexture::SetToContextRtv(ID3D11DeviceContext* _d3dCtx) {
  if (!mPlaneRtv.empty()) {
    _d3dCtx->OMSetRenderTargets(1, mPlaneRtv[0].GetAddressOf(), nullptr);
  }
}

void CTexture::SetToContextRtv(ID2D1DeviceContext* _d2dCtx) {
  if (!mBimaps.empty()) {
    _d2dCtx->SetTarget(mBimaps[0].Get());
  }
}

void CTexture::Clear(ID3D11DeviceContext* _d3dCtx, const float _color[4]) {
  for (size_t i = 0; i < mPlaneRtv.size(); i++) {
    _d3dCtx->ClearRenderTargetView(mPlaneRtv[i].Get(), _color);
  }
}

void CTexture::GetResource(ID3D11Resource** _resource) {
  mPlaneSrv[0]->GetResource(_resource);
}

Microsoft::WRL::ComPtr<ID2D1Bitmap1> CTexture::GetBitmap(size_t _planeIdx) {
  return mBimaps[_planeIdx];
}

void CTexture::GetBitmapBrush(ID2D1DeviceContext* _d2dCtx, ID2D1BitmapBrush1** _brush) {
  HRESULT hr = _d2dCtx->CreateBitmapBrush(mBimaps[0].Get(), _brush);
}

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CTexture::GetSrv(const unsigned int _idx) {
  return mPlaneSrv[_idx];
}
