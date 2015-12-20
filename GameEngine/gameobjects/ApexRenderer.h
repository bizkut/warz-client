//=========================================================================
//	Module: ApexRenderer.h
//	Created by Roman E. Marchenko <roman.marchenko@gmail.com>
//	Copyright (C) 2011.
//=========================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////
#if APEX_ENABLED

#include "NxApex.h"
#include "NxUserRenderResourceManager.h"

//////////////////////////////////////////////////////////////////////////

using namespace physx;
using namespace physx::apex;

//////////////////////////////////////////////////////////////////////////

class UserRenderResourceManager: public physx::apex::NxUserRenderResourceManager
{
	virtual NxUserRenderVertexBuffer*   createVertexBuffer(const NxUserRenderVertexBufferDesc& desc);
	virtual void                        releaseVertexBuffer(NxUserRenderVertexBuffer& buffer);

	virtual NxUserRenderIndexBuffer*    createIndexBuffer(const NxUserRenderIndexBufferDesc& desc);
	virtual void                        releaseIndexBuffer(NxUserRenderIndexBuffer& buffer);

	virtual NxUserRenderBoneBuffer*     createBoneBuffer(const NxUserRenderBoneBufferDesc& desc);
	virtual void                        releaseBoneBuffer(NxUserRenderBoneBuffer& buffer);

	virtual NxUserRenderInstanceBuffer* createInstanceBuffer(const NxUserRenderInstanceBufferDesc& desc);
	virtual void                        releaseInstanceBuffer(NxUserRenderInstanceBuffer& buffer);

	virtual NxUserRenderSpriteBuffer*   createSpriteBuffer(const NxUserRenderSpriteBufferDesc& desc);
	virtual void                        releaseSpriteBuffer(NxUserRenderSpriteBuffer& buffer);

	virtual NxUserRenderResource*       createResource(const NxUserRenderResourceDesc& desc);

	/**
	releaseResource() should not release any of the included buffer pointers.  Those free methods will be
	called separately by the APEX SDK before (or sometimes after) releasing the NxUserRenderResource.
	*/
	virtual void                        releaseResource(NxUserRenderResource& resource);

	/**
	Get the maximum number of bones supported by a given material. Return 0 for infinite.
	For optimal rendering, do not limit the bone count (return 0 from this function).
	*/
	virtual physx::PxU32                getMaxBonesForMaterial(void* material);
};

//////////////////////////////////////////////////////////////////////////

class UserRenderer: public NxUserRenderer
{
	typedef r3dgfxMap(uint32_t, IDirect3DVertexDeclaration9*) DeclsMap;
	DeclsMap declsMap;

	/**	Render to shadow map mode. */
	bool shadowRenderMode;

	IDirect3DVertexDeclaration9 * CreateVertexDeclaration(const NxApexRenderContext& context, D3DVERTEXELEMENT9 *ve) const;
	uint32_t HashVertexDescription(D3DVERTEXELEMENT9 *ve, size_t count) const;
	bool CreateFullVertexDescription(const NxApexRenderContext& context, D3DVERTEXELEMENT9 *ve, size_t count) const;

public:
	UserRenderer();
	~UserRenderer();
	void Destroy();
	void SetShadowRenderMode(bool on) { shadowRenderMode = on; }
	virtual void renderResource(const NxApexRenderContext& context);
};

//////////////////////////////////////////////////////////////////////////

void DestroyApexUserRenderer();

#endif