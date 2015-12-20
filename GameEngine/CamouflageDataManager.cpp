//=========================================================================
//	Module: CamouflageDataManager.cpp
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"
#include "r3dMat.h"
#include "CamouflageDataManager.h"
#include "gameobjects/PhysXWorld.h"
#include "TrueNature2/Terrain3.h"
#include "TrueNature2/Terrain2.h"

#include <limits>

//////////////////////////////////////////////////////////////////////////

CamouflageDataManager gCamouflageDataManager;

static const r3dMaterial::CamoData& CamouflageDataManager_GetCurrentData()
{
	return gCamouflageDataManager.GetCurrentData();
}

CamouflageDataManager::CamouflageDataManager()
{
	// set callback in evenity
	extern const r3dMaterial::CamoData& (*r3dMat_gCamouflageDataManager_GetCurrentData)();
	r3dMat_gCamouflageDataManager_GetCurrentData = &CamouflageDataManager_GetCurrentData;
}

//////////////////////////////////////////////////////////////////////////

void CamouflageDataManager::UpdateCamouflageData(const GameObject &o)
{
	const r3dVector &pos = o.GetPosition();
	PxVec3 orig(pos.x, pos.y + 0.5f, pos.z);

	static const PxVec3 dirs[5] =
	{
		PxVec3( 0, -1,  0),
		PxVec3( 1,  0,  0),
		PxVec3(-1,  0,  0),
		PxVec3( 0,  0, -1),
		PxVec3( 0,  0,  1)
	};

	PxRaycastHit hit;
	float closest = std::numeric_limits<float>::max();
	PhysicsCallbackObject* target = 0;
	uint32_t faceId = 0;
	r3dVector impactPos(0, 0, 0);

	for (int i = 0; i < _countof(dirs); ++i)
	{
		PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK,0,0,0), PxSceneQueryFilterFlags(PxSceneQueryFilterFlag::eDYNAMIC | PxSceneQueryFilterFlag::eSTATIC));
		if(g_pPhysicsWorld->raycastSingle(orig, dirs[i], 5.0f, PxSceneQueryFlags(PxSceneQueryFlag::eIMPACT), hit, filter))
		{
			r3dVector hitPos(hit.impact.x, hit.impact.y, hit.impact.z);
			float dist = (hitPos - pos).LengthSq();
			if (dist < closest)
			{
				r3d_assert(hit.shape);
				target = static_cast<PhysicsCallbackObject*>(hit.shape->getActor().userData);
				PxU32 faceIndex = hit.faceIndex;
				{
					PxTriangleMeshGeometry pxGeometry;
					if(hit.shape->getTriangleMeshGeometry(pxGeometry))
					{
						r3d_assert(pxGeometry.triangleMesh);
						const PxU32* remap = pxGeometry.triangleMesh->getTrianglesRemap();
						r3d_assert(remap);
						PxU32 numTriangles = pxGeometry.triangleMesh->getNbTriangles();
						r3d_assert(faceIndex < numTriangles);
						faceIndex = remap[faceIndex];
					}
				}

				faceId = faceIndex;
				closest = dist;
				impactPos = hitPos;
			}
		}
	}

	r3dTexture * newCamoTex = 0;
	if (target)
	{
		r3dMaterial *material = 0;
		GameObject *gameObj = target->isGameObject();

		if (gameObj)
		{
			if (gameObj->isObjType(OBJTYPE_Mesh))
			{
				material = static_cast<MeshGameObject*>(target)->MeshLOD[0]->GetFaceMaterial(faceId);
			}
			if (gameObj->isObjType(OBJTYPE_Terrain))
			{
				newCamoTex = Terrain->GetDominantTexture( impactPos ) ;
			}
		}
		else if (target->GetMaterial(faceId))
		{
			material = target->GetMaterial(faceId);
		}
		else if (target->hasMesh())
		{
			material = target->hasMesh()->GetFaceMaterial(faceId);
		}

		if (material)
		{
			newCamoTex = material->Texture ? material->Texture->GetFullSizeTexture() : NULL;
		}

		r3dMaterial::CamoData cd = GetCamouflageData(o);
		float currT = r3dGetTime();
		//	This is for abrupt texture changes
		float camoPhase = currT - cd.transitionStartTime;
		if (camoPhase > CAMOUFLAGE_LERP_DURATION)
		{
			cd.prevTex = cd.curTex;
			cd.curTex = newCamoTex;

			cd.transitionStartTime = currT;
			dataStorage.AddAndReplace(o.GetHashID(), cd);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

r3dMaterial::CamoData CamouflageDataManager::GetCamouflageData(const GameObject &o)
{
	r3dMaterial::CamoData r = dataStorage.GetObject(o.GetHashID(), r3dMaterial::CamoData());
	return r;
}

//////////////////////////////////////////////////////////////////////////

void CamouflageDataManager::SetCurrentData(const GameObject &o)
{
	currData = dataStorage.GetObject(o.GetHashID(), r3dMaterial::CamoData());
}

//-------------------------------------------------------------------------
//	class CamoTimeLine
//-------------------------------------------------------------------------

CamoTimeLine::CamoTimeLine()
: transparentCamoAlpha(1.0f)
, transparentCamoAlphaTarget(1.0f)
, camoMasterTransparency(0)
, lastCamoTime(0)
, fadeTime(0.25f)
, transTime(1.0f)
, currentTimeSpot(1.0f)
, transparencyOnShock(0)
{}

//////////////////////////////////////////////////////////////////////////

void CamoTimeLine::Update()
{
	if (lastCamoTime == 0)
		lastCamoTime = r3dGetTime();
	float currTime = r3dGetTime();
	float dt = currTime - lastCamoTime;
	lastCamoTime = currTime;

	transparencyOnShock -= dt ;

	if( transparencyOnShock < 0 )
		transparencyOnShock = 0 ;

	float s = transparentCamoAlphaTarget - currentTimeSpot;
	// Sign
	float sign = s > 0 ? 1.0f : s < 0 ? -1.0f : 0;

	float totalTime = fadeTime + transTime;
	float dist = R3D_MIN(dt / totalTime, abs(s));

	currentTimeSpot += dist * sign;

	//	Now calc master transparency and camo transparency
	if (currentTimeSpot * totalTime > transTime)
	{
		transparentCamoAlpha = 0.99f;
		camoMasterTransparency = (currentTimeSpot * totalTime - transTime) / fadeTime;
	}
	else
	{
		camoMasterTransparency = 0;
		transparentCamoAlpha = currentTimeSpot * totalTime / transTime;
	}
}

//////////////////////////////////////////////////////////////////////////

void CamoTimeLine::SetNewCamoTarget(float t)
{
	if (t == 1)
	{
		transparentCamoAlphaTarget = t;
		return;
	}

	transparentCamoAlphaTarget = transTime * t;
	transparentCamoAlphaTarget /= fadeTime + transTime;
}
