//=============================================================================
// Effect used to display a shadow map on a quad.  The quad vertices are
// given in NDC space.
//=============================================================================
 
Texture2D gDiffuseMap;
Texture2D gLightingMap;

SamplerState gTriLinearSam
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

struct VS_IN
{
	float3 posL : POSITION;
	float2 texC : TEXCOORD;
};

struct VS_OUT
{
	float4 posH : SV_POSITION;
	float2 texC : TEXCOORD;
};
 
VS_OUT ShadingVS(VS_IN vIn)
{
	VS_OUT vOut;

	vOut.posH = float4(vIn.posL, 1.0f);
	vOut.posH.z = 0.01f;
	
	vOut.texC = vIn.texC;
	
	return vOut;
}

float4 ShadingPS(VS_OUT pIn) : SV_Target
{
	float4 diffusespec = gDiffuseMap.Sample(gTriLinearSam, pIn.texC);
	float4 lighting = gLightingMap.Sample(gTriLinearSam, pIn.texC);

	//diffuse lighting color
	float3 diff_lit = lighting.rgb;
	float3 spec_lit = lighting.a / (dot(lighting.rgb, float3(0.2126f, 0.7152f, 0.0722f)) + 1e-6f) * lighting.rgb;
	
	float3 final = max(diff_lit*diffusespec.rgb , 0 );
	return float4(final,1);
}
 
technique10 ShadingTech
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0, ShadingVS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, ShadingPS() ) );
    }
}
