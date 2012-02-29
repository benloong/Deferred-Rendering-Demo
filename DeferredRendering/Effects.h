#ifndef EFFECTS_H
#define EFFECTS_H

#include "d3dUtil.h"

namespace fx
{
	extern ID3D10Effect* SkyFX;
	extern ID3D10Effect* ShadowFX;
	extern ID3D10Effect* BuildShadowMapFX;
	extern ID3D10Effect* DrawShadowMapFX;
    extern ID3D10Effect* GenGBufferFX;
    extern ID3D10Effect* LightingFX;
    extern ID3D10Effect* ShadingFX;
	void InitAll(ID3D10Device* device);
	void DestroyAll();
};

#endif // EFFECTS_H