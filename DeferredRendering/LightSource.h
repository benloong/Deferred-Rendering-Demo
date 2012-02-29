#pragma once
#include "d3dutil.h"

//spot light cone  
//radius: spot light radius
//height: spot light distance
//n : how many circle slice
template <typename T>
void CreateConeMesh(std::vector<T>& vb, std::vector<uint16_t>& ib, uint16_t vertex_base, float radius, float height, uint16_t n)
{
	for (int i = 0; i < n; ++ i)
	{
		vb.push_back(T());
		vb.back().x = vb.back().y = vb.back().z = 0;
	}

	float outer_radius = radius / cos(PI / n);
	for (int i = 0; i < n; ++ i)
	{
		vb.push_back(T());
		float angle = i * 2 * PI / n;
		vb.back().x = outer_radius * cos(angle);
		vb.back().y = outer_radius * sin(angle);
		vb.back().z = height;
	}

	vb.push_back(T());
	vb.back().x = vb.back().y = 0;
	vb.back().z = height;

	for (int i = 0; i < n; ++ i)
	{
		vb.push_back(T());
		vb.back() = vb[vertex_base + n + i];
	}

	for (uint16_t i = 0; i < n - 1; ++ i)
	{
		ib.push_back(vertex_base + i);
		ib.push_back(vertex_base + n + i + 1);
		ib.push_back(vertex_base + n + i);
	}
	ib.push_back(vertex_base + n - 1);
	ib.push_back(vertex_base + n + 0);
	ib.push_back(vertex_base + n + n - 1);

	for (uint16_t i = 0; i < n - 1; ++ i)
	{
		ib.push_back(vertex_base + 2 * n);
		ib.push_back(vertex_base + 2 * n + 1 + i);
		ib.push_back(vertex_base + 2 * n + 1 + i + 1);
	}
	ib.push_back(vertex_base + 2 * n);
	ib.push_back(vertex_base + 2 * n + 1 + n - 1);
	ib.push_back(vertex_base + 2 * n + 1);
}


//for point light volume 
//half_length : point light range
template <typename T>
void CreateBoxMesh(std::vector<T>& vb, std::vector<uint16_t>& ib, uint16_t vertex_base, float half_length)
{
	vb.push_back(T());
	vb.back().x = -half_length;
	vb.back().y = +half_length;
	vb.back().z = -half_length;
	vb.push_back(T());
	vb.back().x = +half_length;
	vb.back().y = +half_length;
	vb.back().z = -half_length;
	vb.push_back(T());
	vb.back().x = +half_length;
	vb.back().y = -half_length;
	vb.back().z = -half_length;
	vb.push_back(T());
	vb.back().x = -half_length;
	vb.back().y = -half_length;
	vb.back().z = -half_length;

	vb.push_back(T());
	vb.back().x = -half_length;
	vb.back().y = +half_length;
	vb.back().z = +half_length;
	vb.push_back(T());
	vb.back().x = +half_length;
	vb.back().y = +half_length;
	vb.back().z = +half_length;
	vb.push_back(T());
	vb.back().x = +half_length;
	vb.back().y = -half_length;
	vb.back().z = +half_length;
	vb.push_back(T());
	vb.back().x = -half_length;
	vb.back().y = -half_length;
	vb.back().z = +half_length;

	ib.push_back(vertex_base + 0);
	ib.push_back(vertex_base + 1);
	ib.push_back(vertex_base + 2);
	ib.push_back(vertex_base + 2);
	ib.push_back(vertex_base + 3);
	ib.push_back(vertex_base + 0);

	ib.push_back(vertex_base + 5);
	ib.push_back(vertex_base + 4);
	ib.push_back(vertex_base + 7);
	ib.push_back(vertex_base + 7);
	ib.push_back(vertex_base + 6);
	ib.push_back(vertex_base + 5);

	ib.push_back(vertex_base + 4);
	ib.push_back(vertex_base + 5);
	ib.push_back(vertex_base + 1);
	ib.push_back(vertex_base + 1);
	ib.push_back(vertex_base + 0);
	ib.push_back(vertex_base + 4);

	ib.push_back(vertex_base + 1);
	ib.push_back(vertex_base + 5);
	ib.push_back(vertex_base + 6);
	ib.push_back(vertex_base + 6);
	ib.push_back(vertex_base + 2);
	ib.push_back(vertex_base + 1);

	ib.push_back(vertex_base + 3);
	ib.push_back(vertex_base + 2);
	ib.push_back(vertex_base + 6);
	ib.push_back(vertex_base + 6);
	ib.push_back(vertex_base + 7);
	ib.push_back(vertex_base + 3);

	ib.push_back(vertex_base + 4);
	ib.push_back(vertex_base + 0);
	ib.push_back(vertex_base + 3);
	ib.push_back(vertex_base + 3);
	ib.push_back(vertex_base + 7);
	ib.push_back(vertex_base + 4);
}

template <typename T>
void CreateQuadMesh(std::vector<T> &vb, std::vector<uint16_t> &ib, uint16_t vertex_base )
{
}

class LightVolume
{
public:
    LightVolume(ID3D10Device * device);
    ~LightVolume(void);

    virtual void Render(void);
protected:
    void Draw(void);
protected:
    ID3D10Device           *mDevice;
    ID3D10EffectTechnique  *mTech;
    ID3D10Buffer           *mVB;
    ID3D10Buffer           *mIB;
    uint32_t                mIndexCount;
};

class AmbientLightSource : public LightVolume
{
public:
    AmbientLightSource(ID3D10Device *device, D3DXCOLOR const &color);
    ~AmbientLightSource(void);
private:
    D3DXCOLOR     mColor;
    ID3D10EffectVariable    *mColorVar;
};

class PointLightSource : public LightVolume
{
public:
    PointLightSource(ID3D10Device *device, D3DXVECTOR3 const &color, D3DXVECTOR3 const &position, D3DXVECTOR3 const &falloff);
    ~PointLightSource(void);

private:
    D3DXCOLOR       mColor;
    D3DXVECTOR3     mPosition;
    D3DXVECTOR3     mFalloff;

    ID3D10EffectVariable   *mColorVar;
    ID3D10EffectVariable   *mPositionESVar;  //eye space position parameter
    ID3D10EffectVariable   *mFalloffVar; 
};