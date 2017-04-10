#pragma once
#include "..\helpers\Dx.h"

class CModel {
public:
  CModel(const wchar_t* _objFilePath);
  ~CModel();

  virtual void Initialize(ID3D11Device* _d3dDevice, const int _screenWidth);

  void SetToContext(ID3D11DeviceContext* _d3dContext);
  void Render(ID3D11DeviceContext* _d3dContext);

protected:
  const wchar_t* mObjFilePath;
  unsigned int mVertexCount;
  unsigned int mVertexBufferStride;
  Microsoft::WRL::ComPtr<ID3D11Buffer> mVertexBuffer;


  virtual bool InitializeBuffers(ID3D11Device* _d3dDevice, const int _screenWidth);
  virtual void SetBufferToContext(ID3D11DeviceContext* _d3dContext);

private:
  struct VertexType {
    DirectX::XMFLOAT3 mPosition;
    DirectX::XMFLOAT3 mNormal;
  };
};