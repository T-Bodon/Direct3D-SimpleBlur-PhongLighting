#pragma once
#include "CDxResources.h"
#include "..\Camera\CSimpleCamera.h"
#include "Models\CModel.h"
#include "Models\CPlainModel.h"
#include "Renderers\CTexturingShader.h"
#include "Renderers\OpacityMaskBuilderShader.h"
#include "Renderers\CPhongShader.h"
#include "Models\Resources\CTextureResource.h"
#include "helpers\Texture.h"

#include <memory>

namespace global {
  const bool  gFullScreen = false;
  const bool  gVSyncEnabled = true;
  const float gScreenDepth = 1000.0f;
  const float gScreenNear = 0.1f;

  const float gDpiX = 96.0f;
  const float gDpiY = 96.0f;
}

class CSimpleGraphics 
{
public:
  CSimpleGraphics(const int _screenWidth, const int _screenHeight, HWND _hwnd);
  ~CSimpleGraphics();

  bool ProcessFrame(unsigned int _input);

private:
  std::unique_ptr<CDxResources> mDxRes;
  std::unique_ptr<CSimpleCamera> mCamera;

  std::unique_ptr<CModel> mModel;
  std::unique_ptr<CPlainModel> mPlainModel;

  std::unique_ptr<CTextureResource> mBgTexture;

  std::unique_ptr<CTexturingShader> mBgShader;
  std::unique_ptr<CTexturingShader> mHorizontalBlurShader;
  std::unique_ptr<CTexturingShader> mBlendShader;
  std::unique_ptr<CPhongShader> mPhongShader;

  std::unique_ptr<CTexture> mTemporaryRT;
  std::unique_ptr<CTexture> mBluredRT;
  std::unique_ptr<CTexture> mOpacityMaskTexture;

  const float mScreenWidth;
  const float mScreenHeight;
  float mRotationFactor;

  unsigned int mInput;

  bool Render();
  void CreateOpacityMask();
  void ProcessInput(unsigned int _input);
};