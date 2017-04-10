#include "OpacityMaskBuilderShader.h"
#include "..\helpers\CResourceLoader.h"

COpacityMaskShader::COpacityMaskShader(float _width)
  : mTextureWidth(_width) {
}

COpacityMaskShader::~COpacityMaskShader() {
}

bool COpacityMaskShader::SetShaderParameters(ID3D11DeviceContext * _d3dContext, CAligned<DirectX::XMMATRIX, 16> _world, 
  CAligned<DirectX::XMMATRIX, 16> _view, CAligned<DirectX::XMMATRIX, 16> _ortho) {

  if (IShader::SetShaderParameters(_d3dContext, *_world, *_view, *_ortho)) {
    D3D11_MAPPED_SUBRESOURCE  mappedResource;
    TextureWidthBuffer* dataPtr;

    unsigned int bufferNumber = 0;
    HRESULT result = _d3dContext->Map(mTextWidthBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result)) {
      return false;
    }

    dataPtr = (TextureWidthBuffer*)mappedResource.pData;
    dataPtr->mTextureWidth = mTextureWidth;

    _d3dContext->Unmap(mTextWidthBuffer.Get(), 0);

    _d3dContext->PSSetConstantBuffers(bufferNumber, 1, mTextWidthBuffer.GetAddressOf());
    return true;
  }

  return false;
}

bool COpacityMaskShader::CreateAdditionalBuffers(ID3D11Device * _d3dDevice) {
  D3D11_BUFFER_DESC bufferDesc;
  bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  bufferDesc.ByteWidth = sizeof(TextureWidthBuffer);
  bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  bufferDesc.MiscFlags = 0;
  bufferDesc.StructureByteStride = 0;

  HRESULT result = _d3dDevice->CreateBuffer(&bufferDesc, NULL, &mTextWidthBuffer);
  return true;
}