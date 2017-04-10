#pragma once
#include "IShader.h"

class CPhongShader : public IShader {
public:
  CPhongShader();
  virtual ~CPhongShader();

  //virtual bool InitializeShader(ID3D11Device* _d3dDevice, const wchar_t* _vsName, const wchar_t* _psName) override;

  bool SetToContext(ID3D11DeviceContext* _d3dContext, CAligned<DirectX::XMMATRIX, 16> _worldMatrix,
    CAligned<DirectX::XMMATRIX, 16> _viewMatrix, CAligned<DirectX::XMMATRIX, 16> _projectionMatrix,
    MaterialProperties _material, LightProperties _light);

private:
  Microsoft::WRL::ComPtr<ID3D11Buffer> mLightBuffer;
  Microsoft::WRL::ComPtr<ID3D11Buffer> mMaterialBuffer;
  
  bool SetShaderParameters(ID3D11DeviceContext* _d3dContext, CAligned<DirectX::XMMATRIX, 16> _world,
    CAligned<DirectX::XMMATRIX, 16> _view, CAligned<DirectX::XMMATRIX, 16> _projection,
    MaterialProperties _material, LightProperties _light);

  void InitializeInputLayout(std::vector<D3D11_INPUT_ELEMENT_DESC>& _layouts) override;
  bool CreateAdditionalBuffers(ID3D11Device* _d3dDevice) override;
};
