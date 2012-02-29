#include "LightSource.h"
#include "Effects.h"
#include "InputLayouts.h"


LightVolume::LightVolume(ID3D10Device *device) :mDevice(device), mIB(NULL)
    ,mVB(NULL)
    ,mIndexCount(0)
{
}


LightVolume::~LightVolume(void)
{
    ReleaseCOM(mIB);
    ReleaseCOM(mVB);
}

void LightVolume::Render(void)
{
}

void LightVolume::Draw(void)
{
    mDevice->IASetInputLayout(InputLayout::Pos);
    UINT stride = sizeof(D3DXVECTOR3);
    UINT offset = 0;
    mDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
    mDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R16_UINT, 0);
    mDevice->DrawIndexed(this->mIndexCount, 0, 0);
}