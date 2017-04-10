#include "CTexturingShader.h"
#include "..\helpers\CResourceLoader.h"

CTexturingShader::CTexturingShader() {
  mLayoutDescLength = 2;
}

CTexturingShader::~CTexturingShader() {
}
//
//bool CBackgroundShader::InitializeShader(ID3D11Device * _d3dDevice, const wchar_t * _vsFilename, 
//  const wchar_t * _psFilename) {
//  std::vector<uint8_t> vsData;
//  std::vector<uint8_t> psData;
//
//  LoadShaders(_d3dDevice, _vsFilename, _psFilename, vsData, psData);
//
//  D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
//  InitializeInputLayout(polygonLayout);
//  HRESULT result = _d3dDevice->CreateInputLayout(polygonLayout, 2, vsData.data(),
//    vsData.size(), &mLayout);
//  if (FAILED(result)) {
//    return false;
//  }
//
//  CreateMatrixBuffer(_d3dDevice);
//  
//  D3D11_SAMPLER_DESC samplerDesc;
//  samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
//  samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
//  samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
//  samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
//  samplerDesc.MipLODBias = 0.0f;
//  samplerDesc.MaxAnisotropy = 1;
//  samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
//  samplerDesc.BorderColor[0] = 0;
//  samplerDesc.BorderColor[1] = 0;
//  samplerDesc.BorderColor[2] = 0;
//  samplerDesc.BorderColor[3] = 0;
//  samplerDesc.MinLOD = 0;
//  samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
//
//  result = _d3dDevice->CreateSamplerState(&samplerDesc, &mSampleState);
//  if (FAILED(result)) {
//    return false;
//  }
//
//  return true;
//}

void CTexturingShader::SetShaderToContext(ID3D11DeviceContext * _d3dContext) {
  IShader::SetShaderToContext(_d3dContext);
  _d3dContext->PSSetSamplers(0, 1, mSampleState.GetAddressOf());
}

bool CTexturingShader::CreateAdditionalBuffers(ID3D11Device * _d3dDevice) {

  D3D11_SAMPLER_DESC samplerDesc;
  samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
  samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
  samplerDesc.MipLODBias = 0.0f;
  samplerDesc.MaxAnisotropy = 1;
  samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
  samplerDesc.BorderColor[0] = 0;
  samplerDesc.BorderColor[1] = 0;
  samplerDesc.BorderColor[2] = 0;
  samplerDesc.BorderColor[3] = 0;
  samplerDesc.MinLOD = 0;
  samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

  HRESULT result = _d3dDevice->CreateSamplerState(&samplerDesc, &mSampleState);
  if (FAILED(result)) {
    return false;
  }

  return true;
}

void CTexturingShader::InitializeInputLayout(std::vector<D3D11_INPUT_ELEMENT_DESC>& _layouts) {
  IShader::InitializeInputLayout(_layouts);

  _layouts[1].SemanticName = "TEXCOORD";
  _layouts[1].SemanticIndex = 0;
  _layouts[1].Format = DXGI_FORMAT_R32G32_FLOAT;
  _layouts[1].InputSlot = 0;
  _layouts[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
  _layouts[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
  _layouts[1].InstanceDataStepRate = 0;
}
