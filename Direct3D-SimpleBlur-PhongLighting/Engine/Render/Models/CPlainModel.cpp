#include "CPlainModel.h"
#include "..\Helpers\ImageUtils.h"

#include <vector>
#include <numeric>

namespace {
  const int gTextureVerticesForShader = 6;
}

CPlainModel::CPlainModel(const int _screenW, const int _screenH,
  const wchar_t * _texturePath, const int _bmpW, const int _bmpH)
  : CModel(_texturePath), mScreenWidth(_screenW),
  mScreenHeight(_screenH), mBitmapWidth(_bmpW), mBitmapHeight(_bmpH) {
  }

CPlainModel::~CPlainModel() {
}

void CPlainModel::Initialize(ID3D11Device * _d3dDevice, const int _screenWidth) {
  mVertexBufferStride = sizeof(VertexType);

  InitializeBuffers(_d3dDevice, mScreenWidth);
  //LoadTexture(_d3dDevice, mObjFilePath);
}
//try not to use pixel coordinates  + not use ortho matrix
bool CPlainModel::InitializeBuffers(ID3D11Device * _d3dDevice, const int _screenWidth) {
  D3D11_BUFFER_DESC vertexBufferDesc;
  D3D11_SUBRESOURCE_DATA vertexData;

  mVertexCount = gTextureVerticesForShader;

  std::vector<VertexType> vertices = std::vector<VertexType>(mVertexCount);
  
  // Calculate the screen coordinates of the left side of the bitmap.
  auto left = (float)((mScreenWidth / 2) * -1);

  // Calculate the screen coordinates of the right side of the bitmap.
  auto right = left + (float)mBitmapWidth;

  // Calculate the screen coordinates of the top of the bitmap.
  auto top = (float)(mScreenHeight / 2);

  // Calculate the screen coordinates of the bottom of the bitmap.
  auto bottom = top - (float)mBitmapHeight;

  vertices[0].mPosition = DirectX::XMFLOAT3(left, top, 1.0f);  // Top left.
  vertices[0].mTexture = DirectX::XMFLOAT2(0.0f, 0.0f);

  vertices[1].mPosition = DirectX::XMFLOAT3(right, bottom, 1.0f);  // Bottom right.
  vertices[1].mTexture = DirectX::XMFLOAT2(1.0f, 1.0f);

  vertices[2].mPosition = DirectX::XMFLOAT3(left, bottom, 1.0f);  // Bottom left.
  vertices[2].mTexture = DirectX::XMFLOAT2(0.0f, 1.0f);

  vertices[3].mPosition = DirectX::XMFLOAT3(left, top, 1.0f);  // Top left.
  vertices[3].mTexture = DirectX::XMFLOAT2(0.0f, 0.0f);

  vertices[4].mPosition = DirectX::XMFLOAT3(right, top, 1.0f);  // Top right.
  vertices[4].mTexture = DirectX::XMFLOAT2(1.0f, 0.0f);

  vertices[5].mPosition = DirectX::XMFLOAT3(right, bottom, 1.0f);  // Bottom right.
  vertices[5].mTexture = DirectX::XMFLOAT2(1.0f, 1.0f);

  vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  vertexBufferDesc.ByteWidth = sizeof(VertexType) * mVertexCount;
  vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  vertexBufferDesc.CPUAccessFlags = 0;
  vertexBufferDesc.MiscFlags = 0;
  vertexBufferDesc.StructureByteStride = 0;

  vertexData.pSysMem = vertices.data();
  vertexData.SysMemPitch = 0;
  vertexData.SysMemSlicePitch = 0;

  HRESULT result = _d3dDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertexBuffer);

  if (FAILED(result))
    return false;

  return true;
}

//void CPlainModel::SetBufferToContext(ID3D11DeviceContext * _d3dContext, ID3D11ShaderResourceView* _srv) {
//  CModel::SetBufferToContext(_d3dContext);
//
//  /*if (!_srv) {
//    _d3dContext->PSSetShaderResources(0, 1, mTexture.GetAddressOf());
//  }
//  else {
//    _d3dContext->PSSetShaderResources(0, 1, &_srv);
//  }*/
//}

//bool CPlainModel::LoadTexture(ID3D11Device * _d3dDevice, const wchar_t * _texturePath) {
//  Microsoft::WRL::ComPtr<IStream> stream;
//
//  HRESULT hr = SHCreateStreamOnFileEx(_texturePath, STGM_READ, FILE_ATTRIBUTE_NORMAL, FALSE,
//    nullptr, stream.GetAddressOf());
//
//  ImagePixels rawData = ImageUtils::LoadDxPBgra32Image(stream, mBitmapWidth);
//
//  D3D11_SUBRESOURCE_DATA rawDataDescription;
//  rawDataDescription.pSysMem = rawData.Data.data();
//  rawDataDescription.SysMemPitch = rawData.Data.size() / mScreenHeight;
//  rawDataDescription.SysMemSlicePitch = rawData.Data.size();
//
//  Microsoft::WRL::ComPtr<ID3D11Texture2D> tmpTexture2d;
//  D3D11_TEXTURE2D_DESC depthBufferDesc;
//  depthBufferDesc.Width = mScreenWidth;
//  depthBufferDesc.Height = mScreenHeight;
//  depthBufferDesc.MipLevels = 1;
//  depthBufferDesc.ArraySize = 1;
//  //GUID_WICPixelFormat32bppPBGRA
//  depthBufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
//  depthBufferDesc.SampleDesc.Count = 1;
//  depthBufferDesc.SampleDesc.Quality = 0;
//  depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//  depthBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;// | D3D11_BIND_RENDER_TARGET;
//  depthBufferDesc.CPUAccessFlags = 0;
//  depthBufferDesc.MiscFlags = 0;
//
//  hr = _d3dDevice->CreateTexture2D(&depthBufferDesc, &rawDataDescription, &tmpTexture2d);
//
//  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
//  srvDesc.Format = depthBufferDesc.Format;
//  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
//  srvDesc.Texture2D.MipLevels = depthBufferDesc.MipLevels;
//  srvDesc.Texture2D.MostDetailedMip = 0;
//  hr = _d3dDevice->CreateShaderResourceView(tmpTexture2d.Get(), &srvDesc, &mTexture);
//
//  if (FAILED(hr)) {
//    return false;
//  }
//
//  return true;
//}
