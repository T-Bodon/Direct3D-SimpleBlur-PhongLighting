#include "CSimpleGraphics.h"
#include "helpers\CResourceLoader.h"
#include "helpers\CD2DTargetState.h"
#include "helpers\COMRenderTargetState.h"

#include "..\Input\CSimpleInput.h"

CSimpleGraphics::CSimpleGraphics(const int _screenWidth, const int _screenHeight, HWND _hwnd) 
  : mScreenWidth((float)_screenWidth), mScreenHeight((float)_screenHeight), mRotationFactor(0.1f),
  mInput(SimpleControls::ControlDirectional) {
  mDxRes = std::make_unique<CDxResources>(_screenWidth, _screenHeight,
    global::gVSyncEnabled, _hwnd, global::gFullScreen, global::gScreenDepth, global::gScreenNear);

  mCamera = std::make_unique<CSimpleCamera>();
  mCamera->SetPosition(0.0f, 0.0f, 0.f);

  auto packagePath = helpers::CShaderLoader::GetPackagePath();
  auto objPath = packagePath + L"simpleBox.obj";
  mModel = std::make_unique<CModel>(objPath.data());
  mModel->Initialize(mDxRes->GetDevice(), std::lround(mScreenWidth));

  mBgShader = std::make_unique<CTexturingShader>();
  mBgShader->InitializeShader(mDxRes->GetDevice(), L"SimpleVS.cso", L"BackgroundTexturePS.cso");

  mHorizontalBlurShader = std::make_unique<CTexturingShader>();
  mHorizontalBlurShader->InitializeShader(mDxRes->GetDevice(), L"SimpleVS.cso", L"BlurPS.cso");

  auto bgPath = packagePath + L"background1.png";

  mBgTexture = std::make_unique<CTextureResource>();
  mBgTexture->LoadTexture(mDxRes->GetDevice(), bgPath.data());

  mPlainModel = std::make_unique<CPlainModel>((const int)mScreenWidth, (const int)mScreenHeight,
    bgPath.data(), (const int)mScreenWidth, (const int)mScreenHeight);
  mPlainModel->Initialize(mDxRes->GetDevice(), std::lround(mScreenWidth));

  mTemporaryRT = std::make_unique<CTexture>(STextureType::ABGR32, true, true);
  mTemporaryRT->Resize(mDxRes->GetDevice(), mDxRes->GetD2dContext(), DirectX::XMUINT2(_screenWidth, _screenHeight));

  mBluredRT = std::make_unique<CTexture>(STextureType::ABGR32, true, true);
  mBluredRT->Resize(mDxRes->GetDevice(), mDxRes->GetD2dContext(), DirectX::XMUINT2(_screenWidth, _screenHeight));

  mOpacityMaskTexture = std::make_unique<CTexture>(STextureType::R8, true);
  mOpacityMaskTexture->Resize(mDxRes->GetDevice(), mDxRes->GetD2dContext(), DirectX::XMUINT2(_screenWidth, _screenHeight));

  mBlendShader = std::make_unique<CTexturingShader>();
  mBlendShader->InitializeShader(mDxRes->GetDevice(), L"SimpleVS.cso", L"OpacityMaskBlendPS.cso");

  mPhongShader = std::make_unique<CPhongShader>();
  mPhongShader->InitializeShader(mDxRes->GetDevice(), L"PhongVS.cso", L"PhongPS.cso");

  CreateOpacityMask();
}

CSimpleGraphics::~CSimpleGraphics() {
}

bool CSimpleGraphics::ProcessFrame(unsigned int _input) {
  ProcessInput(_input);

  if (!Render())  {
    return false;
  }
  return true;
}

bool CSimpleGraphics::Render() {
  CAligned<DirectX::XMMATRIX, 16> viewMatrix;
  CAligned<DirectX::XMMATRIX, 16> projectionMatrix;
  CAligned<DirectX::XMMATRIX, 16> worldMatrix;
  CAligned<DirectX::XMMATRIX, 16> orthoMatrix;
  bool result;
  float color[4] = { 0.0f, 0.5f, 0.0f, 1.0f };

  // Clear the buffers to begin the scene.
  mDxRes->BeginScene(color);
  auto d3dCtx = mDxRes->GetDeviceContext();
  auto d2dCtx = mDxRes->GetD2dContext();

  mTemporaryRT->Clear(d3dCtx, color);
  {
    COMRenderTargetState d3dState(d3dCtx);

    mTemporaryRT->SetToContextRtv(d3dCtx);

    mCamera->GetViewMatrix(viewMatrix);
    mDxRes->GetWorldMatrix(*worldMatrix);
    mDxRes->GetProjectionMatrix(*projectionMatrix);
    mDxRes->GetOrthoMatrix(*orthoMatrix);

    {
      mDxRes->TurnZBufferOff();

      mPlainModel->SetToContext(mDxRes->GetDeviceContext());
      auto ptr = mBgTexture->GetResource();
      d3dCtx->PSSetShaderResources(0, 1, &ptr);
      result = mBgShader->SetToContext(mDxRes->GetDeviceContext(), DirectX::XMMatrixIdentity(), *viewMatrix,
        *orthoMatrix);
      mPlainModel->Render(d3dCtx);

      mDxRes->TurnZBufferOn();
    }
    
    worldMatrix = DirectX::XMMatrixScaling(0.5, 0.5, 0.5);
    worldMatrix = DirectX::XMMatrixMultiply(*worldMatrix, DirectX::XMMatrixRotationY(mRotationFactor));
    worldMatrix = DirectX::XMMatrixMultiply(*worldMatrix, DirectX::XMMatrixTranslation(0, 0, 2));
    mRotationFactor += 0.0075f;

    mModel->SetToContext(mDxRes->GetDeviceContext());

    MaterialProperties material;
    material.mMaterial.mDiffuse = DirectX::XMFLOAT4(0.49f, 0.325f, 0.698f, 1.0f);
    material.mMaterial.mSpecular = DirectX::XMFLOAT4(0.9f, 0.9f, 0.41f, 1.0f);
    material.mMaterial.mAmbient = DirectX::XMFLOAT4(.156f, 0.150f, 0.392f, 1.0f);

    LightProperties light;
    light.mGlobalAmbient = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
    light.mLightingData.mLightType = mInput;
    light.mLightingData.Enabled = true;
    light.mEyePosition = DirectX::XMFLOAT4(0, 0, 0, 1);
    light.mLightingData.mDirection = DirectX::XMFLOAT4(0.0f, 0.0f, 2.0f, 0.0f);
    light.mLightingData.mPosition = DirectX::XMFLOAT4(0.0f, 0.5f, 1.0f, 1.0f);

    CAligned<DirectX::XMMATRIX, 16> viewProj = DirectX::XMMatrixMultiply(*viewMatrix, *projectionMatrix);
    CAligned<DirectX::XMMATRIX, 16> InvWorld = XMMatrixTranspose(XMMatrixInverse(nullptr, *worldMatrix));

    result = mPhongShader->SetToContext(d3dCtx, *viewProj, *worldMatrix, *InvWorld,
      material, light);

    mModel->Render(d3dCtx);
  }

  {
    mBluredRT->Clear(d3dCtx, color);
    COMRenderTargetState d3dState(d3dCtx);
    mBluredRT->SetToContextRtv(d3dCtx);

    mPlainModel->SetToContext(mDxRes->GetDeviceContext());
    mHorizontalBlurShader->SetToContext(mDxRes->GetDeviceContext(), DirectX::XMMatrixIdentity(), *viewMatrix,
      *orthoMatrix);

    d3dCtx->PSSetShaderResources(0, 1, mTemporaryRT->GetSrv().GetAddressOf());
    mPlainModel->Render(d3dCtx);
  }

  mPlainModel->SetToContext(mDxRes->GetDeviceContext());
  std::vector<ID3D11ShaderResourceView*> tmpVec(3);
  tmpVec[0] = mTemporaryRT->GetSrv().Get();
  tmpVec[1] = mBluredRT->GetSrv().Get();
  tmpVec[2] = mOpacityMaskTexture->GetSrv().Get();

  mBlendShader->SetToContext(d3dCtx, DirectX::XMMatrixIdentity(), *viewMatrix,
    *orthoMatrix);
  d3dCtx->PSSetShaderResources(0, 3, tmpVec.data());
  mPlainModel->Render(d3dCtx);

  mDxRes->EndScene();

  return true;
}

void CSimpleGraphics::CreateOpacityMask() {
  std::unique_ptr<COpacityMaskShader> mOpacityMaskBuilder = std::make_unique<COpacityMaskShader>(mScreenWidth);
  mOpacityMaskBuilder->InitializeShader(mDxRes->GetDevice(), L"OpacityMaskConstructorVS.cso", L"OpacityMaskConstructorPS.cso");

  CAligned<DirectX::XMMATRIX, 16> viewMatrix;
  CAligned<DirectX::XMMATRIX, 16> worldMatrix;
  CAligned<DirectX::XMMATRIX, 16> orthoMatrix;

  auto d3dCtx = mDxRes->GetDeviceContext();

  float color[4] = { 0.0f, 0.5f, 0.0f, 1.0f };
  mOpacityMaskTexture->Clear(d3dCtx, color);

  {
    COMRenderTargetState d3dState(d3dCtx);
    mOpacityMaskTexture->SetToContextRtv(d3dCtx);

    mCamera->GetViewMatrix(viewMatrix);
    mDxRes->GetWorldMatrix(*worldMatrix);
    mDxRes->GetOrthoMatrix(*orthoMatrix);

    mDxRes->TurnZBufferOff();
    mPlainModel->SetToContext(mDxRes->GetDeviceContext());
    auto result = mOpacityMaskBuilder->SetToContext(mDxRes->GetDeviceContext(), DirectX::XMMatrixIdentity(), *viewMatrix,
      *orthoMatrix);
    mPlainModel->Render(d3dCtx);
    mDxRes->TurnZBufferOn();
  }
}

void CSimpleGraphics::ProcessInput(unsigned int _input) {
  for (unsigned int i = SimpleControls::ControlDirectional; i < SimpleControls::Controls_MAX; i++) {
    if (_input == i) {
      mInput = _input;
    }
  }
}
