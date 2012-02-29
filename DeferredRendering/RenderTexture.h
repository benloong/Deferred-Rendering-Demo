#pragma once
#include "d3dutil.h"
class RenderTexture
{
public:
    RenderTexture(void);
    ~RenderTexture(void);
    
    RenderTexture(ID3D10Device* device, UINT width, UINT height, DXGI_FORMAT colorFormat);
    ID3D10ShaderResourceView * GetSRV() const // return ShaderResourceView;
    {
        return m_srv;
    }
    ID3D10RenderTargetView  *  GetRTV() const //return rendertargetView;
    {
        return m_rtv;
    }
    D3D10_VIEWPORT const &      GetViewPort() const
    {
        return m_vp;
    }

    //bind this render target to pipeline 
    void BeginRender(ID3D10DepthStencilView* dsv, D3DXCOLOR clearColor);

    void EndRender(void);

private:
    //set object noncopyable
    RenderTexture(RenderTexture &rhs);
    RenderTexture&  operator=(RenderTexture &rhs);

    void BuildTexture(void);

private:
    ID3D10Device*   m_device;
    ID3D10ShaderResourceView * m_srv;
    ID3D10RenderTargetView  *  m_rtv;
    ID3D10Texture2D     *       m_tex;

    UINT        m_width;
    UINT        m_height;

    DXGI_FORMAT m_format;
    D3D10_VIEWPORT     m_vp;

};

