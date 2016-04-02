//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------
cbuffer WVPBuffer : register(b0)
{
	matrix World;
	matrix View;
	matrix Proj;
}

struct VS_INPUT
{
	float4 vPosition    : POSITION;
	float4 vColor       : COLOR;
};

struct VS_OUTPUT
{
	float4 vColor       : COLOR;
	float4 vPosition    : SV_POSITION;
};

//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
VS_OUTPUT main(VS_INPUT Input)
{
	VS_OUTPUT Output;

	float4 pos = mul(Input.vPosition, World);
	pos = mul(pos, View);
	pos = mul(pos, Proj);
	Output.vPosition = pos;

	Output.vColor = Input.vColor;
	return Output;
}