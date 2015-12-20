//=========================================================================
//	Module: AutodeskCustomBridges.cpp
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#if ENABLE_AUTODESK_NAVIGATION

//////////////////////////////////////////////////////////////////////////

#include "../../gameobjects/PhysXWorld.h"
#include "../../gameobjects/PhysObj.h"
#include "AutodeskCustomBridges.h"
#include "../NavGenerationHelpers.h"
#include "JobChief.h"

//////////////////////////////////////////////////////////////////////////

extern r3dCamera gCam;
using namespace Kaim;
using namespace Nav;

//-------------------------------------------------------------------------
//	class AutodeskGeneratorInputProducer
//-------------------------------------------------------------------------

AutodeskGeneratorInputProducer::AutodeskGeneratorInputProducer()
{

}

//////////////////////////////////////////////////////////////////////////

KyResult AutodeskGeneratorInputProducer::Produce(const Kaim::GeneratorSector& sector, Kaim::ClientInputConsumer& inputConsumer)
{
#ifndef WO_SERVER
	PopulateLevelMesh(gLevelMesh);

	Kaim::DynamicNavTag navTag;

	for (uint32_t i = 0; i < gLevelMesh->meshes.Count(); ++i)
	{
		Nav::LevelGeometry::Mesh &p = gLevelMesh->meshes[i];

		for (uint32_t i = 0; i < p.indices.Count(); i += 3)
		{
			const r3dVector &pt1 = p.vertices[p.indices[i + 0]];
			const r3dVector &pt2 = p.vertices[p.indices[i + 1]];
			const r3dVector &pt3 = p.vertices[p.indices[i + 2]];

			Vec3f p1(pt1.x, pt1.z, pt1.y);
			Vec3f p2(pt2.x, pt2.z, pt2.y);
			Vec3f p3(pt3.x, pt3.z, pt3.y);

			inputConsumer.ConsumeTriangle(p1, p2, p3, navTag);
		}
	}
	
	for (uint32_t i = 0; i < seedPoints.Count(); ++i)
	{
		const r3dVector & sp = seedPoints[i];
		Vec3f pos(sp.x, sp.z, sp.y);
		inputConsumer.ConsumeSeedPoint(pos);
	}
#endif	
	return Kaim::Result::Success;
}

//-------------------------------------------------------------------------
//	class AutodeskNavMeshDrawer
//-------------------------------------------------------------------------

AutodeskNavMeshDrawer::AutodeskNavMeshDrawer()
: vtxOffset(0)
, lockPtr(0)
, capacity(0)
, ndxOffset(0)
, lockNdxPtr(0)
, ndxCapacity(0)
{
}

//////////////////////////////////////////////////////////////////////////

AutodeskNavMeshDrawer::~AutodeskNavMeshDrawer()
{
// Calling D3DReleaseResource here causes an unhandled exception
// accessing memory, because it is trying to access memory that
// has already been freed.
//	D3DReleaseResource();
}

//////////////////////////////////////////////////////////////////////////

void AutodeskNavMeshDrawer::DoBegin(const VisualGeometrySetupConfig& setupConfig)
{
	if (setupConfig.m_opaqueTriangleCount > 0)
		r3dError("need to handle m_opaqueTriangleCount");
	
	if (setupConfig.m_transparentTriangleCount == 0)
		return;

	if (capacity != setupConfig.m_transparentTriangleCount * 3)
	{
		D3DReleaseResource();

		capacity = setupConfig.m_transparentTriangleCount * 3;
		ndxCapacity = 2 * capacity;
		D3DCreateResource();
	}
	void **ptr = reinterpret_cast<void**>(&lockPtr);
	vertices.Lock(0, 0, ptr, 0);

	void **ptrNdx = reinterpret_cast<void**>(&lockNdxPtr);
	indexes.Lock(0, 0, ptrNdx, 0);
}

//////////////////////////////////////////////////////////////////////////

void AutodeskNavMeshDrawer::DoPushTriangle(const Kaim::VisualTriangle& triangle)
{
	if (!lockPtr || !lockNdxPtr)
		return;

	R3D_DEBUG_VERTEX v[3];

	const Vec3f &v1 = (triangle.A);
	const Vec3f &v2 = (triangle.B);
	const Vec3f &v3 = (triangle.C);

	v[0].Pos.Assign(v1.x, v1.z + 0.1f, v1.y);
	v[1].Pos.Assign(v2.x, v2.z + 0.1f, v2.y);
	v[2].Pos.Assign(v3.x, v3.z + 0.1f, v3.y);

	for (int i = 0; i < _countof(v); ++i)
	{
		v[i].color = r3dColor24(triangle.m_color.m_r, triangle.m_color.m_g, triangle.m_color.m_b, triangle.m_color.m_a);
		Vec3f n = triangle.m_normal;
		v[i].Normal.Assign(n.x, n.z, n.y);
		v[i].tu = 0;
		v[i].tv = 0;
	}

	uint32_t ndx[6];
	ndx[0] = vtxOffset;
	ndx[1] = vtxOffset + 1;
	ndx[2] = vtxOffset + 1;
	ndx[3] = vtxOffset + 2;
	ndx[4] = vtxOffset + 2;
	ndx[5] = vtxOffset;

	const uint32_t vertexSize = sizeof(R3D_DEBUG_VERTEX);
	r3d_assert(vtxOffset < capacity);
	memcpy_s(lockPtr + vtxOffset * vertexSize, (capacity - vtxOffset) * vertexSize, v, _countof(v) * sizeof(v[0]));
	vtxOffset += _countof(v);

	const uint32_t ndxSize = sizeof(uint32_t);
	r3d_assert(ndxOffset < ndxCapacity);
	memcpy_s(lockNdxPtr + ndxOffset * ndxSize, (ndxCapacity - ndxOffset) * ndxSize, ndx, _countof(ndx) * sizeof(ndx[0]));
	ndxOffset += 2 * _countof(v);
}

void AutodeskNavMeshDrawer::DoPushLine(const Kaim::VisualLine& visualLine)
{
}

void AutodeskNavMeshDrawer::DoPushText(const Kaim::VisualText& visualText)
{
}

//////////////////////////////////////////////////////////////////////////

void AutodeskNavMeshDrawer::DoEnd()
{
	if (lockNdxPtr)
	{
		indexes.Unlock();
		lockNdxPtr = 0;
		ndxOffset = 0;
	}

	if (lockPtr)
	{
		vertices.Unlock();
		lockPtr = 0;
		vtxOffset = 0;
	}
}

//////////////////////////////////////////////////////////////////////////

void AutodeskNavMeshDrawer::Render()
{
	if (capacity == 0)
		return;
	//	Check for unlocked buffer
	r3d_assert(lockPtr == 0);

	d3dc._SetDecl(R3D_DEBUG_VERTEX::getDecl());
	d3dc._SetStreamSource(0, vertices.Get(), 0, sizeof(R3D_DEBUG_VERTEX));
	d3dc._SetIndices(indexes.Get());

	r3dRenderer->SetCullMode(D3DCULL_NONE);
	int psIdx = r3dRenderer->GetCurrentPixelShaderIdx();
	int vsIdx = r3dRenderer->GetCurrentVertexShaderIdx();

	r3dRenderer->SetPixelShader();
	r3dRenderer->SetVertexShader();

	int rm = r3dRenderer->GetRenderingMode();
	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_ZC);

	r3dRenderer->Draw(D3DPT_TRIANGLELIST, 0, capacity / 3);

	float colr[ 4 ] = { r3dColor::yellow.R, r3dColor::yellow.G, r3dColor::yellow.B, r3dColor::yellow.A } ;
	D3D_V( r3dRenderer->pd3ddev->SetPixelShaderConstantF( 0, colr, 1 ) ) ;
	r3dRenderer->DrawIndexed(D3DPT_LINELIST, 0, 0, 2, 0, ndxCapacity / 2);

	r3dRenderer->SetRenderingMode(rm);

	r3dRenderer->SetPixelShader(psIdx);
	r3dRenderer->SetVertexShader(vsIdx);

	r3dRenderer->RestoreCullMode();
}

//////////////////////////////////////////////////////////////////////////

void AutodeskNavMeshDrawer::D3DCreateResource()
{
	r3d_assert(capacity > 0);
	
	r3dDeviceTunnel::CreateVertexBuffer(capacity * sizeof(R3D_DEBUG_VERTEX), 0, 0, D3DPOOL_MANAGED, &vertices) ;
	r3dDeviceTunnel::SetD3DResourcePrivateData(&vertices, "AutodeskNavMeshDrawer: VB");

	/*6 indexes to produce three lines using 3 verts per triangle = 2 times the number of vertices * size of storage type */
	r3dDeviceTunnel::CreateIndexBuffer(2 * capacity * sizeof(uint32_t), 0, D3DFMT_INDEX32, D3DPOOL_MANAGED, &indexes);
	r3dDeviceTunnel::SetD3DResourcePrivateData(&indexes, "AutodeskNavMeshDrawer: IB");
}

//////////////////////////////////////////////////////////////////////////

void AutodeskNavMeshDrawer::D3DReleaseResource()
{
	if (indexes.Valid())
		indexes.ReleaseAndReset();

	if (!vertices.Valid())	return;

	vertices.ReleaseAndReset();
	capacity = 0;
}

//-------------------------------------------------------------------------
//	AutodeskPerfCounter class
//-------------------------------------------------------------------------

AutodeskPerfCounter::AutodeskPerfCounter()
: newStrOffset(0)
{
	ZeroMemory(strBuf, _countof(strBuf));
}

//////////////////////////////////////////////////////////////////////////

AutodeskPerfCounter::~AutodeskPerfCounter()
{

}

//////////////////////////////////////////////////////////////////////////

void AutodeskPerfCounter::Begin(const char* name)
{
	uint32_t l = strlen(name);
	r3d_assert(newStrOffset + l + 1 < _countof(strBuf));

	r3dProfiler::StartSample(name, r3dHash::MakeHash(name));
	eventsStack.PushBack(strBuf + newStrOffset);

	memcpy_s(strBuf + newStrOffset, _countof(strBuf) - newStrOffset, name, l);
	newStrOffset += l;
	strBuf[newStrOffset] = 0;
	newStrOffset += 1;
}

//////////////////////////////////////////////////////////////////////////

void AutodeskPerfCounter::End()
{
	char *s = eventsStack.GetLast();
	r3dProfiler::EndSample(s, r3dHash::MakeHash(s));

	newStrOffset -= strlen(s) + 1;

	eventsStack.PopBack();
}

//////////////////////////////////////////////////////////////////////////

AutodeskPerfCounter2::AutodeskPerfCounter2()
{
}

AutodeskPerfCounter2::~AutodeskPerfCounter2()
{
}

void AutodeskPerfCounter2::Reset()
{
	counters.clear();
	events.clear();
}

void AutodeskPerfCounter2::Dump()
{
	r3dOutToLog("Autodesk Counters:\n");
	float cntTime = 0;
	for(EventType::iterator it = counters.begin(); it != counters.end(); ++it)
	{
		r3dOutToLog("%2.7f, %s\n", it->second, it->first);
		cntTime += it->second;
	}

	r3dOutToLog("Autodesk Events:\n");
	float evtTime = 0;
	for(EventType::iterator it = events.begin(); it != events.end(); ++it)
	{
		r3dOutToLog("%2.7f, %s\n", it->second, it->first);
		evtTime += it->second;
	}
	r3dOutToLog("counters: %.6f, events: %.6f, total: %.6f\n", cntTime, evtTime, cntTime + evtTime);
}

void AutodeskPerfCounter2::Begin(const char* name)
{
	EventType::value_type p = std::pair<const char*, float>(name, 0.0f);
	std::pair<EventType::iterator, bool> result = events.insert(p);
	lastEvent = result.first;
	lastTime  = r3dGetTime();
}

//////////////////////////////////////////////////////////////////////////

void AutodeskPerfCounter2::End()
{
	float t2 = r3dGetTime() - lastTime;
	lastEvent->second += t2;
}

//-------------------------------------------------------------------------
//	AutodeskParrallelBuilder class
//-------------------------------------------------------------------------

namespace
{
	struct ParallelJobDesc
	{
		void **elements;
		Kaim::IParallelElementFunctor *fn;
	};

//////////////////////////////////////////////////////////////////////////

	void ParallelJobFunc(void* Data, size_t ItemStart, size_t ItemCount, size_t ThreadIndex )
	{
		(void)ThreadIndex;
		ParallelJobDesc *jd = reinterpret_cast<ParallelJobDesc*>(Data);
		for (size_t i = 0; i < ItemCount; ++i)
		{
			jd->fn->Do(jd->elements[ItemStart + i]);
		}
	}
}

//////////////////////////////////////////////////////////////////////////

KyResult AutodeskParallelBuilder::Init()
{
	return Result::Success;
}

//////////////////////////////////////////////////////////////////////////

KyResult AutodeskParallelBuilder::ParallelFor(void **elements, KyUInt32 elementsCount, Kaim::IParallelElementFunctor *functor)
{
	ParallelJobDesc jd;
	jd.elements = elements;
	jd.fn = functor;
	g_pJobChief->Exec(&ParallelJobFunc, &jd, elementsCount);

	return Result::Success;
}

//////////////////////////////////////////////////////////////////////////

KyResult AutodeskParallelBuilder::Terminate()
{
	return Result::Success;
}

#endif 