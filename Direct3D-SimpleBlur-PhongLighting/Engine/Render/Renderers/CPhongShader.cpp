#include "CPhongShader.h"

CPhongShader::CPhongShader() {
  mLayoutDescLength = 2;
}

CPhongShader::~CPhongShader() {
}

//bool CPhongShader::InitializeShader(ID3D11Device * _d3dDevice, const wchar_t * _vsName, const wchar_t * _psName) {
//  std::vector<uint8_t> vsData;
//  std::vector<uint8_t> psData;
//
//  LoadShaders(_d3dDevice, _vsName, _psName, vsData, psData);
//
//  D3D11_INPUT_ELEMENT_DESC  polygonLayout[2];
//  InitializeInputLayout(polygonLayout);
//  HRESULT result = _d3dDevice->CreateInputLayout(polygonLayout, 2, vsData.data(),
//    vsData.size(), &mLayout);
//  if (FAILED(result)) {
//    return false;
//  }
//
//  CreateMatrixBuffer(_d3dDevice);
//
//  
//
//  return true;
//}

bool CPhongShader::SetToContext(ID3D11DeviceContext * _d3dContext, CAligned<DirectX::XMMATRIX, 16> _worldMatrix, CAligned<DirectX::XMMATRIX, 16> _viewMatrix, CAligned<DirectX::XMMATRIX, 16> _projectionMatrix, 
  MaterialProperties _material, LightProperties _light) {

  if (!SetShaderParameters(_d3dContext, *_worldMatrix, *_viewMatrix, *_projectionMatrix,
    _material, _light)) {
    return false;
  }

  SetShaderToContext(_d3dContext);
  return true;
}

bool CPhongShader::SetShaderParameters(ID3D11DeviceContext * _d3dContext, CAligned<DirectX::XMMATRIX, 16> _world, 
  CAligned<DirectX::XMMATRIX, 16> _view, CAligned<DirectX::XMMATRIX, 16> _projection, 
  MaterialProperties _material, LightProperties _light) {

  if (IShader::SetShaderParameters(_d3dContext, *_world, *_view, *_projection)) {
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    LightProperties* dataPtr;

    unsigned int bufferNumber = 0;
    HRESULT result = _d3dContext->Map(mLightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) {
      return false;
    }

    dataPtr = (LightProperties*)mappedResource.pData;
    dataPtr->mEyePosition = _light.mEyePosition;
    dataPtr->mGlobalAmbient = _light.mGlobalAmbient;
    dataPtr->mLightingData = _light.mLightingData;

    _d3dContext->Unmap(mLightBuffer.Get(), 0);

    D3D11_MAPPED_SUBRESOURCE  mappedResource2;
    MaterialProperties* dataPtr2;
    result = _d3dContext->Map(mMaterialBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource2);
    if (FAILED(result)) {
      return false;
    }

    dataPtr2 = (MaterialProperties*)mappedResource2.pData;
    dataPtr2->mMaterial = _material.mMaterial;

    _d3dContext->Unmap(mMaterialBuffer.Get(), 0);

    ID3D11Buffer* pixelShaderConstantBuffers[2] = {
      mMaterialBuffer.Get(),
      mLightBuffer.Get()
    };
    _d3dContext->PSSetConstantBuffers(bufferNumber, 2, pixelShaderConstantBuffers);
    return true;
  }

  return false;
}

void CPhongShader::InitializeInputLayout(std::vector<D3D11_INPUT_ELEMENT_DESC>& _layouts) {
  IShader::InitializeInputLayout(_layouts);

  _layouts[1].SemanticName = "NORMAL";
  _layouts[1].SemanticIndex = 0;
  _layouts[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
  _layouts[1].InputSlot = 0;
  _layouts[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
  _layouts[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
  _layouts[1].InstanceDataStepRate = 0;
}

bool CPhongShader::CreateAdditionalBuffers(ID3D11Device* _d3dDevice) {
  D3D11_BUFFER_DESC lightBufferDesc;
  lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  lightBufferDesc.ByteWidth = sizeof(LightProperties);
  lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  lightBufferDesc.MiscFlags = 0;
  lightBufferDesc.StructureByteStride = 0;

  HRESULT result = _d3dDevice->CreateBuffer(&lightBufferDesc, NULL, &mLightBuffer);

  lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  lightBufferDesc.ByteWidth = sizeof(MaterialProperties);
  lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  lightBufferDesc.MiscFlags = 0;
  lightBufferDesc.StructureByteStride = 0;

  result = _d3dDevice->CreateBuffer(&lightBufferDesc, NULL, &mMaterialBuffer);

  return true;
}
