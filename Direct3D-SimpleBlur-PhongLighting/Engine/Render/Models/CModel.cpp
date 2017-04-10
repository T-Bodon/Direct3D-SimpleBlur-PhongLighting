#include "CModel.h"
#include "..\helpers\CResourceLoader.h"

CModel::CModel(const wchar_t* _objFilePath)// , ID3D11Device* _d3dDevice, const int _screenWidth)
  : mVertexCount(0), mObjFilePath(_objFilePath) {
}

CModel::~CModel(){
}

void CModel::Initialize(ID3D11Device* _d3dDevice, const int _screenWidth) {
  mVertexBufferStride = sizeof(VertexType);

  InitializeBuffers(_d3dDevice, _screenWidth);
}

void CModel::SetToContext(ID3D11DeviceContext* _d3dContext) {
  SetBufferToContext(_d3dContext);
}

void CModel::Render(ID3D11DeviceContext * _d3dContext) {
  _d3dContext->Draw(mVertexCount, 0);
}

bool CModel::InitializeBuffers(ID3D11Device* _d3dDevice, const int _screenWidth) {
  D3D11_BUFFER_DESC vertexBufferDesc;
  D3D11_SUBRESOURCE_DATA vertexData;
  HRESULT result;

  helpers::CSimpleObjLoader loader;
  loader.LoadObj(mObjFilePath);

  auto vertexBuffer = loader.GetVBuff();
  auto normalBuffer = loader.GetVnBuff();

  mVertexCount = (unsigned int)vertexBuffer->size();

  std::vector<VertexType> vertices(mVertexCount);

  for (size_t i = 0; i < mVertexCount; i++) {
    vertices[i].mPosition = vertexBuffer->at(i);
    vertices[i].mNormal   = normalBuffer->at(i);
  }

  vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  vertexBufferDesc.ByteWidth  = sizeof(VertexType) * mVertexCount;
  vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertexBufferDesc.CPUAccessFlags = 0;
  vertexBufferDesc.MiscFlags = 0;
  vertexBufferDesc.StructureByteStride = 0;

  vertexData.pSysMem = vertices.data();
  vertexData.SysMemPitch = 0;
  vertexData.SysMemSlicePitch = 0;

  result = _d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertexBuffer);
  if (FAILED(result))
    return false;

  return true;
}

void CModel::SetBufferToContext(ID3D11DeviceContext * _d3dContext) {
  unsigned int offset = 0;

  _d3dContext->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &mVertexBufferStride, &offset);

  _d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

