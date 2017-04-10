#pragma once
#include "IShader.h"

class COpacityMaskShader : public IShader {
public:
  COpacityMaskShader(float _textureWidth);
  virtual ~COpacityMaskShader();

 /* bool InitializeShader(ID3D11Device* _d3dDevice, const wchar_t* _vsFilename,
    const wchar_t* _psFilename) override;*/

private:
  float mTextureWidth;
  Microsoft::WRL::ComPtr<ID3D11Buffer> mTextWidthBuffer;

  bool SetShaderParameters(ID3D11DeviceContext* _d3dContext, CAligned<DirectX::XMMATRIX, 16> _world,
    CAligned<DirectX::XMMATRIX, 16> _view, CAligned<DirectX::XMMATRIX, 16> _ortho) override;
  bool CreateAdditionalBuffers(ID3D11Device* _d3dDevice) override;
};