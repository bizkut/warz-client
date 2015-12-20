//=========================================================================
//	Module: ApexActor.cpp
//	Created by Roman E. Marchenko <roman.marchenko@gmail.com>
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#if APEX_ENABLED

#include "ApexActor.h"
#include "ApexCommon.h"
#include "ApexWorld.h"
#include "ApexRenderer.h"
#include "PhysObj.h"

//////////////////////////////////////////////////////////////////////////

using namespace physx;
using namespace physx::apex;

extern UserRenderer apexUserRenderer;
extern bool g_bEditMode;
extern r3dCamera gCam;

//////////////////////////////////////////////////////////////////////////

D3DXMATRIX CalcPose(const r3dPoint3D &pos, const r3dPoint3D &rotVec)
{
	D3DXMATRIX r, t, pose;
	D3DXMatrixRotationYawPitchRoll(&r, R3D_DEG2RAD(rotVec.Z), R3D_DEG2RAD(rotVec.Y), R3D_DEG2RAD(rotVec.X));
	D3DXMatrixTranslation(&t, pos.x, pos.y, pos.z);
	D3DXMatrixMultiply(&pose, &r, &t);
	return pose;
}

//////////////////////////////////////////////////////////////////////////

void ApexActorParameters::SetPose(const r3dPoint3D &pos, const r3dPoint3D &rotVec)
{
	pose = CalcPose(pos, rotVec);
}

//////////////////////////////////////////////////////////////////////////

ApexActorBase * CreateApexActor(const ApexActorParameters &params, bool makePreview)
{
	//	If not in editor and quality lower than ultra, dont create apex objects
	if (!g_bEditMode && !r_apex_enabled->GetBool())
		return 0;

	if (makePreview)
		return game_new Private::ApexPreviewActor(params);
	else
		return game_new Private::ApexActor(params);
}

//-------------------------------------------------------------------------
//	ApexActorBase impl
//-------------------------------------------------------------------------

ApexActorBase::ApexActorBase(const ApexActorParameters &ap)
: params(ap)
, rewriteBuffers(false)
, numMaterials(0)
{
	r3d_assert(g_pApexWorld);
	if (!g_pApexWorld)
		return;

	ZeroMemory(materials, sizeof(r3dMaterial*) * APEX_ACTOR_MAX_MATERIALS);

	//	Register self in ApexWorld
	g_pApexWorld->actors.PushBack(this);
}

//////////////////////////////////////////////////////////////////////////

ApexActorBase::~ApexActorBase()
{
	r3d_assert(g_pApexWorld);
	if (!g_pApexWorld)
		return;

	g_pApexWorld->actors.Erase(g_pApexWorld->actors.FindItemIndex(this));
}

//////////////////////////////////////////////////////////////////////////

void ApexActorBase::Update()
{
	NxApexRenderable *r = GetRenderable();
	if (!r) return;
	r->lockRenderResources();
	r->updateRenderResources(rewriteBuffers, this);
	rewriteBuffers = false;
	r->unlockRenderResources();
}

//////////////////////////////////////////////////////////////////////////

void ApexActorBase::Draw()
{
	NxApexRenderable *r = GetRenderable();
	if (!r) return;
	r->dispatchRenderResources(apexUserRenderer);
}

//////////////////////////////////////////////////////////////////////////

void ApexActorBase::DrawShadow()
{
	NxApexRenderable *r = GetRenderable();
	if (!r) return;
	apexUserRenderer.SetShadowRenderMode(true);
	r->dispatchRenderResources(apexUserRenderer);
	apexUserRenderer.SetShadowRenderMode(false);
}

//////////////////////////////////////////////////////////////////////////

r3dBoundBox ApexActorBase::GetBBox() const
{
	r3dBoundBox rv;
	NxApexRenderable *r = GetRenderable();
	if (!r) return rv;

	PxBounds3 bb = r->getBounds();
	
	rv.Org = r3dPoint3D(bb.minimum.x, bb.minimum.y, bb.minimum.z);
	PxVec3 dims = bb.getDimensions();
	rv.Size = r3dPoint3D(dims.x, dims.y, dims.z);
	return rv;
}

//////////////////////////////////////////////////////////////////////////

void ApexActorBase::SetPosition(const r3dVector &pos)
{
	//	No position setting
}

//////////////////////////////////////////////////////////////////////////

void ApexActorBase::SetRotation(const r3dVector &angles)
{

}

//////////////////////////////////////////////////////////////////////////

void ApexActorBase::AddMaterial(r3dMaterial *m)
{
	if (!m) return;
	
	//	Search for existing material
	r3dMaterial **start = materials;
	r3dMaterial **end = materials + numMaterials;
	r3dMaterial **f = std::find(start, end, m);

	//	Not found, add one (if free space available)
	if (f == end)
	{
		if (numMaterials == APEX_ACTOR_MAX_MATERIALS - 1)
		{
			r3d_assert(!"There are too many different materials for apex object. Please increase APEX_ACTOR_MAX_MATERIALS value");
			return;
		}
		materials[numMaterials++] = m;
	}
}

//////////////////////////////////////////////////////////////////////////

r3dMaterial * ApexActorBase::GetMaterial(uint32_t /*faceID*/)
{
	// No matter how hard I try to find a way to get material from NxRaycastHit structure, I cannot do this.
	// So we return simply first material. It is used for bullet hits particles spawn, chameleon camo, an such things, so I think this is not a big problem.
	// If needs arises, this problem should be investigated more.
	return materials[0];
}

//////////////////////////////////////////////////////////////////////////

namespace Private
{

//-------------------------------------------------------------------------
//	ApexActor impl
//-------------------------------------------------------------------------

ApexActor::ApexActor(const ApexActorParameters &ap)
: ApexActorBase(ap)
, actor(CreateActor())
{
}

//////////////////////////////////////////////////////////////////////////

NxDestructibleActor * ApexActor::CreateActor()
{
	NxResourceProvider *nrp = g_pApexWorld->apexSDK->getNamedResourceProvider();
	NxApexAsset *asset = reinterpret_cast<physx::apex::NxApexAsset*>(nrp->getResource("ApexObject", params.assetPath.c_str()));
	NxDestructibleAsset * destrAsset = static_cast<NxDestructibleAsset*>(asset);

	NxParameterized::Interface * descParams = asset->getDefaultActorDesc();
	NxParameterized::Handle globalPose(descParams);

	PxMat44 m(const_cast<float*>(&params.pose._11));
	descParams->getParameterHandle("globalPose", globalPose);
	globalPose.setParamMat44(m);

	NxParameterized::Handle dynamic(descParams);
	descParams->getParameterHandle("dynamic", dynamic);
	dynamic.setParamBool(false);
	
	NxParameterized::Handle p(descParams);
	descParams->getParameterHandle("shapeDescTemplate.collisionGroup", p);
	p.setParamU16(PHYSCOLL_COLLISION_GEOMETRY);

	descParams->getParameterHandle("actorDescTemplate.userData", p);
	p.setParamU64(reinterpret_cast<PxU64>(params.userData));

	NxDestructibleActor *rv = static_cast<NxDestructibleActor*>(destrAsset->createApexActor(*descParams, *g_pApexWorld->apexScene));
	if (rv) rv->userData = params.userData;

	return rv;
}

//////////////////////////////////////////////////////////////////////////

NxApexRenderable * ApexActor::GetRenderable() const
{
	return actor;
}

//////////////////////////////////////////////////////////////////////////

ApexActor::~ApexActor()
{
	if (actor)
		actor->release();
}

//////////////////////////////////////////////////////////////////////////

ApexActorBase * ApexActor::ConvertToOpposite()
{
	if (!actor) return 0;
	PxMat44 m = actor->getInitialGlobalPose();
	return game_new ApexPreviewActor(params);
}

//////////////////////////////////////////////////////////////////////////

ApexActorBase * ApexActor::Clone() const
{
	if (!actor) return 0;
	return game_new ApexActor(params);
}

//////////////////////////////////////////////////////////////////////////

void ApexActor::ApplyAreaDamage(float damage, float momentum, const r3dPoint3D& position, float radius, bool falloff)
{
	if (!actor)
		return;

	//	Construct bounding box from explosion position and radius
	r3dBoundBox explBB;
	explBB.Org = position - r3dPoint3D(radius / 2, radius / 2, radius / 2);
	explBB.Size = r3dPoint3D(radius * 2, radius * 2, radius * 2);

	r3dBoundBox bb = GetBBox();
	if (!bb.Intersect(explBB))
		return;

	actor->applyRadiusDamage(damage, momentum, PxVec3(position.x, position.y, position.z), radius, falloff);
}

//////////////////////////////////////////////////////////////////////////

void ApexActor::ApplyDamage(float damage, float momentum, const r3dPoint3D &pos, const r3dPoint3D &direction)
{
	if (!actor)
		return;

	actor->applyDamage(damage, momentum, PxVec3(pos.x, pos.y, pos.z), PxVec3(direction.x, direction.y, direction.z));
}

//-------------------------------------------------------------------------
//	ApexPreviewActor
//-------------------------------------------------------------------------

ApexPreviewActor::ApexPreviewActor(const ApexActorParameters &ap)
: ApexActorBase(ap)
, prv(CreatePreviewActor())
{
}

//////////////////////////////////////////////////////////////////////////

NxApexAssetPreview * ApexPreviewActor::CreatePreviewActor()
{
	NxResourceProvider *nrp = g_pApexWorld->apexSDK->getNamedResourceProvider();
	NxApexAsset *asset = reinterpret_cast<physx::apex::NxApexAsset*>(nrp->getResource("ApexObject", params.assetPath.c_str()));
	NxDestructibleAsset * destrAsset = static_cast<NxDestructibleAsset*>(asset);

	if (!asset) return 0;
	NxParameterized::Interface *previewParams = asset->getDefaultAssetPreviewDesc();
	NxParameterized::Handle globalPose(previewParams);
	previewParams->getParameterHandle("globalPose", globalPose);

	PxMat44 m(const_cast<float*>(&params.pose._11));
	globalPose.setParamMat44(m);

	NxApexAssetPreview *rv = destrAsset->createApexAssetPreview(*previewParams,g_pApexWorld->apexSDK->createAssetPreviewScene());
	return rv;
}

//////////////////////////////////////////////////////////////////////////

NxApexRenderable * ApexPreviewActor::GetRenderable() const
{
	return prv;
}

//////////////////////////////////////////////////////////////////////////

ApexPreviewActor::~ApexPreviewActor()
{
	if (prv)
		prv->release();
}

//////////////////////////////////////////////////////////////////////////

void ApexPreviewActor::SetPosition(const r3dVector &pos)
{
	if (!prv) return;
	PxMat44 m = prv->getPose();
	m.setPosition(PxVec3(pos.x, pos.y, pos.z));
	prv->setPose(m);
}

//////////////////////////////////////////////////////////////////////////

void ApexPreviewActor::SetRotation(const r3dVector &angles)
{
	ApexActorBase::SetRotation(angles);

	if (!prv) return;
	PxMat44 m = prv->getPose();
	PxVec3 pos = m.getPosition();
	D3DXMATRIX p = CalcPose(r3dPoint3D(pos.x, pos.y, pos.z), angles);
	m = PxMat44(&p._11);

	prv->setPose(m);
}

//////////////////////////////////////////////////////////////////////////

ApexActorBase * ApexPreviewActor::ConvertToOpposite()
{
	if (!prv) return 0;
	PxMat44 m = prv->getPose();
	ApexActorParameters ap(params);
	ap.pose = D3DXMATRIX(&m.column0.x);
	return game_new ApexActor(ap);
}

//////////////////////////////////////////////////////////////////////////

ApexActorBase * ApexPreviewActor::Clone() const
{
	if (!prv) return 0;
	return game_new ApexPreviewActor(params);
}

} // namespace Private

#endif