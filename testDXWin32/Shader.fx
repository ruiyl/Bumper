//--------------------------------------------------------------------------------------
// File: Tutorial05.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer ConstantBuffer : register(b0)
{
	matrix View;
}
cbuffer ConstantBuffer : register(b1)
{
	matrix Projection;
}
cbuffer ConstantBuffer : register(b2)
{
	matrix World;
}
cbuffer ConstantBuffer : register(b3)
{
	float4 Color;
}

//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos : POSITION;
	float2 TexCoord : TEXCOORD;
	float3 normal : NORMAL;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 TexCoord : TEXCOORD;
	float3 normal : NORMAL;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;
	output.Pos = mul(input.Pos, World);
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.TexCoord = input.TexCoord;
	output.normal = input.normal;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS0(PS_INPUT input) : SV_Target
{
	return txDiffuse.Sample(samLinear, input.TexCoord);
}

float4 PS1(PS_INPUT input) : SV_Target
{
	return Color;
}