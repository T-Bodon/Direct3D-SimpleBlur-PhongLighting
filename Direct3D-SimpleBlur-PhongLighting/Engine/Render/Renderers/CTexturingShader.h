#pragma once
#include "IShader.h"

class CTexturingShader : public IShader {
public:
  CTexturingShader();
  virtual ~CTexturingShader();

  /*bool InitializeShader(ID3D11Device* _d3dDevice, const wchar_t* _vsFilename,
    const wchar_t* _psFilename) override;*/
 
private:
  Microsoft::WRL::ComPtr<ID3D11SamplerState> mSampleState;

  void SetShaderToContext(ID3D11DeviceContext* _d3dContext) override;
  void InitializeInputLayout(std::vector<D3D11_INPUT_ELEMENT_DESC>& _layouts) override;
  bool CreateAdditionalBuffers(ID3D11Device* _d3dDevice) override;
};