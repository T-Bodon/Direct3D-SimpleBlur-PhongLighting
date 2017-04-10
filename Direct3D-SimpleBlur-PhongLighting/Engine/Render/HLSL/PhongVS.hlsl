cbuffer MatrixBuffer : register(b0)
{
  matrix ViewProjectionMatrix;
  matrix WorldMatrix;
  matrix InvertedWorldMatrix;
}

struct VertexInputType
{
  float4 position : POSITION;
  float3 normal : NORMAL;
};

struct VertexShaderOutput
{
  float4 PositionWS : TEXCOORD1;
  float3 NormalWS : TEXCOORD2;
  float4 mPosition : SV_Position;
};

VertexShaderOutput LightingVS(VertexInputType IN)
{
  VertexShaderOutput OUT;

  OUT.mPosition = mul(IN.position, WorldMatrix);
  OUT.mPosition = mul(OUT.mPosition, ViewProjectionMatrix);
  OUT.PositionWS = mul(IN.position, WorldMatrix);
  OUT.NormalWS = mul(IN.normal, (float3x3)InvertedWorldMatrix);

  return OUT;
}