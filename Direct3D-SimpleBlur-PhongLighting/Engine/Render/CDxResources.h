#pragma once
#include "helpers\Dx.h"
#include "helpers\CAligned.h"


class CDxResources
{
public:
  CDxResources(const int _screenW, const int _screenH, const bool _vsync, HWND _hwnd,
    const bool _fullscreen, const float _screenDepth, const float _screenNear);
  ~CDxResources();

  void BeginScene(float* _color);
  void EndScene();

  ID3D11Device * GetDevice();
  ID3D11DeviceContext * GetDeviceContext();

  ID2D1Device * GetD2dDevice();
  ID2D1DeviceContext * GetD2dContext();
  ID2D1Factory1 * GetD2dFactory();
  
  void GetProjectionMatrix(DirectX::XMMATRIX& _result);
  void GetWorldMatrix(DirectX::XMMATRIX& _result);
  void GetOrthoMatrix(DirectX::XMMATRIX& _result);

  void TurnZBufferOn();
  void TurnZBufferOff();

private:
  struct D3DMatrices
  {
    DirectX::XMMATRIX  mProjectionMatrix;
    DirectX::XMMATRIX  mWorldMatrix;
    DirectX::XMMATRIX  mOrthoMatrix;

    D3DMatrices();
  };

  bool mVsyncEnabled;
  Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
  Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
  Microsoft::WRL::ComPtr<ID3D11DeviceContext> mDeviceContext;
  Microsoft::WRL::ComPtr<ID2D1Factory1> mD2dFactory;
  Microsoft::WRL::ComPtr<ID2D1Device> mD2dDevice;
  Microsoft::WRL::ComPtr<ID2D1DeviceContext> mD2dContext;
  Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRenderTargetView;
  Microsoft::WRL::ComPtr<ID3D11Texture2D> mDepthStencilBuffer;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthStencilState;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthDisabledStencilState;
  Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDepthStencilView;
  Microsoft::WRL::ComPtr<ID3D11RasterizerState> mRasterState;
  CAligned<D3DMatrices, 16> mProjectionWorldOrtho;

  void CreateDeviceIndependentResources();
  void CreateDeviceDependentResources(const unsigned int _screenW,
    const unsigned int _screenH, HWND _hwnd, const bool _fullscreen);
  void CreateTargetAndStates(const unsigned int _screenW,
    const unsigned int _screenH);
  void InitializeMatrices(const int _screenW, const int _screenH,
    const float _screenDepth, const float _screenNear);
  void CreateDirect2DResources();
};