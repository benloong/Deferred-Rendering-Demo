
cbuffer cbPerFrame
{
	float4x4 gView;
	float4x4 gProject;
	float3   gEyePosW;
};
cbuffer cbPerObject
{
	float4x4 gWorld;
	float4x4 gTexMtx;  //scale texcoord
	float	 shininese;
	bool	 gCubeMapEnabled;
}

Texture2D gDiffuseMap;
Texture2D gSpecMap;
Texture2D gNormalMap;
TextureCube gCubeMap;

SamplerState gTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VS_IN
{
	float3 posL     : POSITION;
	float3 tangentL : TANGENT;
	float3 normalL  : NORMAL;
	float2 texC     : TEXCOORD;
};

struct VS_OUT
{
	float4 posH : SV_POSITION;
	float4 posW  : POSITION;
	float2 texC : TEXCOORD;
	float3 tangentW : TANGENT;
	float4 normalW : NORMAL;
};
 
struct PS_OUT
{
	float4 normalshine : SV_Target0;
	float4 diffusespec : SV_Target1;
};


VS_OUT VS(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.posW = mul(float4(vIn.posL, 1.0f), gWorld);
	vOut.posH = mul(vOut.posW, gView);
	vOut.posH = mul(vOut.posH, gProject);

	vOut.tangentW = mul(float4(vIn.tangentL, 1.0f), gWorld);

	vOut.normalW = mul(float4(vIn.normalL, 0.0f), gWorld);
	//vOut.normalV = mul(vOut.normalV, gView);
	
	vOut.texC = mul(float4(vIn.texC, 0.0f, 1.0f), gTexMtx).xy;
	return vOut;
}

//rt0  store normal and shinese 
//rt1  store diffuse and specular power
PS_OUT PS(VS_OUT pIn)
{
	PS_OUT pOut;

	float4 diffuse = gDiffuseMap.Sample( gTriLinearSam, pIn.texC );
	float4 spec  = gSpecMap.Sample(gTriLinearSam, pIn.texC );
	float3 normalT = gNormalMap.Sample(gTriLinearSam, pIn.texC);
	//uncompress each component form [0,1] to [-1,1]
	normalT = 2.0f * normalT - 1.0f;

	float3 N = normalize(pIn.normalW.xyz);
	float3 T = normalize(pIn.tangentW - dot(pIn.tangentW, N) *N);
	float3 B = cross(N,T);
	float3x3 TBN = float3x3(T,B,N);
	float3 bumpedNormalW = normalize(mul(normalT, TBN));
 
	float3 bumpedNormalV = mul(float4(bumpedNormalW,0), gView);

	if(gCubeMapEnabled)
	{
		float3 incident = pIn.posW - gEyePosW;
		float3 refW = reflect(incident, bumpedNormalW);
		float4 reflectedColor = gCubeMap.Sample(gTriLinearSam, refW);
		diffuse += reflectedColor;
	}
	//store GBuffer
	pOut.normalshine = float4(bumpedNormalV, shininese);
	pOut.diffusespec = float4(diffuse.rgb, spec.a);

	return pOut;
}

technique10 GBufferTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}
