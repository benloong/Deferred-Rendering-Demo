#pragma once


#include "d3dapp.h"
#include "Light.h"
#include "Box.h"
#include "Quad.h"
#include "Cylinder.h"
#include "Sphere.h"
#include "Camera.h"
#include "Effects.h"
#include "InputLayouts.h"
#include "TextureMgr.h"
#include "Sky.h"
#include "DrawableTex2D.h"
#include "RenderTexture.h"
#include "NDCQuad.h"
#include "lightGeometry.h"

class DeferredDemo : public D3DApp
{
public:
    //for debug window infomation
    enum DEBUG_INFO{
        DI_NORM,
        DI_DIFFUSE,
        DI_DEPTH,
        DI_LIGHT
    };
    DeferredDemo(HINSTANCE hInstance);
    ~DeferredDemo(void);

    void initApp(void);
    void onResize(void);
    void updateScene(float dt);
    void renderScene(void);

    LRESULT msgProc(UINT msg, WPARAM wParam, LPARAM lParam);

    void genGBufferPass(void);
    void lightingPass(void);
    void shadingPass(void);


private:
    void bindRenderTargets(UINT num, ID3D10RenderTargetView **rtvs, ID3D10DepthStencilView* dsv);
    void bindViewport(D3D10_VIEWPORT const &vp);

private:
    POINT mOldMousePos;

    DEBUG_INFO dm;
	Quad mFloor;
	Box mBase;
	Sphere mBall;
	Cylinder mColumn;
	Sky mSky;

	// for drawing shadow map on screen

	DrawableTex2D mShadowMap;
    
	D3DXVECTOR4 mReflectNone;
	D3DXVECTOR4 mReflectAll;

	D3DXVECTOR4 mReflectMtrl[10];

	D3DXMATRIX mFloorWorld;
	D3DXMATRIX mBaseWorld;
	D3DXMATRIX mCenterBallWorld;
	D3DXMATRIX mColumnWorld[10];
	D3DXMATRIX mBallWorld[10];

	D3DXMATRIX mFloorTexMtx;
	D3DXMATRIX mIdentityTexMtx;


	Light mParallelLight;

	D3DXMATRIX mLightView;
	D3DXMATRIX mLightVolume;

	ID3D10ShaderResourceView* mFloorMapRV;
	ID3D10ShaderResourceView* mFloorNormalMapRV;
	ID3D10ShaderResourceView* mBaseMapRV;
	ID3D10ShaderResourceView* mBaseNormalMapRV;
	ID3D10ShaderResourceView* mBallMapRV;
	ID3D10ShaderResourceView* mColumnMapRV;
	ID3D10ShaderResourceView* mColumnNormalMapRV;
	ID3D10ShaderResourceView* mSpecMapRV;
	ID3D10ShaderResourceView* mDefaultSpecMapRV;
	ID3D10ShaderResourceView* mDefaultNormalMapRV;
	ID3D10ShaderResourceView* mEnvMapRV;
 
	ID3D10EffectTechnique* mTech;
	ID3D10EffectVariable* mfxLightVar;
	ID3D10EffectVariable* mfxEyePosVar;
	ID3D10EffectMatrixVariable* mfxLightWVPVar;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectMatrixVariable* mfxTexMtxVar;
	ID3D10EffectVectorVariable* mfxReflectMtrlVar;
	ID3D10EffectScalarVariable* mfxCubeMapEnabledVar;
	ID3D10EffectShaderResourceVariable* mfxDiffuseMapVar;
	ID3D10EffectShaderResourceVariable* mfxSpecMapVar;
	ID3D10EffectShaderResourceVariable* mfxNormalMapVar;
	ID3D10EffectShaderResourceVariable* mfxShadowMapVar;
	ID3D10EffectShaderResourceVariable* mfxCubeMapVar;


	ID3D10EffectTechnique* mBuildShadowMapTech;
	ID3D10EffectMatrixVariable* mfxBuildShadowMapLightWVPVar;
	ID3D10EffectShaderResourceVariable* mfxBuildShadowMapDiffuseMapVar;

    //NDC QUAD
    NDCQuad *   m_quad;
    DrawableTex2D  m_depthTex;
    //add gen gbuffer tech
    RenderTexture* m_normalRT;
    RenderTexture* m_diffusespecRT;
    ID3D10EffectTechnique* m_genGBufferTech;
    ID3D10EffectMatrixVariable * m_ObjectModelVar;
    ID3D10EffectMatrixVariable * m_CameraViewVar;
    ID3D10EffectMatrixVariable * m_CameraProjVar;
    ID3D10EffectMatrixVariable * m_TexMtx;
    ID3D10EffectScalarVariable * m_Shininese;
    ID3D10EffectShaderResourceVariable   * m_DiffuseMapVar;
    ID3D10EffectShaderResourceVariable *    m_SpecMapVar;
    ID3D10EffectShaderResourceVariable*     m_NormalMapVar;
    ID3D10EffectShaderResourceVariable    * m_CubeMapVar;
    ID3D10EffectScalarVariable    *         m_CubeMapEnabledVar;
    ID3D10EffectVariable  *                 m_EyePosVar;

    //lighting pass tech
    RenderTexture*  m_lightRT;
    Light           m_pointLight;
    Light           m_pointLights[5];

    LightBoxMesh*   m_pointLightMesh;
    ID3D10EffectTechnique*      m_lightSourceTech;
    ID3D10EffectTechnique*      m_pointLightTech;
    ID3D10EffectTechnique*      m_parallelLightTech;
    ID3D10EffectVariable*       m_lightDirVar;
    ID3D10EffectVariable*       m_pointLightVar;
    ID3D10EffectVariable*       m_lightPosVVar;
    ID3D10EffectMatrixVariable* m_pointLightWVPVar;
    ID3D10EffectMatrixVariable* m_pointLightWVVar;
    ID3D10EffectShaderResourceVariable*   m_depthVar;
    ID3D10EffectShaderResourceVariable*   m_gbufferVar;

    //final shading pass 
    NDCQuad *     m_finalQuad;
    ID3D10EffectTechnique*      m_shadingTech;
    ID3D10EffectShaderResourceVariable *  m_diffuseVar;
    ID3D10EffectShaderResourceVariable *  m_lightingVar;


    ID3D10RasterizerState *    m_solidMode;

	ID3D10EffectTechnique* mDrawShadowMapTech;
	ID3D10EffectShaderResourceVariable* mfxDrawShadowMapTexVar;
};

