//=========================================================================
//	Module: r3dMeshTexDensityVisualizer.h
//	Copyright (C) 2012.
//=========================================================================

#pragma once
#ifndef FINAL_BUILD

//////////////////////////////////////////////////////////////////////////

#include "r3dObj.h"

//////////////////////////////////////////////////////////////////////////

class MeshTextureDensityVisualizer
{
public:
	explicit MeshTextureDensityVisualizer(r3dMesh *m);
	~MeshTextureDensityVisualizer();
	void SetTexelDensityStreamAndVDecl();
	void SetShaderConstants();
	void SetShaders();

private:
	void CreateNewVertexDeclarationForMesh();
	void CalculateTextureDensity();
	uint32_t modifiedVDeclIdx;
	
	r3dMesh *mesh;
	r3dD3DVertexBufferTunnel texelDensityData;
};

#endif // FINAL_BUILD
