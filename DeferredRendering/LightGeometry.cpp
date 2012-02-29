#include "LightGeometry.h"
#include "InputLayouts.h"
#include "LightSource.h"

LightGeometry::LightGeometry(ID3D10Device *device):m_device(device)
{
}


LightGeometry::~LightGeometry(void)
{
}


LightConeMesh::LightConeMesh(ID3D10Device * device, float radius, float height, uint16_t n) : LightGeometry(device)
{
    std::vector<D3DXVECTOR3> vb;
    std::vector<uint16_t>    ib;
    ::CreateConeMesh(vb, ib,0,radius, height, n);
    m_numIndex = ib.size();

    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(D3DXVECTOR3) * vb.size();
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;

    D3D10_SUBRESOURCE_DATA  vinitData;
    vinitData.pSysMem = &vb[0];
    HR(m_device->CreateBuffer(&vbd, &vinitData, &m_vb));

    vbd.ByteWidth = sizeof(ib[0])*ib.size();
    vbd.BindFlags = D3D10_BIND_INDEX_BUFFER;

    vinitData.pSysMem = &ib[0];
    HR(m_device->CreateBuffer(&vbd, &vinitData, &m_ib));
}

void LightConeMesh::Draw(void)
{
    m_device->IASetInputLayout(InputLayout::Pos);
    UINT stride = sizeof(D3DXVECTOR3);
    UINT offset = 0;
    m_device->IASetVertexBuffers(0,1,&m_vb,&stride,&offset);
    m_device->IASetIndexBuffer(m_ib, DXGI_FORMAT_R16_UINT , 0);
    m_device->DrawIndexed(m_numIndex,0,0);
}


LightBoxMesh::LightBoxMesh(ID3D10Device *device, float radius): LightGeometry(device)
{
    std::vector<D3DXVECTOR3> vb;
    std::vector<uint16_t>    ib;

    ::CreateBoxMesh(vb, ib, 0, radius);
    m_numIndex = ib.size();

    D3D10_BUFFER_DESC bd;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    bd.Usage = D3D10_USAGE_IMMUTABLE;
    bd.ByteWidth = sizeof(vb[0])*vb.size();
    bd.BindFlags = D3D10_BIND_VERTEX_BUFFER;

    D3D10_SUBRESOURCE_DATA initData;
    initData.pSysMem = &vb[0];
    HR(m_device->CreateBuffer(&bd,&initData, &m_vb));

    bd.ByteWidth = sizeof(ib[0]) * ib.size();
    bd.BindFlags = D3D10_BIND_INDEX_BUFFER;

    initData.pSysMem = &ib[0];
    HR(m_device->CreateBuffer(&bd, &initData, &m_ib));

}

void LightBoxMesh::Draw(void)
{
    m_device->IASetInputLayout(InputLayout::Pos);
    UINT stride = sizeof(D3DXVECTOR3);
    UINT offset = 0;
    m_device->IASetVertexBuffers(0,1,&m_vb,&stride,&offset);
    m_device->IASetIndexBuffer(m_ib, DXGI_FORMAT_R16_UINT , 0);
    m_device->DrawIndexed(m_numIndex,0,0);
}