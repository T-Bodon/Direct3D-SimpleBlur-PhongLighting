#pragma once
#include "CModel.h"

class CPlainModel : public CModel {
public:
  CPlainModel(const int _screenW, const int _screenH,
    const wchar_t* _texturePath, const int _bmpW, const int _bmpH);
  ~CPlainModel();

  void Initialize(ID3D11Device* _d3dDevice, const int _screenWidth) override;

private:
  struct VertexType {
    DirectX::XMFLOAT3 mPosition;
    DirectX::XMFLOAT2 mTexture;
  };
/*
  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTexture;*/

  int mScreenWidth;
  int mScreenHeight;
  int mBitmapWidth;
  int mBitmapHeight;

  bool InitializeBuffers(ID3D11Device* _d3dDevice, const int _screenWidth) override;
  //void SetBufferToContext(ID3D11DeviceContext* _d3dContext) override;
/*
  bool LoadTexture(ID3D11Device* _d3dDevice, const wchar_t* texturePath);*/
};
