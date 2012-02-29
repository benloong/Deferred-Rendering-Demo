#include "DeferredDemo.h"
#include "LightSource.h"

DeferredDemo::DeferredDemo(HINSTANCE hinst) : D3DApp(hinst), mFloorMapRV(0), mFloorNormalMapRV(0), 
  mBaseMapRV(0), mBaseNormalMapRV(0), mBallMapRV(0), mColumnMapRV(0), 
  mColumnNormalMapRV(0), mSpecMapRV(0), mDefaultNormalMapRV(0), mEnvMapRV(0), dm(DI_NORM)
{
	
	D3DXMatrixIdentity(&mFloorWorld);
	D3DXMatrixTranslation(&mBaseWorld, 0.0f, 1.0f, 0.0f);

	D3DXMATRIX S, T;
	D3DXMatrixScaling(&S, 2.0f, 2.0f, 2.0f);
	D3DXMatrixTranslation(&T, 0.0f, 3.0f, 0.0f);
	mCenterBallWorld = S*T;
 
	for(int i = 0; i < 5; ++i)
	{
		D3DXMatrixTranslation(&mColumnWorld[i*2+0], -5.0f, 1.5f, -12.0f + i*6.0f);
		D3DXMatrixTranslation(&mColumnWorld[i*2+1], +5.0f, 1.5f, -12.0f + i*6.0f);

		D3DXMatrixTranslation(&mBallWorld[i*2+0], -5.0f, 3.5f, -12.0f + i*6.0f);
		D3DXMatrixTranslation(&mBallWorld[i*2+1], +5.0f, 3.5f, -12.0f + i*6.0f);
	}

	mReflectMtrl[0] = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
	mReflectMtrl[1] = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
	mReflectMtrl[2] = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	mReflectMtrl[3] = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	mReflectMtrl[4] = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	mReflectMtrl[5] = D3DXVECTOR4(1.0f, 0.0f, 0.0f, 1.0f);
	mReflectMtrl[6] = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	mReflectMtrl[7] = D3DXVECTOR4(0.0f, 1.0f, 0.0f, 1.0f);
	mReflectMtrl[8] = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);
	mReflectMtrl[9] = D3DXVECTOR4(0.0f, 0.0f, 1.0f, 1.0f);


	D3DXMatrixScaling(&mFloorTexMtx, 10.0f, 20.0f, 1.0f);
	D3DXMatrixIdentity(&mIdentityTexMtx);
 
	mReflectNone = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 1.0f);
	mReflectAll  = D3DXVECTOR4(1.0f, 1.0f, 1.0f, 1.0f);
}

DeferredDemo::~DeferredDemo(void)
{
    if( md3dDevice )
		md3dDevice->ClearState();


	fx::DestroyAll();
	InputLayout::DestroyAll();

    //ÊÍ·Å
    delete m_normalRT;
    delete m_quad;
    delete m_pointLightMesh;
    delete m_lightRT;
    delete m_finalQuad;
}


void DeferredDemo::initApp(void)
{
    	D3DApp::initApp();

	fx::InitAll(md3dDevice);
	InputLayout::InitAll(md3dDevice);
	GetTextureMgr().init(md3dDevice);

    m_normalRT= new RenderTexture(md3dDevice, 1024,1024, DXGI_FORMAT_R16G16B16A16_FLOAT );
    m_diffusespecRT = new RenderTexture(md3dDevice, 1024, 1024, DXGI_FORMAT_R16G16B16A16_FLOAT);
    m_lightRT = new RenderTexture(md3dDevice, 1024, 1024, DXGI_FORMAT_R16G16B16A16_FLOAT);
    m_quad = new NDCQuad(md3dDevice, D3DXVECTOR3(0.5f, -0.5f,0.0f), D3DXVECTOR3(0.5f, 0.5f,0.0f));
    m_finalQuad = new NDCQuad(md3dDevice, D3DXVECTOR3(0.0f, 0.0f,0.0f), D3DXVECTOR3(1.0f,1.0f,1.0f));

    //create solid rasterizer state
    D3D10_RASTERIZER_DESC rsDesc;
    ZeroMemory(&rsDesc, sizeof(rsDesc));
    rsDesc.CullMode = D3D10_CULL_BACK;
    rsDesc.FillMode = D3D10_FILL_SOLID ;
    rsDesc.FrontCounterClockwise = false;
    rsDesc.DepthClipEnable = true;
    
    HR(md3dDevice->CreateRasterizerState(&rsDesc, &m_solidMode));


    /*std::vector<D3DXVECTOR3> vb;
    std::vector<uint16_t>  ib;
    ::CreateConeMesh(vb,ib,0,1,2,12);*/
	mTech                = fx::ShadowFX->GetTechniqueByName("ShadowTech");
	mfxLightVar          = fx::ShadowFX->GetVariableByName("gLight");
	mfxEyePosVar         = fx::ShadowFX->GetVariableByName("gEyePosW");
	mfxLightWVPVar       = fx::ShadowFX->GetVariableByName("gLightWVP")->AsMatrix();
	mfxWVPVar            = fx::ShadowFX->GetVariableByName("gWVP")->AsMatrix();
	mfxWorldVar          = fx::ShadowFX->GetVariableByName("gWorld")->AsMatrix();
	mfxTexMtxVar         = fx::ShadowFX->GetVariableByName("gTexMtx")->AsMatrix();
	mfxReflectMtrlVar    = fx::ShadowFX->GetVariableByName("gReflectMtrl")->AsVector();
	mfxCubeMapEnabledVar = fx::ShadowFX->GetVariableByName("gCubeMapEnabled")->AsScalar();
	mfxDiffuseMapVar     = fx::ShadowFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	mfxSpecMapVar        = fx::ShadowFX->GetVariableByName("gSpecMap")->AsShaderResource();
	mfxNormalMapVar      = fx::ShadowFX->GetVariableByName("gNormalMap")->AsShaderResource();
	mfxShadowMapVar      = fx::ShadowFX->GetVariableByName("gShadowMap")->AsShaderResource();
	mfxCubeMapVar        = fx::ShadowFX->GetVariableByName("gCubeMap")->AsShaderResource();

	mBuildShadowMapTech            = fx::BuildShadowMapFX->GetTechniqueByName("BuildShadowMapTech");
	mfxBuildShadowMapLightWVPVar   = fx::BuildShadowMapFX->GetVariableByName("gLightWVP")->AsMatrix();
	mfxBuildShadowMapDiffuseMapVar = fx::BuildShadowMapFX->GetVariableByName("gDiffuseMap")->AsShaderResource();

    //GBuffer technique and technique variables
    m_genGBufferTech    =fx::GenGBufferFX->GetTechniqueByName("GBufferTech");
    m_ObjectModelVar    = fx::GenGBufferFX->GetVariableByName("gWorld")->AsMatrix();
    m_CameraViewVar     = fx::GenGBufferFX->GetVariableByName("gView")->AsMatrix();
    m_CameraProjVar     = fx::GenGBufferFX->GetVariableByName("gProject")->AsMatrix();
    m_TexMtx            = fx::GenGBufferFX->GetVariableByName("gTexMtx")->AsMatrix();
    m_Shininese         = fx::GenGBufferFX->GetVariableByName("gShininese")->AsScalar();
    m_DiffuseMapVar     = fx::GenGBufferFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
    m_SpecMapVar        = fx::GenGBufferFX->GetVariableByName("gSpecMap")->AsShaderResource();
    m_NormalMapVar      = fx::GenGBufferFX->GetVariableByName("gNormalMap")->AsShaderResource();
    m_CubeMapVar        = fx::GenGBufferFX->GetVariableByName("gCubeMap")->AsShaderResource();
    m_CubeMapEnabledVar    = fx::GenGBufferFX->GetVariableByName("gCubeMapEnabled")->AsScalar();
    m_EyePosVar         = fx::GenGBufferFX->GetVariableByName("gEyePosW");

    m_depthTex.init(md3dDevice,1024,1024,false,DXGI_FORMAT_UNKNOWN);
    //lighting pass tech 
    m_pointLightTech = fx::LightingFX->GetTechniqueByName("DeferredPointTech");
    m_parallelLightTech = fx::LightingFX->GetTechniqueByName("DeferredDirectionalTech");
    m_lightSourceTech = fx::LightingFX->GetTechniqueByName("LightTech");
    m_pointLightVar  = fx::LightingFX->GetVariableByName("gLight");
    m_lightPosVVar      = fx::LightingFX->GetVariableByName("gLightPosV");
    m_pointLightWVPVar = fx::LightingFX->GetVariableByName("gWorldViewProj")->AsMatrix();
    m_pointLightWVVar           = fx::LightingFX->GetVariableByName("gWorldView")->AsMatrix();
    m_depthVar       = fx::LightingFX->GetVariableByName("gDepthMap")->AsShaderResource();
    m_gbufferVar     = fx::LightingFX->GetVariableByName("gGBufferMap")->AsShaderResource();
    m_lightDirVar    = fx::LightingFX->GetVariableByName("gLigthDirES");

    //final shading tech
    m_shadingTech  = fx::ShadingFX->GetTechniqueByName("ShadingTech");
    m_diffuseVar   = fx::ShadingFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
    m_lightingVar  = fx::ShadingFX->GetVariableByName("gLightingMap")->AsShaderResource();

    //gen shadow map
	mDrawShadowMapTech     = fx::DrawShadowMapFX->GetTechniqueByName("DrawShadowMapTech");
	mfxDrawShadowMapTexVar = fx::DrawShadowMapFX->GetVariableByName("gShadowMap")->AsShaderResource();

	//buildNDCQuad();

	mShadowMap.init(md3dDevice, 2048, 2048, false, DXGI_FORMAT_UNKNOWN);
 
	mClearColor = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

	GetCamera().position() = D3DXVECTOR3(0.0f, 1.8f, -10.0f);

	mFloorMapRV         = GetTextureMgr().createTex(L"floor_diffuse.dds");
	mFloorNormalMapRV   = GetTextureMgr().createTex(L"floor_normal.dds");
	mBaseMapRV          = GetTextureMgr().createTex(L"stone_diffuse.dds");
	mBaseNormalMapRV    = GetTextureMgr().createTex(L"stone_normal.dds");
	mBallMapRV          = GetTextureMgr().createTex(L"blackdiffuse.dds");
	mColumnMapRV        = GetTextureMgr().createTex(L"bricks_diffuse.dds");
	mColumnNormalMapRV  = GetTextureMgr().createTex(L"bricks_normal.dds");
	mSpecMapRV          = GetTextureMgr().createTex(L"spec.dds");
	mDefaultSpecMapRV   = GetTextureMgr().createTex(L"defaultspec.dds");
	mDefaultNormalMapRV = GetTextureMgr().createTex(L"default_normal.dds");
	mEnvMapRV           = GetTextureMgr().createCubeTex(L"grassenvmap1024.dds");

	mFloor.init(md3dDevice, 41, 21, 1.0f);
	mBase.init(md3dDevice, 2.0f);
	mBall.init(md3dDevice, 0.5f, 30, 30);
	mColumn.init(md3dDevice, 0.25f, 1.0f, 3.0f, 30, 30);
	mSky.init(md3dDevice, mEnvMapRV, 5000.0f);

 
	// direction updated at runtime
	mParallelLight.ambient  = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
	mParallelLight.diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mParallelLight.specular = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);

    m_pointLight.ambient  = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
    m_pointLight.diffuse  = D3DXCOLOR(0.3f, 1.0f, 0.6f, 1.0f);
    m_pointLight.specular = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
    m_pointLight.pos       = D3DXVECTOR3(0.0f, 2.0f,3.0f);
    m_pointLight.att      =  D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    m_pointLight.range    = 3.0f;
    m_pointLightMesh = new LightBoxMesh(md3dDevice, 50.0f);

    D3DXCOLOR   colors[] = {
        WHITE,
        RED,
        GREEN,
        BLUE,
        YELLOW
    };
    for(int i = 0 ; i < 5; ++i)
    {
        float x = 2.5f * cosf(i*2*PI/5 );
        float z = 2.5f * sinf(i*2*PI/5);
        m_pointLights[i].ambient  = D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f);
        m_pointLights[i].diffuse  = colors[i];
        m_pointLights[i].specular = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
        m_pointLights[i].pos      = D3DXVECTOR3(x, 2.0f, z);
        m_pointLights[i].att      = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
        m_pointLights[i].range    = 3.0f;
    }


	D3DXMatrixOrthoLH(&mLightVolume, 30.0f, 30.0f, 1.0f, 100.0f);
}
void DeferredDemo::updateScene(float dt)
{
    D3DApp::updateScene(dt);

	// Update angles based on input to orbit camera around scene.
	if(GetAsyncKeyState('A') & 0x8000)	GetCamera().strafe(-1.3f*dt);
	if(GetAsyncKeyState('D') & 0x8000)	GetCamera().strafe(+1.3f*dt);
	if(GetAsyncKeyState('W') & 0x8000)	GetCamera().walk(+1.3f*dt);
	if(GetAsyncKeyState('S') & 0x8000)	GetCamera().walk(-1.3f*dt);
 
    if(GetAsyncKeyState('1') & 0x8000)	dm = DI_NORM;
	if(GetAsyncKeyState('2') & 0x8000)	dm = DI_DIFFUSE;
    if(GetAsyncKeyState('3') & 0x8000)  dm = DI_DEPTH;
    if(GetAsyncKeyState('4') & 0x8000)  dm = DI_LIGHT;

	GetCamera().rebuildView();

	// Animate light and keep shadow in sync.
	D3DXVECTOR3 lightPos;
	lightPos.x = 30.0f;//*cosf(0.25f*mTimer.getGameTime());
	lightPos.y = 20.0f;
	lightPos.z = 30.0f;//*sinf(0.25f*mTimer.getGameTime());

    m_pointLight.pos.x = 3.0f*cosf( mTimer.getGameTime() );
    m_pointLight.pos.z = 5.0f*sinf( mTimer.getGameTime() );

    m_pointLights[0].pos.x = 2.5f * cosf( mTimer.getGameTime() );
	D3DXMatrixLookAtLH(&mLightView, &lightPos,
		&D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	D3DXVECTOR3 lightDirection = -lightPos;
	D3DXVec3Normalize(&mParallelLight.dir, &lightDirection);
}
void DeferredDemo::renderScene(void)
{
    D3DApp::renderScene();
	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(0, blendFactor, 0xffffffff);
	
	md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	md3dDevice->IASetInputLayout(InputLayout::PosTangentNormalTex);



    //Render normal to texture
    ID3D10RenderTargetView * views[] = { m_normalRT->GetRTV(), m_diffusespecRT->GetRTV()};
    this->bindRenderTargets(2, views, m_depthTex.DepthStecilView());
    this->bindViewport(m_normalRT->GetViewPort());
    this->genGBufferPass();
    m_normalRT->EndRender();

    //lighting pass
    this->lightingPass();

    resetOMTargetsAndViewport();
	

    //final shading pass
	this->shadingPass();

	mSky.draw();


	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	md3dDevice->RSSetState(0);
	mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, WHITE);

	mSwapChain->Present(0, 0);
}

void DeferredDemo::onResize(void)
{
    D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	GetCamera().setLens(0.25f*PI, aspect, 0.5f, 1000.0f);
}

LRESULT DeferredDemo::msgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	POINT mousePos;
	int dx = 0;
	int dy = 0;
	switch(msg)
	{
	case WM_LBUTTONDOWN:
		if( wParam & MK_LBUTTON )
		{
			SetCapture(mhMainWnd);

			mOldMousePos.x = LOWORD(lParam);
			mOldMousePos.y = HIWORD(lParam);
		}
		return 0;

	case WM_LBUTTONUP:
		ReleaseCapture();
		return 0;

	case WM_MOUSEMOVE:
		if( wParam & MK_LBUTTON )
		{
			mousePos.x = (int)LOWORD(lParam); 
			mousePos.y = (int)HIWORD(lParam); 

			dx = mousePos.x - mOldMousePos.x;
			dy = mousePos.y - mOldMousePos.y;

			GetCamera().pitch( dy * 0.0087266f );
			GetCamera().rotateY( dx * 0.0087266f );
			
			mOldMousePos = mousePos;
		}
		return 0;
  
	}

	return D3DApp::msgProc(msg, wParam, lParam);
}

void DeferredDemo::genGBufferPass(void)
{       	
    md3dDevice->IASetInputLayout(InputLayout::PosTangentNormalTex);

	// Set per frame constants.
 
	D3DXMATRIX view = GetCamera().view();
    D3DXMATRIX proj = GetCamera().proj();
    D3D10_TECHNIQUE_DESC techDesc;
    m_genGBufferTech->GetDesc( &techDesc );
    m_CameraProjVar->SetMatrix((float*)&proj);
    m_CameraViewVar->SetMatrix((float*)&view);
    m_CubeMapVar->SetResource(mEnvMapRV);
    m_EyePosVar->SetRawValue(&GetCamera().position(), 0, sizeof(D3DXVECTOR3));

    for(UINT i = 0; i < techDesc.Passes; ++i)
    {
        ID3D10EffectPass* pass = m_genGBufferTech->GetPassByIndex(i);

        
		//
		// draw floor
		//
        m_ObjectModelVar->SetMatrix((float*)&mFloorWorld);
        m_TexMtx->SetMatrix((float*)&mFloorTexMtx);
        m_DiffuseMapVar ->SetResource(mFloorMapRV);
        m_SpecMapVar->SetResource(mSpecMapRV);
        m_NormalMapVar->SetResource(mFloorNormalMapRV);
        m_CubeMapEnabledVar->SetBool(false);
        m_Shininese ->SetFloat(1.0f);
		pass->Apply(0);
		mFloor.draw();
		//
		// draw base
		//
		
        m_ObjectModelVar->SetMatrix((float*)&mBaseWorld);
        m_TexMtx->SetMatrix((float*)&mIdentityTexMtx);
        m_DiffuseMapVar->SetResource(mBaseMapRV);
        m_SpecMapVar->SetResource(mSpecMapRV);
        m_NormalMapVar->SetResource(mBaseNormalMapRV);
        m_CubeMapEnabledVar->SetBool(false);
        m_Shininese->SetFloat(1.0f);
		pass->Apply(0);
		mBase.draw();

		//
		// draw center ball
		//
        m_ObjectModelVar->SetMatrix((float*)&mCenterBallWorld);
        m_TexMtx->SetMatrix((float*)&mIdentityTexMtx);
        m_DiffuseMapVar->SetResource(mBallMapRV);
        m_NormalMapVar->SetResource(mDefaultNormalMapRV);
        m_SpecMapVar->SetResource(mDefaultSpecMapRV);
        m_CubeMapEnabledVar->SetBool(true);
        m_Shininese->SetFloat(1.0f);
		pass->Apply(0);
		mBall.draw();

		//
		// draw columns
		//
        m_TexMtx->SetMatrix((float*)&mIdentityTexMtx);
        m_DiffuseMapVar->SetResource(mColumnMapRV);
        m_SpecMapVar->SetResource(mDefaultSpecMapRV);
        m_CubeMapEnabledVar->SetBool(false);
        m_NormalMapVar->SetResource(mColumnNormalMapRV);
        m_Shininese->SetFloat(1.0f);
		for(int i = 0; i < 10; ++i)
		{
			//m_CameraWVVar->SetMatrix((float*)&(mColumnWorld[i]*view));
            m_ObjectModelVar->SetMatrix((float*)&mColumnWorld[i]);
	    	
			pass->Apply(0);
			mColumn.draw();
		}	
		//
		// draw balls
		//
        m_TexMtx->SetMatrix((float*)&mIdentityTexMtx);
        m_DiffuseMapVar->SetResource(mBallMapRV);
        m_SpecMapVar->SetResource(mDefaultSpecMapRV);
        m_CubeMapEnabledVar->SetBool(true);
        m_NormalMapVar->SetResource(mDefaultNormalMapRV);
        m_Shininese->SetFloat(1.0f);
		for(int i = 0; i < 10; ++i)
		{
			//m_CameraWVVar->SetMatrix((float*)&(mBallWorld[i]*view));
            m_ObjectModelVar->SetMatrix((float*)&mBallWorld[i]);
            m_TexMtx->SetMatrix((float*)&mIdentityTexMtx);
			
			pass->Apply(0);
			mBall.draw();
		}

    }
}

//lighting pass renderring lighting information to lighting texture
//input GBuffer normal map
void DeferredDemo::lightingPass(void)
{
    //per light source
    //this->resetOMTargetsAndViewport();
    ID3D10RenderTargetView * rt = m_lightRT->GetRTV();
    this->bindRenderTargets(1, &rt, NULL);
    this->bindViewport(m_lightRT->GetViewPort());
    	
    D3DXMATRIX view = GetCamera().view();
    D3DXMATRIX proj = GetCamera().proj();

    m_pointLightVar->SetRawValue(&m_pointLight,0, sizeof(m_pointLight));

    m_depthVar->SetResource(m_depthTex.depthMap());
    m_gbufferVar->SetResource(m_normalRT->GetSRV());

    D3DXMATRIX mtx;
    D3DXMatrixTranslation(&mtx,m_pointLight.pos.x, m_pointLight.pos.y, m_pointLight.pos.z);

    D3DXMATRIX mvp = (mtx*view*proj);
    m_pointLightWVPVar->SetMatrix((float*)&mvp);
    D3DXVECTOR4 lightposview;
    ::D3DXVec3Transform(&lightposview, &m_pointLight.pos, &(view)); 
    D3DXVECTOR3 l ;
    l.x = lightposview.x;
    l.y = lightposview.y; 
    l.z = lightposview.z;

    m_lightPosVVar->SetRawValue(&l,0, sizeof(D3DXVECTOR4));
  
    m_pointLightWVVar->SetMatrix((float*)&(mtx*view));

    D3D10_TECHNIQUE_DESC techDesc;

    m_pointLightTech->GetDesc( &techDesc );

    for(uint32_t i = 0 ; i < techDesc.Passes; ++i)
    {
        ID3D10EffectPass * pass = m_pointLightTech->GetPassByIndex(i);
        

        pass->Apply(0);
        m_pointLightMesh->Draw();

    }


    for(int i = 0 ;  i < 5; ++i)
    {
        D3DXMATRIX mtx;
        D3DXMatrixTranslation(&mtx,m_pointLights[i].pos.x, m_pointLights[i].pos.y, m_pointLights[i].pos.z);

        m_pointLightVar->SetRawValue(&m_pointLights[i], 0 ,sizeof(m_pointLights[i]));
        D3DXMATRIX mvp = (mtx*view*proj);
        m_pointLightWVPVar->SetMatrix((float*)&mvp);
        D3DXVECTOR4 lightposview;
        ::D3DXVec3Transform(&lightposview, &m_pointLights[i].pos, &(view)); 
        D3DXVECTOR3 l ;
        l.x = lightposview.x;
        l.y = lightposview.y; 
        l.z = lightposview.z;
    
        m_lightPosVVar->SetRawValue(&l,0, sizeof(D3DXVECTOR4));
      
        m_pointLightWVVar->SetMatrix((float*)&(mtx*view));

        ID3D10EffectPass * pass = m_pointLightTech->GetPassByIndex(0);
        

        pass->Apply(0);
        m_pointLightMesh->Draw();
    }
    //m_pointLightWVPVar->SetMatrix
    m_pointLightVar->SetRawValue(&mParallelLight,0, sizeof(Light));
    D3DXVECTOR4 l_dir_es;
    D3DXVECTOR4 l_dir_ws;
    l_dir_ws.x = mParallelLight.dir.x;
    l_dir_ws.y = mParallelLight.dir.y;
    l_dir_ws.z = mParallelLight.dir.z;
    l_dir_ws.w = 0;
    
    ::D3DXVec4Transform(&l_dir_es, &l_dir_ws, &view);
    m_lightDirVar->SetRawValue(&l_dir_es, 0, sizeof(D3DXVECTOR3));
    m_parallelLightTech->GetDesc( &techDesc);
    for(uint32_t i = 0; i < techDesc.Passes; ++i)
    {
        ID3D10EffectPass * pass = m_parallelLightTech->GetPassByIndex(i);
        pass->Apply(0);
        m_pointLightMesh->Draw();
    }

    md3dDevice->RSSetState(m_solidMode);
    float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(0, blendFactor, 0xffffffff);

    //draw light source mesh
    m_lightSourceTech->GetDesc(&techDesc);
    D3DXMATRIX scale;
    ::D3DXMatrixScaling(&scale,0.001f,0.001f,0.001f);
    m_pointLightWVPVar->SetMatrix((float*)&(scale * mvp));
    ID3D10EffectPass * pass = m_lightSourceTech->GetPassByIndex(0);
    pass->Apply(0);
    m_pointLightMesh->Draw();

    for(int i = 0 ; i < 5; ++i)
    {
        D3DXMATRIX scale;
        ::D3DXMatrixScaling(&scale,0.001f,0.001f,0.001f);
        D3DXMATRIX mtx;
        D3DXMatrixTranslation(&mtx,m_pointLights[i].pos.x, m_pointLights[i].pos.y, m_pointLights[i].pos.z);

        m_pointLightVar->SetRawValue(&m_pointLights[i], 0 ,sizeof(Light));
        D3DXMATRIX mvp = (mtx*view*proj);
        m_pointLightWVPVar->SetMatrix((float*)&(scale * mvp));
        ID3D10EffectPass * pass = m_lightSourceTech->GetPassByIndex(0);
        pass->Apply(0);
        m_pointLightMesh->Draw();
    }
}

//final pass  shading object to backsurface
//input GBuffer diffuse map, lighting map
void DeferredDemo::shadingPass(void)
{
    md3dDevice->IASetInputLayout(InputLayout::PosTex);

    m_diffuseVar->SetResource(m_diffusespecRT->GetSRV());
    m_lightingVar->SetResource(m_lightRT->GetSRV());
    ID3D10EffectPass* pass = m_shadingTech->GetPassByIndex(0);
    pass->Apply(0);
    m_finalQuad->draw();
    //debug window
    switch(dm){
    case DI_NORM:
        mfxDrawShadowMapTexVar->SetResource(m_normalRT->GetSRV());
        break;
    case DI_DIFFUSE:
        mfxDrawShadowMapTexVar->SetResource(m_diffusespecRT->GetSRV());
        break;
    case DI_DEPTH:
        mfxDrawShadowMapTexVar->SetResource(m_depthTex.depthMap());
        break;
    case DI_LIGHT:
        mfxDrawShadowMapTexVar->SetResource(m_lightRT->GetSRV());

    }
	pass = mDrawShadowMapTech->GetPassByIndex(0);
	pass->Apply(0);
    m_quad->draw();
}

void DeferredDemo::bindRenderTargets(UINT num, ID3D10RenderTargetView **views, ID3D10DepthStencilView* dsv)
{
    md3dDevice->OMSetRenderTargets(num, views, dsv);
    for(int i = 0 ; i < num; ++i)
    {
        float color[4] = { 0,0,0,0};
        md3dDevice->ClearRenderTargetView(views[i], color);
    }
    if(dsv)
        md3dDevice->ClearDepthStencilView(dsv, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);
}

void DeferredDemo::bindViewport(const D3D10_VIEWPORT & vp)
{
    md3dDevice->RSSetViewports(1, &vp);
}
