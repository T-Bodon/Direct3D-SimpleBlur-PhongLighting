Texture2D shaderTexture;
SamplerState SampleType;

struct PixelInputType
{
  float4 position : SV_POSITION;
  float2 tex : TEXCOORD0;
};

float normpdf(in float x, in float sigma)
{
  return 0.39894*exp(-0.5*x*x / (sigma*sigma)) / sigma;
}

float4 main(PixelInputType input) : SV_TARGET
{
  float4 color = shaderTexture.Sample(SampleType, input.tex);

  const int mSize = 11;
  const int kSize = (mSize - 1) / 2;
  float kernel[mSize];
  float4 final_colour = float4(0.0, 0.0, 0.0, 0.0);

  //create the 1-D kernel
  float sigma = 7.0;
  float Z = 0.0;
  for (int j = 0; j <= kSize; ++j)
  {
    kernel[kSize + j] = kernel[kSize - j] = normpdf(float(j), sigma);
  }
  
  //get the normalization factor (as the gaussian has been clamped)
  for (int k = 0; k < mSize; ++k)
  {
    Z += kernel[k];
  }
  
  //read out the texels
  for (int i = -kSize; i <= kSize; ++i)
  {
    for (int j = -kSize; j <= kSize; ++j)
    {
      float2 tmp = float2(i / 512.0f, j / 512.0f);
      final_colour += kernel[kSize + j] * kernel[kSize + i] *
        shaderTexture.Sample(SampleType, input.tex + tmp);  
    }
  }
  
  float4 fragColor = float4(final_colour.r / (Z*Z), final_colour.g / (Z*Z), final_colour.b / (Z*Z), 1.0);
  return fragColor;
}