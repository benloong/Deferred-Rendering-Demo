//=============================================================================
// lighthelper.fx by Frank Luna (C) 2008 All Rights Reserved.
//
// Structures and functions for lighting calculations.
//=============================================================================


struct Light
{
	float3 pos;
	float3 dir;
	float4 ambient;
	float4 diffuse;
	float4 spec;
	float3 att;
	float  spotPower;
	float  range;

};


cbuffer cbPerLight
{
	Light		gLight;
	float4		gLightPosV;
	float3		gLigthDirES;
	float4x4	gWorldViewProj;
	float4x4	gWorldView;
};

Texture2D		gDepthMap;
Texture2D		gGBufferMap;

SamplerState  gDepthSam
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
};

BlendState   gLightingBlend
{
	BlendEnable[0] = TRUE;
    SrcBlend = ONE; 
    DestBlend = ONE;
    BlendOp = ADD;
    SrcBlendAlpha = ZERO;
    DestBlendAlpha = ZERO;
    BlendOpAlpha = ADD;
    RenderTargetWriteMask[0] = 0x0F;
};

//cull front face and set far plane infinite
RasterizerState gCullFrontFace
{
	//FillMode = Wireframe;
	CullMode = Front;
};

void DeferredVS(in float3 posL : POSITION, out float4 posH :SV_POSITION, out float3 posV : POSITION, out float4 projTexCoord : TEXCOORD)
{
	
	posH = mul(float4(posL,1.0f), gWorldViewProj);
	posV = mul(float4(posL,1.0f), gWorldView).xyz;
	
	projTexCoord = posH;
}

//linearize projected depth
//n : near plane
//f : far plane
//depth: depthmap value
float linearize_depth(float n, float f, float depth)
{
	float q = f/(f-n);
	return q*n / (q-depth);
}

float spec_term(float3 halfway, float3 normal, float roughness)
{
	return pow(max(dot(halfway, normal), 0.0f), roughness);
}
float att_term(float3 light_pos, float3 pos, float3 atten)
{
	float d = distance(light_pos, pos);
	return 1 / (atten.x + d * atten.y + d*d * atten.z);
}

float2 trans_coord(float4 coord)
{
	coord.xyz /= coord.w;
	return float2(0.5f * coord.x + 0.5f,  -0.5f * coord.y + 0.5f);
}

float2 shadow_term(float3 pos_es, float  shadow_depth)
{

}
//posV: lighting volume position in view space
void DeferredPointPS(in float4 posH : SV_POSITION, in float3 posV : POSITION, in float4 projTexCoord: TEXCOORD, out float4 color : SV_Target)
{
	//为方便使用摄像机默认的近裁剪面和远裁剪面 near0.5   far1000
	
	float2 coord = trans_coord(projTexCoord);
	//fetch depth
	float depth_non_linear = gDepthMap.Sample(gDepthSam, coord).r;
	//linearize depth
	float depth_linear = linearize_depth(0.5f, 1000.0f, depth_non_linear);

	//reconstruct geometry position in view space
	float3 view_dir = normalize(posV);
	float3 pos_view = view_dir * (depth_linear / view_dir.z);
	//fetch normal
	float4 normspec = gGBufferMap.Sample(gDepthSam, coord);
	float3 norm = normalize((normspec.xyz ));
	//light position in view space 
	float3 lightpos_view = gLightPosV.xyz;
	//light direction
	float3 l_dir = normalize(lightpos_view - pos_view);
	float n_dot_l = max(dot(norm, l_dir),0);
	//n_dot_l = max(n_dot_l,0);
	float specTerm = spec_term(normalize(l_dir- view_dir), norm, normspec.w*256);
	float atten = att_term(lightpos_view, pos_view, gLight.att);

	float2 ds = atten * n_dot_l * float2(1, specTerm);
	ds = max(ds,0);

	//float  d  = length(lightpos_view);

	color = ds.xxxy * gLight.diffuse;
	//color = float4(norm,0);
	//color = float4(view_dir, 0);
}

void DeferredSpotPS(in float4 posH : SV_POSITION, in float3 posV : POSITION, in float4 projCoord : TEXCOORD, out float4 color : SV_Target)
{
}

void DeferredDirectionalPS(in float4 posH :SV_POSITION, in float3 posV :POSITION, in float4 projCoord : TEXCOORD, out float4 color : SV_Target)
{
	//get ndc texture coord
	float2 coord = trans_coord(projCoord);
	float4 normspec = gGBufferMap.Sample(gDepthSam, coord);
	float3 norm  = normalize(normspec.xyz);
	float  spec  = normspec.w * 256.0f;
	float  depth_linear = linearize_depth(0.5f, 1000.0f, gDepthMap.Sample(gDepthSam, coord).r);

	//eye position is (0,0,0)
	float3 view_dir = normalize(posV);
	//reconstruction pixel position in eye space
	float3 pos_es = view_dir * depth_linear / view_dir.z;
	float3 l_dir = - gLigthDirES;
	float n_dot_l = max(dot(norm, l_dir),0);

	float spec_term_ = spec_term(normalize(l_dir - view_dir), norm, spec);
	float2 ds = n_dot_l * float2(1, spec_term_);
	ds = max(ds,0);

	//color = ds.xxxy * gLight.diffuse;
	color = 0;
}

//for ambient lighting
void DeferredAmbientPS(in float4 posH : SV_POSITION, in float3 posV : POSITION, in float4 projCoord : TEXCOORD, out float4 color: SV_Target)
{
}
 
 

 void LightVS(in float3 posL : POSITION,  out float4 posH : SV_POSITION)
 {
		posH = mul(float4(posL, 1.0f), gWorldViewProj);
 }

 void LightPS(in float4 posH : SV_POSITION, out float4 c : SV_Target)
 {
	c = gLight.diffuse;
 }
 technique10 DeferredPointTech
 {
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, DeferredVS()));
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader( ps_4_0, DeferredPointPS()));
		SetRasterizerState(gCullFrontFace);
		SetBlendState(gLightingBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
	}
 }

 technique10 DeferredSpotTech
 {
	pass P0
	{

	}
 }

 technique10 DeferredDirectionalTech
 {
	pass P0
	{
		SetVertexShader( CompileShader( vs_4_0, DeferredVS()));
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader( ps_4_0, DeferredDirectionalPS()));
		SetRasterizerState(gCullFrontFace);
		SetBlendState(gLightingBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
	}
 }
 technique10 DeferredAmbientTech
 {
	pass P0
	{
		SetVertexShader( CompileShader(vs_4_0, DeferredVS()));
		SetGeometryShader(NULL);
		SetPixelShader( CompileShader( ps_4_0, DeferredAmbientPS()));
		SetRasterizerState(gCullFrontFace);
		SetBlendState(gLightingBlend, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
	}
 }

 //for render lighting gizmo
 technique10 LightTech
 {
	pass P0
	{
		SetVertexShader(CompileShader(vs_4_0, LightVS()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_4_0, LightPS()));
	}
 }
