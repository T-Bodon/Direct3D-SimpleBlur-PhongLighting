// Light types.
#define DIRECTIONAL_LIGHT 49
#define POINT_LIGHT 50
#define SPOT_LIGHT 51

struct _Material
{
  float4 mEmissive;
  float4 mAmbient;
  float4 mDiffuse;
  float4 mSpecular;
  float mSpecularPower;
  float2 mPadding;
};

cbuffer MaterialProperties : register(b0)
{
  _Material mMaterial;
};

struct Light
{
  float4 mPosition;
  float4 mDirection;
  float4 mColor;
  float mSpotAngle;
  float mConstantAttenuation;
  float mLinearAttenuation;
  float mQuadraticAttenuation;
  int mLightType;
  bool Enabled;
  int2 mPadding;
};

cbuffer LightProperties : register(b1)
{
  float4 mEyePosition;
  float4 mGlobalAmbient;
  Light mLightingData;
};

struct PixelInputType
{
  float4 PositionWS : TEXCOORD1;
  float3 NormalWS : TEXCOORD2;
  float2 TexCoord : TEXCOORD0;
  float4 mPosition : SV_Position;
};

float4 DoDiffuse(Light light, float3 L, float3 N)
{
  float NdotL = max(0, dot(N, L));
  
  return light.mColor * NdotL;
}

float4 DoSpecular(Light light, float3 V, float3 L, float3 N)
{
  // Phong lighting.
  float3 R = normalize(reflect(-L, N));
  float RdotV = max(0, dot(R, V));

  // Blinn-Phong lighting
  float3 H = normalize(L + V);
  float NdotH = max(0, dot(N, H));

  return light.mColor * pow(RdotV, mMaterial.mSpecularPower);
}

float DoAttenuation(Light light, float d)
{
  return 1.0f / (light.mConstantAttenuation + light.mLinearAttenuation * d + light.mQuadraticAttenuation * d * d);
}

struct LightingResult
{
  float4 mDiffuse;
  float4 mSpecular;
};

LightingResult DoPointLight(Light light, float3 V, float4 P, float3 N)
{
  LightingResult result;

  float3 L = (light.mPosition - P).xyz;
  float distance = length(L);
  L = L / distance;

  float attenuation = DoAttenuation(light, distance);

  result.mDiffuse = DoDiffuse(light, L, N) * attenuation;
  result.mSpecular = DoSpecular(light, V, L, N) * attenuation;

  return result;
}

LightingResult DoDirectionalLight(Light light, float3 V, float4 P, float3 N)
{
  LightingResult result;
  
  float3 L = -light.mDirection.xyz;

  result.mDiffuse = DoDiffuse(light, L, N);
  result.mSpecular = DoSpecular(light, V, L, N);

  return result;
}

float DoSpotCone(Light light, float3 L)
{
  float minCos = cos(light.mSpotAngle);
  float maxCos = (minCos + 1.0f) / 2.0f;
  float cosAngle = dot(light.mDirection.xyz, -L);
  return smoothstep(minCos, maxCos, cosAngle);
}

LightingResult DoSpotLight(Light light, float3 V, float4 P, float3 N)
{
  LightingResult result;

  float3 L = (light.mPosition - P).xyz;
  float distance = length(L);
  L = L / distance;

  float attenuation = DoAttenuation(light, distance);
  float spotIntensity = DoSpotCone(light, L);

  result.mDiffuse = DoDiffuse(light, L, N) * attenuation * spotIntensity;
  result.mSpecular = DoSpecular(light, V, L, N) * attenuation * spotIntensity;

  return result;
}

LightingResult ComputeLighting(float4 P, float3 N)
{
  float3 V = normalize(mEyePosition - P).xyz;

  LightingResult totalResult = { { 0, 0, 0, 0 },{ 0, 0, 0, 0 } };

  {
    LightingResult result = { { 0, 0, 0, 0 },{ 0, 0, 0, 0 } };

    if (mLightingData.mLightType == DIRECTIONAL_LIGHT) {
      result = DoDirectionalLight(mLightingData, V, P, N);
    }
    if (mLightingData.mLightType == POINT_LIGHT) {
      result = DoPointLight(mLightingData, V, P, N);
    }
    if (mLightingData.mLightType == SPOT_LIGHT) {
      result = DoSpotLight(mLightingData, V, P, N);
    }

    totalResult.mDiffuse += result.mDiffuse;
    totalResult.mSpecular += result.mSpecular;
  }

  totalResult.mDiffuse = saturate(totalResult.mDiffuse);
  totalResult.mSpecular = saturate(totalResult.mSpecular);

  return totalResult;
}

float4 TexturedLitPixelShader(PixelInputType IN) : SV_TARGET
{
  LightingResult lit = ComputeLighting(IN.PositionWS, normalize(IN.NormalWS));

float4 emissive = mMaterial.mEmissive;
float4 ambient = mMaterial.mAmbient * mGlobalAmbient;
float4 diffuse = mMaterial.mDiffuse * lit.mDiffuse;
float4 specular = mMaterial.mSpecular * lit.mSpecular;

float4 texColor = { 1, 1, 1, 1 };

float4 finalColor = (emissive + ambient + diffuse + specular) * texColor;

return finalColor;
}