//--------------------------------------------------------------------------------------
// Input / Output structures
//--------------------------------------------------------------------------------------
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
	Output.vPosition = Input.vPosition;
	Output.vColor = Input.vColor;
	return Output;
}