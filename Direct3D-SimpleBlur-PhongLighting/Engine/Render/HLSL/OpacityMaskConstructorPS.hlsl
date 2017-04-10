struct PixelInputType
{
  float4 position : SV_POSITION;
};

cbuffer TextureWidth : register(b0)
{
  float textureWidth;
  float3 padding;
}

float OpacityMaskBuilderPS(PixelInputType input) : SV_TARGET
{
  float textureColor = 1;
  if (input.position.x > textureWidth / 2) {
    textureColor = 0;
  }

  return textureColor;
}