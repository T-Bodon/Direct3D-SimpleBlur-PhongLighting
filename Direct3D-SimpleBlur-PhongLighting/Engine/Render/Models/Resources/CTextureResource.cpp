#include "CTextureResource.h"
#include "..\..\Helpers\ImageUtils.h"

CTextureResource::CTextureResource()
{
}

CTextureResource::~CTextureResource()
{
}

bool CTextureResource::LoadTexture(ID3D11Device * _d3dDevice, const wchar_t * _texturePath) {
  Microsoft::WRL::ComPtr<IStream> stream;

  HRESULT hr = SHCreateStreamOnFileEx(_texturePath, STGM_READ, FILE_ATTRIBUTE_NORMAL, FALSE,
    nullptr, stream.GetAddressOf());

  ImagePixels rawData = ImageUtils::LoadDxPBgra32Image(stream, 512);

  D3D11_SUBRESOURCE_DATA rawDataDescription;
  rawDataDescription.pSysMem = rawData.Data.data();
  rawDataDescription.SysMemPitch = rawData.Data.size() / 512;
  rawDataDescription.SysMemSlicePitch = rawData.Data.size();

  Microsoft::WRL::ComPtr<ID3D11Texture2D> tmpTexture2d;
  D3D11_TEXTURE2D_DESC depthBufferDesc;
  depthBufferDesc.Width = 512;
  depthBufferDesc.Height = 512;
  depthBufferDesc.MipLevels = 1;
  depthBufferDesc.ArraySize = 1;
  //GUID_WICPixelFormat32bppPBGRA
  depthBufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
  depthBufferDesc.SampleDesc.Count = 1;
  depthBufferDesc.SampleDesc.Quality = 0;
  depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  depthBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;// | D3D11_BIND_RENDER_TARGET;
  depthBufferDesc.CPUAccessFlags = 0;
  depthBufferDesc.MiscFlags = 0;

  hr = _d3dDevice->CreateTexture2D(&depthBufferDesc, &rawDataDescription, &tmpTexture2d);

  D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
  srvDesc.Format = depthBufferDesc.Format;
  srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
  srvDesc.Texture2D.MipLevels = depthBufferDesc.MipLevels;
  srvDesc.Texture2D.MostDetailedMip = 0;
  hr = _d3dDevice->CreateShaderResourceView(tmpTexture2d.Get(), &srvDesc, &mTexture);

  if (FAILED(hr)) {
    return false;
  }

  return true;
}

ID3D11ShaderResourceView * CTextureResource::GetResource() {
  return mTexture.Get();
}
