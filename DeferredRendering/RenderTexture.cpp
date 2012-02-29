#include "RenderTexture.h"


RenderTexture::RenderTexture(void)
{
}


RenderTexture::~RenderTexture(void)
{
    ReleaseCOM(m_srv);
    ReleaseCOM(m_rtv);
    ReleaseCOM(m_tex);
}


RenderTexture::RenderTexture(ID3D10Device* device, UINT width, UINT height, DXGI_FORMAT colorFormat)
    :m_device(device), m_width(width), m_height(height), m_format(colorFormat),
    m_srv(0),m_rtv(0),m_tex(0)
{
    BuildTexture();
    m_vp.Width = m_width;
    m_vp.Height = m_height;
    m_vp.MaxDepth = 1;
    m_vp.MinDepth = 0;
    m_vp.TopLeftX = 0;
    m_vp.TopLeftY = 0;
}

void RenderTexture::BuildTexture(void)
{
    D3D10_TEXTURE2D_DESC  texDesc;
    D3D10_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	D3D10_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
    memset(&texDesc, 0 , sizeof(texDesc));

    texDesc.Width     = m_width;
	texDesc.Height    = m_height;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 1;
	texDesc.Format    = m_format;
	texDesc.SampleDesc.Count   = 1;  
	texDesc.SampleDesc.Quality = 0;  
	texDesc.Usage          = D3D10_USAGE_DEFAULT;
	texDesc.BindFlags      = D3D10_BIND_RENDER_TARGET | D3D10_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0; 
	texDesc.MiscFlags      = D3D10_RESOURCE_MISC_GENERATE_MIPS;

    HR(m_device->CreateTexture2D(&texDesc, 0, &m_tex));


    renderTargetViewDesc.Format = texDesc.Format;
    renderTargetViewDesc.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;
    renderTargetViewDesc.Texture2D.MipSlice = 0;

	
	HR(m_device->CreateRenderTargetView(m_tex, &renderTargetViewDesc, &m_rtv));
    
    shaderResourceViewDesc.Format = texDesc.Format;
    shaderResourceViewDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    shaderResourceViewDesc .Texture2D.MostDetailedMip = 0;
    shaderResourceViewDesc .Texture2D.MipLevels =1;
	
    HR(m_device->CreateShaderResourceView(m_tex,&shaderResourceViewDesc , &m_srv));

	// View saves a reference to the texture so we can release our reference.
	ReleaseCOM(m_tex);
}

void RenderTexture::BeginRender(ID3D10DepthStencilView * dsv, D3DXCOLOR clearColor)
{
    m_device->OMSetRenderTargets(1, &m_rtv, dsv);
    m_device->RSSetViewports(1, &m_vp);
    m_device->ClearRenderTargetView(m_rtv, clearColor);
    m_device->ClearDepthStencilView(dsv, D3D10_CLEAR_DEPTH|D3D10_CLEAR_STENCIL, 1.0f, 0);
}

void RenderTexture::EndRender(void)
{
}