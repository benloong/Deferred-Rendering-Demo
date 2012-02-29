#pragma once
#include "d3dutil.h"

//normalized coordinate quad
class NDCQuad
{
public:
    NDCQuad(ID3D10Device* device, D3DXVECTOR3 const & pos, D3DXVECTOR3 const & scale);
    ~NDCQuad(void);

    void Render(void);
    void draw(void);
    
private:
    void Scale(float x, float y);
    void Translate(float x, float y);
    void Translate(D3DXVECTOR3 v);

private:

    ID3D10Device * m_device;
    ID3D10Buffer * m_vertexBuffer;
};

