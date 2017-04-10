#include "CDxResources.h"

CDxResources::CDxResources(const int _screenW, const int _screenH,
  const bool  _vsync, HWND _hwnd, const bool _fullscreen,
  const float _screenDepth, const float _screenNear) {
  mVsyncEnabled = _vsync;

  CreateDeviceIndependentResources();
  CreateDeviceDependentResources(_screenW, _screenH, _hwnd, _fullscreen);
  CreateTargetAndStates(_screenW, _screenH);
  InitializeMatrices(_screenW, _screenH, _screenDepth, _screenNear);
  CreateDirect2DResources();
}

CDxResources::~CDxResources() {
}

void CDxResources::BeginScene(float* _color) {
  mDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), _color);
  mDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void CDxResources::EndScene() {
  // Present the back buffer to the screen since rendering is complete.
  if (mVsyncEnabled) {
    // Lock to screen refresh rate.
    mSwapChain->Present(1, 0);
  }
  else {
    // Present as fast as possible.
    mSwapChain->Present(0, 0);
  }

}

ID3D11Device* CDxResources::GetDevice() {
  return mDevice.Get();
}

ID3D11DeviceContext* CDxResources::GetDeviceContext() {
  return mDeviceContext.Get();
}

ID2D1Device* CDxResources::GetD2dDevice() {
  return mD2dDevice.Get();
}

ID2D1DeviceContext* CDxResources::GetD2dContext() {
  return mD2dContext.Get();
}

ID2D1Factory1* CDxResources::GetD2dFactory() {
  return mD2dFactory.Get();
}

void CDxResources::GetProjectionMatrix(DirectX::XMMATRIX& _result) {
  _result = mProjectionWorldOrtho->mProjectionMatrix;
}

void CDxResources::GetWorldMatrix(DirectX::XMMATRIX& _result) {
  _result = mProjectionWorldOrtho->mWorldMatrix;
}

void CDxResources::GetOrthoMatrix(DirectX::XMMATRIX& _result){
  _result = mProjectionWorldOrtho->mOrthoMatrix;
}

void CDxResources::TurnZBufferOn() {
  mDeviceContext->OMSetDepthStencilState(mDepthStencilState.Get(), 1);
}

void CDxResources::TurnZBufferOff() {
  mDeviceContext->OMSetDepthStencilState(mDepthDisabledStencilState.Get(), 1);
}

void CDxResources::CreateDeviceIndependentResources() {
  auto result = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, 
    IID_PPV_ARGS(mD2dFactory.GetAddressOf()));
}

void CDxResources::CreateDeviceDependentResources(const unsigned int _screenW,
  const unsigned int _screenH, HWND _hwnd, const bool _fullscreen) {
  unsigned int numerator = 0;
  unsigned int denominator = 0;
  DXGI_SWAP_CHAIN_DESC swapChainDesc;

  ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

  // Set to a single back buffer.
  swapChainDesc.BufferCount = 1;
  swapChainDesc.BufferDesc.Width  = _screenW;
  swapChainDesc.BufferDesc.Height = _screenH;
  swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

  if (mVsyncEnabled)
  {
    swapChainDesc.BufferDesc.RefreshRate.Numerator   = numerator;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
  }
  else
  {
    swapChainDesc.BufferDesc.RefreshRate.Numerator   = 0;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
  }

  swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

  swapChainDesc.OutputWindow = _hwnd;

  // Turn multisampling off.
  swapChainDesc.SampleDesc.Count   = 1;
  swapChainDesc.SampleDesc.Quality = 0;

  if (_fullscreen)
    swapChainDesc.Windowed = false;
  else
    swapChainDesc.Windowed = true;

  // Set the scan line ordering and scaling to unspecified.
  swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
  swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

  // Discard the back buffer contents after presenting.
  swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  swapChainDesc.Flags = 0;

  D3D_FEATURE_LEVEL featureLevels[] = {
    D3D_FEATURE_LEVEL_11_1,
    D3D_FEATURE_LEVEL_11_0,
    D3D_FEATURE_LEVEL_10_1,
    D3D_FEATURE_LEVEL_10_0,
    D3D_FEATURE_LEVEL_9_3,
    D3D_FEATURE_LEVEL_9_2,
    D3D_FEATURE_LEVEL_9_1
  };

  HRESULT result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_BGRA_SUPPORT, featureLevels, ARRAYSIZE(featureLevels),
    D3D11_SDK_VERSION, &swapChainDesc, mSwapChain.GetAddressOf(), mDevice.GetAddressOf(), NULL, mDeviceContext.GetAddressOf());
}

void CDxResources::CreateTargetAndStates(const unsigned int _screenW,
  const unsigned int _screenH) {
  ID3D11Texture2D * backBufferPtr;
  D3D11_TEXTURE2D_DESC depthBufferDesc;
  D3D11_DEPTH_STENCIL_DESC  depthStencilDesc;
  D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
  D3D11_RASTERIZER_DESC rasterDesc;
  D3D11_VIEWPORT viewport;

  auto result = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);

  result = mDevice->CreateRenderTargetView(backBufferPtr, NULL, mRenderTargetView.GetAddressOf());

  backBufferPtr->Release();
  backBufferPtr = 0;

  ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

  depthBufferDesc.Width  = _screenW;
  depthBufferDesc.Height = _screenH;
  depthBufferDesc.MipLevels = 1;
  depthBufferDesc.ArraySize  = 1;
  depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthBufferDesc.SampleDesc.Count = 1;
  depthBufferDesc.SampleDesc.Quality = 0;
  depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
  depthBufferDesc.BindFlags  = D3D11_BIND_DEPTH_STENCIL;
  depthBufferDesc.CPUAccessFlags = 0;
  depthBufferDesc.MiscFlags = 0;

  result = mDevice->CreateTexture2D(&depthBufferDesc, NULL, mDepthStencilBuffer.GetAddressOf());
  
  ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

  depthStencilDesc.DepthEnable = true;
  depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

  depthStencilDesc.StencilEnable = true;
  depthStencilDesc.StencilReadMask = 0xFF;
  depthStencilDesc.StencilWriteMask = 0xFF;

  depthStencilDesc.FrontFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
  depthStencilDesc.FrontFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.FrontFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;

  depthStencilDesc.BackFace.StencilFailOp      = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
  depthStencilDesc.BackFace.StencilPassOp      = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.BackFace.StencilFunc        = D3D11_COMPARISON_ALWAYS;

  result = mDevice->CreateDepthStencilState(&depthStencilDesc, mDepthStencilState.GetAddressOf());
  
  mDeviceContext->OMSetDepthStencilState(mDepthStencilState.Get(), 1);

  ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

  depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
  depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
  depthStencilViewDesc.Texture2D.MipSlice = 0;

  result = mDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &depthStencilViewDesc, mDepthStencilView.GetAddressOf());
  
  mDeviceContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());

  rasterDesc.AntialiasedLineEnable = false;
  rasterDesc.CullMode              = D3D11_CULL_BACK;
  rasterDesc.DepthBias             = 0;
  rasterDesc.DepthBiasClamp        = 0.0f;
  rasterDesc.DepthClipEnable       = true;
  rasterDesc.FillMode              = D3D11_FILL_SOLID;
  rasterDesc.FrontCounterClockwise = false;
  rasterDesc.MultisampleEnable     = false;
  rasterDesc.ScissorEnable         = false;
  rasterDesc.SlopeScaledDepthBias  = 0.0f;

  result = mDevice->CreateRasterizerState(&rasterDesc, mRasterState.GetAddressOf());
  
  mDeviceContext->RSSetState(mRasterState.Get());

  viewport.Width    = (float)_screenW;
  viewport.Height   = (float)_screenH;
  viewport.MinDepth = 0.0f;
  viewport.MaxDepth = 1.0f;
  viewport.TopLeftX = 0.0f;
  viewport.TopLeftY = 0.0f;

  mDeviceContext->RSSetViewports(1, &viewport);
  
  // Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
	// that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
  depthStencilDesc.DepthEnable = false;
  depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
  depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
  depthStencilDesc.StencilEnable = true;
  depthStencilDesc.StencilReadMask = 0xFF;
  depthStencilDesc.StencilWriteMask = 0xFF;
  depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
  depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
  depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
  depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
  depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
  result = mDevice->CreateDepthStencilState(&depthStencilDesc, &mDepthDisabledStencilState);
}

void CDxResources::InitializeMatrices(const int _screenW, const int _screenH,
  const float _screenDepth, const float _screenNear) {
  float fieldOfView = (float)DirectX::XM_PIDIV4;
  float screenAspect = (float)_screenW / (float)_screenH;

  mProjectionWorldOrtho->mProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(DirectX::XMConvertToRadians(90.0f), screenAspect, 0.01f, 100.0f);
  mProjectionWorldOrtho->mWorldMatrix      = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationY(DirectX::XM_PIDIV4 * 2.5f), DirectX::XMMatrixTranslation(0.0f, 0.5f, 1.0f));

  mProjectionWorldOrtho->mOrthoMatrix = DirectX::XMMatrixOrthographicLH((float)_screenW, (float)_screenH, 0.01f, 100.0f);
}

void CDxResources::CreateDirect2DResources() {
  D2D1_CREATION_PROPERTIES            creationProps;
  Microsoft::WRL::ComPtr<IDXGIDevice> dxgiDev;

  auto result = mDevice.As(&dxgiDev);
  

#ifdef _DEBUG
  creationProps.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#else
  creationProps.debugLevel = D2D1_DEBUG_LEVEL_NONE;
#endif

  creationProps.options = D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS;
  creationProps.threadingMode = D2D1_THREADING_MODE_MULTI_THREADED;

  result = D2D1CreateDevice(dxgiDev.Get(), creationProps, mD2dDevice.GetAddressOf());  

  result = mD2dDevice->CreateDeviceContext(
    D2D1_DEVICE_CONTEXT_OPTIONS_ENABLE_MULTITHREADED_OPTIMIZATIONS,
    mD2dContext.GetAddressOf());
  
  //Set up Direct2D render target bitmap, linking it to the swapchain
  Microsoft::WRL::ComPtr<IDXGISurface> dxgiBackBuffer;
  D2D1_PIXEL_FORMAT pixelFormat = { DXGI_FORMAT_R8G8B8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED };
  D2D1_BITMAP_PROPERTIES1 bitmapProperties = { pixelFormat, 96.0f, 96.0f,
    D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, 0 };
  //Direct2D needs the dxgi version of the backbuffer surface pointer
  mSwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));

  Microsoft::WRL::ComPtr<ID2D1Bitmap1> targetBitmap;

  result = mD2dContext->CreateBitmapFromDxgiSurface(
    dxgiBackBuffer.Get(),
    &bitmapProperties,
    targetBitmap.GetAddressOf()
  );
  mD2dContext->SetTarget(targetBitmap.Get());
}




CDxResources::D3DMatrices::D3DMatrices()
  : mOrthoMatrix(DirectX::XMMatrixIdentity()),
  mProjectionMatrix(DirectX::XMMatrixIdentity()),
  mWorldMatrix(DirectX::XMMatrixIdentity())
{
}
