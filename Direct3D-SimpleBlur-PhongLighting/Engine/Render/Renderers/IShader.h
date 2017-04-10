#pragma once
#include "BufferStructures.h"
#include "..\helpers\CAligned.h"

#include <vector>

class IShader {
public:
  IShader();
  virtual ~IShader();

  virtual bool InitializeShader(ID3D11Device* _d3dDevice, const wchar_t* _vsName, const wchar_t* _psName);

  virtual bool SetToContext(ID3D11DeviceContext* _d3dContext, CAligned<DirectX::XMMATRIX, 16> _worldMatrix,
    CAligned<DirectX::XMMATRIX, 16> _viewMatrix, CAligned<DirectX::XMMATRIX, 16> _projectionMatrix);

protected:
  Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
  Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
  Microsoft::WRL::ComPtr<ID3D11InputLayout> mLayout;
  Microsoft::WRL::ComPtr<ID3D11Buffer> mMatrixBuffer;

  unsigned int mLayoutDescLength;

  //pass reference to matrix
  virtual bool SetShaderParameters(ID3D11DeviceContext* _d3dContext, CAligned<DirectX::XMMATRIX, 16> _world,
    CAligned<DirectX::XMMATRIX, 16> _view, CAligned<DirectX::XMMATRIX, 16> _ortho);

  virtual void SetShaderToContext(ID3D11DeviceContext* _d3dContext);

  virtual bool LoadShaders(ID3D11Device* _d3dDevice, const wchar_t* _vsName, const wchar_t* _psName,
    std::vector<uint8_t>& _vsData, std::vector<uint8_t>& _psData);
  virtual void InitializeInputLayout(std::vector<D3D11_INPUT_ELEMENT_DESC>& _layouts);
  virtual bool CreateMatrixBuffer(ID3D11Device* _d3dDevice);
  virtual bool CreateAdditionalBuffers(ID3D11Device* _d3dDevice);
};
