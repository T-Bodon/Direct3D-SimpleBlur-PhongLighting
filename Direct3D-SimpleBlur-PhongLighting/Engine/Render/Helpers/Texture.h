#pragma once
#include "CResourceLoader.h" 
#include "Dx.h"

#include <vector>

enum class STextureType : int {
  Unknown = -1,

  I420,
  NV12,
  RGB32,
  ARGB32  = RGB32,
  ABGR32,
  R8,

  Count
};

class CTexture{
public:
  CTexture(
      STextureType _type, bool _isRtv = false, bool _bitmap2D = false);
  ~CTexture();

  bool IsEmpty() const;
  STextureType GetType() const;
  DirectX::XMUINT2 GetSize() const;

  void Resize(ID3D11Device* _d3dDev, ID2D1DeviceContext* _d2dCtx, 
    const DirectX::XMUINT2& _size, uint32_t _mipCount = 1, 
    float _dpiX = 96, float _dpiY = 96);
  void SetToContextRtv(ID3D11DeviceContext* _d3dCtx);
  void SetToContextRtv(ID2D1DeviceContext* _d2dCtx);
  void Clear(ID3D11DeviceContext* _d3dCtx, const float _color[4]);
  void GetResource(ID3D11Resource** _resource);

  Microsoft::WRL::ComPtr<ID2D1Bitmap1> GetBitmap(size_t _planeIdx = 0);
  
  void GetBitmapBrush(ID2D1DeviceContext* _d2dCtx, ID2D1BitmapBrush1** _brush);

  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetSrv(const unsigned int _idx = 0);

private:
  uint32_t mMipCount;
  STextureType mType;
  DirectX::XMUINT2 mSize;
  size_t mByteSize;
  bool mIsRtv;
  bool mBitmap2D;
  DirectX::XMUINT2 mSizePadding;
  size_t  mByteSizePadding;
  std::vector<Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> mPlaneSrv;
  std::vector<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>> mPlaneRtv;
  std::vector<Microsoft::WRL::ComPtr<ID2D1Bitmap1>> mBimaps;
};