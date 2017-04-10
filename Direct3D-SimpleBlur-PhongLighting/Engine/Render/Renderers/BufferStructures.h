#pragma once
#include "..\helpers\Dx.h"

struct MatrixBufferType {
  DirectX::XMMATRIX mWorld;
  DirectX::XMMATRIX mView;
  DirectX::XMMATRIX mProjection;
};

struct _Material {
  _Material()
    : mEmissive(0.0f, 0.0f, 0.0f, 1.0f), mAmbient(0.1f, 0.1f, 0.1f, 1.0f),
    mDiffuse(1.0f, 1.0f, 1.0f, 1.0f), mSpecular(1.0f, 1.0f, 1.0f, 1.0f),
    mSpecularPower(128.0f) {
  }

  DirectX::XMFLOAT4 mEmissive;
  DirectX::XMFLOAT4 mAmbient;
  DirectX::XMFLOAT4 mDiffuse;
  DirectX::XMFLOAT4 mSpecular;
  float mSpecularPower;
  float mPadding[3];
};

struct MaterialProperties {
  _Material mMaterial;
};

enum LightType {
  DirectionalLight = 49,
  PointLight = 50,
  SpotLight = 51
};

struct Light {
  Light()
    : mPosition(0.0f, 0.0f, 0.0f, 1.0f), mDirection(0.0f, 0.0f, 1.0f, 0.0f),
    mColor(1.0f, 1.0f, 1.0f, 1.0f), mSpotAngle(DirectX::XM_PI / 16),
    mConstantAttenuation(1.0f), mLinearAttenuation(0.0f), 
    mQuadraticAttenuation(0.0f), mLightType(DirectionalLight), Enabled(0) {
  }

  DirectX::XMFLOAT4 mPosition;
  DirectX::XMFLOAT4 mDirection;
  DirectX::XMFLOAT4 mColor;
  float mSpotAngle;
  float mConstantAttenuation;
  float mLinearAttenuation;
  float mQuadraticAttenuation;
  int mLightType;
  int Enabled;
  int mPadding[2];
};

struct LightProperties {
  LightProperties()
    : mEyePosition(0.0f, 0.0f, 0.0f, 1.0f), mGlobalAmbient(0.2f, 0.2f, 0.8f, 1.0f) {
  }

  DirectX::XMFLOAT4 mEyePosition;
  DirectX::XMFLOAT4 mGlobalAmbient;
  Light mLightingData;
};

struct TextureWidthBuffer {
  float mTextureWidth;
  DirectX::XMFLOAT3 mPadding;
};