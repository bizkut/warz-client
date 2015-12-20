//=========================================================================
//	Module: GrassEditorPlanes.cpp
//	Copyright (C) 2012.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "GrassEditorPlanes.h"
#include "GrassMap.h"

#include "..\..\Editors\ObjectManipulator3d.h"
#include "..\..\..\..\GameEngine\TrueNature2\Terrain3.h"

//////////////////////////////////////////////////////////////////////////

extern r3dITerrain* Terrain;
extern r3dCamera gCam;
extern GrassMap* g_pGrassMap;
GrassPlanesManager * g_GrassPlanesManager = 0;
float g_NoTerrainLevelBaseHeight = 0.0f;

#ifndef FINAL_BUILD

//////////////////////////////////////////////////////////////////////////

namespace
{
	const char* GrazPlanesDataFileName = "grassplanes.bin";
	char GrazPlanesData_SIG100[] = "GRAZPLANEZ100";

} // unnamed namespace

//////////////////////////////////////////////////////////////////////////

GrassPlanesManager * GetGrassPlaneManager()
{
	if (!g_GrassPlanesManager)
	{
		g_GrassPlanesManager = game_new GrassPlanesManager;
	}
	return g_GrassPlanesManager;
}

//////////////////////////////////////////////////////////////////////////

void FreeGrassPlaneManager()
{
	delete g_GrassPlanesManager;
	g_GrassPlanesManager = NULL;
}

//-------------------------------------------------------------------------
//	GrassPlane
//-------------------------------------------------------------------------

GrassPlane::GrassPlane()
: name("Grass plane")
{
	bounds.Org = r3dPoint3D(0, 3.0f, 0);
	bounds.Size = r3dPoint3D(10.0f, 0.0f, 10.0f);
}

//////////////////////////////////////////////////////////////////////////

float GrassPlane::GetHeight(float x, float z) const
{
	// First check if requested coordinates lies on plane rect x,z projection
	if (IsPointInside(x, z))
		return bounds.Org.y;
	return g_NoTerrainLevelBaseHeight;
}

//////////////////////////////////////////////////////////////////////////

void GrassPlane::SetOrigin(const r3dPoint3D &pos, bool updateGrassMap)
{
	bounds.Org = pos;
	if (updateGrassMap)
		UpdateGrassMap();
}

//////////////////////////////////////////////////////////////////////////

void GrassPlane::SetSize(float x, float z, bool updateGrassMap)
{
	bounds.Size.x = x;
	bounds.Size.y = 0;
	bounds.Size.z = z;

	if (updateGrassMap)
		UpdateGrassMap();
}

//////////////////////////////////////////////////////////////////////////

void GrassPlane::Draw() const
{
	DWORD oldStencil = 0;
	r3dRenderer->pd3ddev->GetRenderState(D3DRS_SCISSORTESTENABLE, &oldStencil);
	r3dRenderer->pd3ddev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
	int oldRenderMode = r3dRenderer->GetRenderingMode();
	r3dRenderer->SetRenderingMode(R3D_BLEND_ZC);
	//	Prepare 4 corner points
	r3dPoint3D p[4] = 
	{
		r3dPoint3D(bounds.Org),
		r3dPoint3D(bounds.Org + r3dPoint3D(bounds.Size.x, 0, 0)),
		r3dPoint3D(bounds.Org + r3dPoint3D(0, 0, bounds.Size.z)),
		r3dPoint3D(bounds.Org + r3dPoint3D(bounds.Size.x, 0, bounds.Size.z))
	};

	//	Draw outline lines
	r3dColor cl = r3dColor::green;
	float width = 0.03f;
	r3dDrawLine3D(p[0], p[1], gCam, width, cl);
	r3dDrawLine3D(p[1], p[3], gCam, width, cl);
	r3dDrawLine3D(p[3], p[2], gCam, width, cl);
	r3dDrawLine3D(p[2], p[0], gCam, width, cl);

	cl.A = 90;

	//	Draw filled triangles
	r3dDrawTriangle3D(p[2], p[1], p[0], gCam, cl);
	r3dDrawTriangle3D(p[2], p[3], p[1], gCam, cl);
 	r3dDrawTriangle3D(p[0], p[1], p[2], gCam, cl);
 	r3dDrawTriangle3D(p[1], p[3], p[2], gCam, cl);
	r3dRenderer->Flush();

	r3dRenderer->pd3ddev->SetRenderState(D3DRS_SCISSORTESTENABLE, oldStencil);
	r3dRenderer->SetRenderingMode(oldRenderMode);
}

//////////////////////////////////////////////////////////////////////////

void GrassPlane::UpdateGrassMap()
{
	if (!Terrain)
	{
		float gridW, gridH;
		g_pGrassMap->GetGridWorldSize(gridW, gridH);
		float maxX = std::max(gridW, std::max(bounds.Org.x, bounds.Size.x + bounds.Org.x));
		float maxZ = std::max(gridH, std::max(bounds.Org.z, bounds.Size.z + bounds.Org.z));

#if 0
		if (maxX > gridW || maxZ > gridH)
		{
			g_pGrassMap->Close();
			g_pGrassMap->Init(maxX, maxZ, GrassMap::MAX_TEX_CELL_COUNT);
			g_pGrassMap->ConformWithNewCellSize();
		}
#endif
	}

	g_pGrassMap->UpdateHeight();
}

//////////////////////////////////////////////////////////////////////////

bool GrassPlane::IsPointInside(float x, float z) const
{
	r3dPoint3D s(bounds.Org);
	r3dPoint3D e(bounds.Org + bounds.Size);

	float minX = std::min(s.x, e.x);
	float maxX = std::max(s.x, e.x);
	float minZ = std::min(s.z, e.z);
	float maxZ = std::max(s.z, e.z);

	return !(x < minX || x > maxX || z < minZ || z > maxZ);
}
//-------------------------------------------------------------------------
//	GrassPlanesManager
//-------------------------------------------------------------------------

GrassPlanesManager::GrassPlanesManager()
{

}

//////////////////////////////////////////////////////////////////////////

GrassPlanesManager::~GrassPlanesManager()
{

}

//////////////////////////////////////////////////////////////////////////

int GrassPlanesManager::GetPlaneIndex(const r3dString &name) const
{
	for (uint32_t i = 0; i < planes.Count(); ++i)
	{
		const GrassPlane & p = planes[i];
		if (p.name == name)
			return static_cast<int>(i);
	}
	return -1;
}

//////////////////////////////////////////////////////////////////////////

GrassPlane & GrassPlanesManager::GetPlane(int idx)
{
	static GrassPlane p;
	if (static_cast<uint32_t>(idx) >= planes.Count() || idx < 0)
		return p;

	return planes[idx];
}

//////////////////////////////////////////////////////////////////////////

GrassPlane & GrassPlanesManager::GetPlane(const r3dString &name)
{
	int idx = GetPlaneIndex(name);
	return GetPlane(idx);
}

//////////////////////////////////////////////////////////////////////////

void GrassPlanesManager::DrawPlanes() const
{
	for (uint32_t i = 0; i < planes.Count(); ++i)
	{
		const GrassPlane &pln = planes[i];
		pln.Draw();
	}
}

//------------------------------------------------------------------------

void GrassPlanesManager::GetCombinedPlaneBounds( r3dBoundBox* oBox )
{
	if( !planes.Count() )
		return;

	r3dPoint3D start;
	r3dPoint3D end;

	start.x = FLT_MAX;
	start.y = FLT_MAX;
	start.z = FLT_MAX;

	end.x = -FLT_MAX;
	end.y = -FLT_MAX;
	end.z = -FLT_MAX;

	for( int i = 0, e = planes.Count(); i < e; i ++ )
	{
		r3dPoint3D istart, iend;

		istart = planes[ i ].GetBounds().Org;
		iend = planes[ i ].GetBounds().Org + planes[ i ].GetBounds().Size;

		start.x = R3D_MIN( start.x, istart.x );
		start.x = R3D_MIN( start.x, iend.x );

		start.y = R3D_MIN( start.y, istart.y );
		start.y = R3D_MIN( start.y, iend.y );

		start.z = R3D_MIN( start.z, istart.z );
		start.z = R3D_MIN( start.z, iend.z );

		end.x = R3D_MAX( end.x, istart.x );
		end.x = R3D_MAX( end.x, iend.x );

		end.y = R3D_MAX( end.y, istart.y );
		end.y = R3D_MAX( end.y, iend.y );

		end.z = R3D_MAX( end.z, istart.z );
		end.z = R3D_MAX( end.z, iend.z );
	}

	oBox->Org = start;
	oBox->Size = end - start;
}

//////////////////////////////////////////////////////////////////////////

GrassPlane & GrassPlanesManager::AddPlane()
{
	planes.Resize(planes.Count() + 1);
	return planes.GetLast();
}

//////////////////////////////////////////////////////////////////////////

float GrassPlanesManager::GetHeight(float x, float z) const
{
	float rv = g_NoTerrainLevelBaseHeight;
	for (uint32_t i = 0; i < planes.Count(); ++i)
	{
		rv = std::max(planes[i].GetHeight(x, z), rv);
	}
	
	return rv;
}

//////////////////////////////////////////////////////////////////////////

bool GrassPlanesManager::Save(const r3dString& levelHomeDir)
{
	using namespace r3dTL;

	r3dString grazPaz = levelHomeDir + AR_GRAZ_PAZ;

	// try creating just in case
	mkdir( grazPaz.c_str() );

	grazPaz += "\\";

	grazPaz += GrazPlanesDataFileName;

	FILE* fout = fopen( grazPaz.c_str(), "wb" );

	if( !fout )
	{
		r3dOutToLog( "GrassPlanesManager::Save: Couldn't open grass cells file %s for writing!", grazPaz.c_str() );
		return false;
	}

	struct AutoClose
	{
		~AutoClose()
		{
			fclose( file );
		}

		FILE *file;
	} autoClose = { fout }; (void)autoClose;

	fwrite( GrazPlanesData_SIG100, sizeof GrazPlanesData_SIG100, 1, fout );

	fwrite_be(g_NoTerrainLevelBaseHeight, fout);
	fwrite_be(GetPlanesCount(), fout);
	for (uint32_t i = 0; i < GetPlanesCount(); ++i)
	{
		GrassPlane &p = GetPlane(i);
		fwrite( p.name.c_str(), p.name.Length() + 1, 1, fout );
		fwrite_be(p.GetBounds().Org, fout);
		fwrite_be(p.GetBounds().Size, fout);
	}
	
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool ReadString( r3dString& oString, r3dFile* fin );

//////////////////////////////////////////////////////////////////////////

bool GrassPlanesManager::Load(const r3dString& levelHomeDir)
{
	using namespace r3dTL;

	r3dString grazPaz = levelHomeDir + AR_GRAZ_PAZ;

	grazPaz += "\\";

	grazPaz += GrazPlanesDataFileName;

	r3dFile* fin = r3d_open( grazPaz.c_str(), "rb" );

	if( !fin )
	{
		return false;
	}

	struct AutoClose
	{
		~AutoClose()
		{
			fclose( file );
		}

		r3dFile *file;
	} autoClose = { fin }; (void)autoClose;

	char compSig[ sizeof GrazPlanesData_SIG100 ];

	if( fread( compSig, sizeof compSig, 1, fin ) != 1 )
		return false;

	if( strcmp( GrazPlanesData_SIG100, compSig ) != 0 )
	{
		r3d_assert(!"Incorrect grass planes save file");
	}

	fread_be(g_NoTerrainLevelBaseHeight, fin);
	uint32_t totalPlanes = 0;
	fread_be(totalPlanes, fin);

	for (uint32_t i = 0; i < totalPlanes; ++i)
	{
		GrassPlane &gp = AddPlane();
		ReadString(gp.name, fin);
		r3dVector v;
		fread_be(v, fin);
		gp.SetOrigin(v, false);
		fread_be(v, fin);
		gp.SetSize(v.x, v.z, false);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////

void GrassPlanesManager::DeletePlane(int idx)
{
	if (static_cast<uint32_t>(idx) < GetPlanesCount())
	{
		g_Manipulator3d.PickerResetPicked();
		planes.Erase(idx);
	}
}

//-------------------------------------------------------------------------
//	GrassPlaneGameObjectProxy
//-------------------------------------------------------------------------

IMPLEMENT_CLASS(GrassPlaneGameObjectProxy, "GrassPlaneGameObjectProxy", "Object");
AUTOREGISTER_CLASS(GrassPlaneGameObjectProxy);

//////////////////////////////////////////////////////////////////////////

GrassPlaneGameObjectProxy::GrassPlaneGameObjectProxy()
: planeIdx(INVALID_GRASS_PLANE_INDEX)
, resizeMode(false)
{
	m_isSerializable = false;
}

//////////////////////////////////////////////////////////////////////////

void GrassPlaneGameObjectProxy::SelectGrassPlane(int idx)
{
	if (planeIdx == idx)
		return;

	planeIdx = idx;

	GrassPlane &gp = GetGrassPlaneManager()->GetPlane(idx);
	r3dVector v = gp.GetBounds().Org;
	if (resizeMode)
	{
		v += gp.GetBounds().Size;
	}
	SetPosition(v);

	g_Manipulator3d.Enable();
	g_Manipulator3d.PickerResetPicked();
	g_Manipulator3d.PickerAddToPicked(this);
}

//////////////////////////////////////////////////////////////////////////

void GrassPlaneGameObjectProxy::SetPosition(const r3dPoint3D& pos)
{
	parent::SetPosition(pos);
	GrassPlane &gp = GetGrassPlaneManager()->GetPlane(planeIdx);
	r3dVector v = GetPosition();
	if (resizeMode)
	{
		v -= gp.GetBounds().Org;
		gp.SetSize(v.x, v.z, false);
	}
	else
	{
		gp.SetOrigin(v, false);
	}

}

//////////////////////////////////////////////////////////////////////////

void GrassPlaneGameObjectProxy::OnPickerActionEnd()
{
	g_pGrassMap->UpdateHeight();
}

//////////////////////////////////////////////////////////////////////////

void GrassPlaneGameObjectProxy::SetResizeMode(bool doResizeMode)
{
	resizeMode = doResizeMode;
	GrassPlane &gp = GetGrassPlaneManager()->GetPlane(planeIdx);
	r3dVector v = gp.GetBounds().Org;
	if (resizeMode)
	{
		v += gp.GetBounds().Size;
	}
	SetPosition(v);
}

//////////////////////////////////////////////////////////////////////////

#endif FINAL_BUILD