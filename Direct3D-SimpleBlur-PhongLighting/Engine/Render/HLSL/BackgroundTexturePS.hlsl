Texture2D shaderTexture;
SamplerState SampleType;

struct PixelInputType
{
  float4 position : SV_POSITION;
  float2 tex : TEXCOORD;
};

float4 BackgroundTexturePS(PixelInputType input) : SV_TARGET
{
  float4 textureColor = shaderTexture.Sample(SampleType, input.tex);

  return textureColor;
}