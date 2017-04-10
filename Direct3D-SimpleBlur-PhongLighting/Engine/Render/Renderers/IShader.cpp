#include "IShader.h"
#include "..\helpers\CResourceLoader.h"

IShader::IShader()
  : mLayoutDescLength(1) {
}

IShader::~IShader() {
}

bool IShader::InitializeShader(ID3D11Device* _d3dDevice, const wchar_t* _vsName, const wchar_t* _psName) {
  std::vector<uint8_t> vsData;
  std::vector<uint8_t> psData;

  LoadShaders(_d3dDevice, _vsName, _psName, vsData, psData);

  std::vector<D3D11_INPUT_ELEMENT_DESC> polygonLayout(mLayoutDescLength);
  InitializeInputLayout(polygonLayout);
  HRESULT result = _d3dDevice->CreateInputLayout(polygonLayout.data(), mLayoutDescLength, 
    vsData.data(), vsData.size(), &mLayout);
  if (FAILED(result)) {
    return false;
  }

  CreateMatrixBuffer(_d3dDevice);
  CreateAdditionalBuffers(_d3dDevice);
  return true;
}

bool IShader::SetToContext(ID3D11DeviceContext * _d3dContext, CAligned<DirectX::XMMATRIX, 16> _worldMatrix, CAligned<DirectX::XMMATRIX, 16> _viewMatrix, CAligned<DirectX::XMMATRIX, 16> _projectionMatrix) {
  if (!SetShaderParameters(_d3dContext, *_worldMatrix, *_viewMatrix, *_projectionMatrix)) {
    return false;
  }

  SetShaderToContext(_d3dContext);
  return true;
}

bool IShader::SetShaderParameters(ID3D11DeviceContext * _d3dContext, 
  CAligned<DirectX::XMMATRIX, 16> _world, CAligned<DirectX::XMMATRIX, 16> _view, 
  CAligned<DirectX::XMMATRIX, 16> _ortho) {
  D3D11_MAPPED_SUBRESOURCE  mappedResource;
  MatrixBufferType* dataPtr;
  unsigned int bufferNumber = 0;

  _world = DirectX::XMMatrixTranspose(*_world);
  _view = DirectX::XMMatrixTranspose(*_view);
  _ortho = DirectX::XMMatrixTranspose(*_ortho);

  HRESULT result = _d3dContext->Map(mMatrixBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
  if (FAILED(result)) {
    return false;
  }

  dataPtr = (MatrixBufferType*)mappedResource.pData;
  dataPtr->mWorld = *_world;
  dataPtr->mView = *_view;
  dataPtr->mProjection = *_ortho;

  _d3dContext->Unmap(mMatrixBuffer.Get(), 0);

  _d3dContext->VSSetConstantBuffers(bufferNumber, 1, mMatrixBuffer.GetAddressOf());

  return true;
}

void IShader::SetShaderToContext(ID3D11DeviceContext * _d3dContext) {
  _d3dContext->IASetInputLayout(mLayout.Get());
  _d3dContext->VSSetShader(mVertexShader.Get(), NULL, 0);
  _d3dContext->PSSetShader(mPixelShader.Get(), NULL, 0);
}

bool IShader::LoadShaders(ID3D11Device* _d3dDevice, const wchar_t * _vsName, const wchar_t * _psName,
  std::vector<uint8_t>& _vsData, std::vector<uint8_t>& _psData) {
  auto packagePath = helpers::CShaderLoader::GetPackagePath();
  auto psFile = packagePath + _psName;
  auto vsFile = packagePath + _vsName;

  _vsData = helpers::CShaderLoader::LoadPackageFile(vsFile);
  HRESULT result = _d3dDevice->CreateVertexShader(_vsData.data(), _vsData.size(), nullptr, &mVertexShader);

  if (FAILED(result)) {
    return false;
  }

  _psData = helpers::CShaderLoader::LoadPackageFile(psFile);
  result = _d3dDevice->CreatePixelShader(_psData.data(), _psData.size(), nullptr, mPixelShader.GetAddressOf());

  if (FAILED(result)) {
    return false;
  }

  return true;
}

void IShader::InitializeInputLayout(std::vector<D3D11_INPUT_ELEMENT_DESC>& _layouts) {
  _layouts[0].SemanticName = "POSITION";
  _layouts[0].SemanticIndex = 0;
  _layouts[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
  _layouts[0].InputSlot = 0;
  _layouts[0].AlignedByteOffset = 0;
  _layouts[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
  _layouts[0].InstanceDataStepRate = 0; 
}

bool IShader::CreateMatrixBuffer(ID3D11Device* _d3dDevice) {
  D3D11_BUFFER_DESC matrixBufferDesc;

  matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
  matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
  matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
  matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
  matrixBufferDesc.MiscFlags = 0;
  matrixBufferDesc.StructureByteStride = 0;

  HRESULT result = _d3dDevice->CreateBuffer(&matrixBufferDesc, NULL, &mMatrixBuffer);
  if (FAILED(result)) {
    return false;
  }

  return true;
}

bool IShader::CreateAdditionalBuffers(ID3D11Device * _d3dDevice) {
  return true;
}
