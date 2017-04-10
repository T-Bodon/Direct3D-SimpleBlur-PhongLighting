#pragma once
#include"..\..\Helpers\Dx.h"

class CTextureResource {
public:
  CTextureResource ();
  ~CTextureResource ();

  bool LoadTexture(ID3D11Device* _d3dDevice, const wchar_t* texturePath);

  ID3D11ShaderResourceView* GetResource(); 

private:
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTexture;
};