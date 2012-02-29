#include "NDCQuad.h"
#include "InputLayouts.h"

struct QuadVertex {
    D3DXVECTOR3   pos;
    D3DXVECTOR2   tex;
};

NDCQuad::NDCQuad(ID3D10Device * device, D3DXVECTOR3 const& pos, D3DXVECTOR3 const & scale):m_device(device)
{
    QuadVertex  v[6] = { 
        {D3DXVECTOR3(-1*scale.x,-1*scale.y,0) + pos, D3DXVECTOR2(0,1)},
        {D3DXVECTOR3(-1*scale.x, 1*scale.y,0) + pos, D3DXVECTOR2(0,0)},
        {D3DXVECTOR3(1 *scale.x, 1*scale.y,0) + pos, D3DXVECTOR2(1,0)},
        {D3DXVECTOR3(-1*scale.x,-1*scale.y,0) + pos, D3DXVECTOR2(0,1)},
        {D3DXVECTOR3( 1*scale.x, 1*scale.y,0) + pos, D3DXVECTOR2(1,0)},
        {D3DXVECTOR3( 1*scale.x,-1*scale.y,0) + pos, D3DXVECTOR2(1,1)}
    };

    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
	vbd.ByteWidth = sizeof(QuadVertex) * 6;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = v;
    HR(m_device->CreateBuffer(&vbd, &vinitData, &m_vertexBuffer));
}


NDCQuad::~NDCQuad(void)
{
}

void NDCQuad::draw(void)
{
    m_device->IASetInputLayout(InputLayout::PosTex);
    UINT stride = sizeof(QuadVertex);
    UINT offset = 0;
    m_device->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	m_device->Draw(6, 0);
}

void NDCQuad::Render(void)
{

}