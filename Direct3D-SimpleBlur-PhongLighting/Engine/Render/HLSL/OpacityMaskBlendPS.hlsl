Texture2D shaderTexture[3];
SamplerState SampleType;

struct PixelInputType
{
  float4 position : SV_POSITION;
  float2 tex : TEXCOORD;
};

float4 BlendPS(PixelInputType input) : SV_TARGET
{
  float4 textureColor;

  float4 clearColor = shaderTexture[0].Sample(SampleType, input.tex);
  float4 bluredColor = shaderTexture[1].Sample(SampleType, input.tex);

  float alpha = shaderTexture[2].Sample(SampleType, input.tex).r;
  float invAlpha = 1 - alpha;
  clearColor = float4(clearColor.r * alpha, clearColor.g * alpha, clearColor.b * alpha, clearColor.a * alpha);
  bluredColor = float4(bluredColor.r * invAlpha, bluredColor.g * invAlpha, bluredColor.b * invAlpha, bluredColor.a * invAlpha);

  textureColor = clearColor + bluredColor;
  return textureColor;
}