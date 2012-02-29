#pragma once

#include "d3dUtil.h"

//光照几何体
//点光源为球形
//平行光用全屏四边形
//聚光灯用圆锥体




class LightGeometry
{
public:
    LightGeometry(ID3D10Device* device);
    ~LightGeometry(void);

    virtual void Draw(void) = 0;
protected:
    ID3D10Device * m_device;
    ID3D10Buffer*  m_vb;
    ID3D10Buffer*  m_ib;
    uint32_t       m_numIndex;
};

//for spot light volume convex mesh
class LightConeMesh : public LightGeometry
{
public:
    LightConeMesh(ID3D10Device * device, float radius, float height, uint16_t n);

    virtual void Draw();

};


//spoint light volume convex mesh
class LightBoxMesh : public LightGeometry
{
public:
    LightBoxMesh(ID3D10Device* device, float radius);

    virtual void Draw();
};

//directional light  using a full screen quad
class LightQuadMesh : public LightGeometry
{
public :
    LightQuadMesh(ID3D10Device* device);

    virtual void Draw();
};
