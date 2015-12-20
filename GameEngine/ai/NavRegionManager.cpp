//=========================================================================
//	Module: NavGenerationHelpers.cpp
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "NavRegionManager.h"
#include "../../EclipseStudio/Sources/XMLHelpers.h"
#include "../TrueNature2/Terrain3.h"

#ifdef WO_SERVER
	// empty function for server from NavGenerationHelpers.cpp
	void ResetCachedLevelGeometry()
	{
	}
#else
	#include "NavGenerationHelpers.h"

	extern r3dCamera gCam;
	extern float g_NoTerrainLevelBaseHeight;
#endif

//////////////////////////////////////////////////////////////////////////

namespace Nav
{
	ConvexRegionsManager gConvexRegionsManager;

//-------------------------------------------------------------------------
//	NavConvexRegionsManager
//-------------------------------------------------------------------------

	namespace
	{
		typedef float VECT2[2];
#define SWAP(i,j) { int Tmp = Ind[i]; Ind[i] = Ind[j]; Ind[j] = Tmp; }
		static const VECT2 *Sort_Pts;

		//////////////////////////////////////////////////////////////////////////

		static float Angle(const VECT2 P0, const VECT2 P1, const VECT2 P2)
		{
			// "Angle" between P0->P1 and P0->P2.
			// actually returns: ||(P1-P0) ^ (P2-P0)||
			float dx21 = P2[0] - P1[0];
			float dy21 = P2[1] - P1[1];
			float dx10 = P1[0] - P0[0];
			float dy10 = P1[1] - P0[1];
			return (dy21*dx10 - dx21*dy10);
		}

		//////////////////////////////////////////////////////////////////////////

		static int Compare(const VECT2 A, const VECT2 B) {
			if      (A[0]<B[0]) return  1;
			else if (A[0]>B[0]) return -1;
			else return 0;
		}

		//////////////////////////////////////////////////////////////////////////

		static int Cmp_Lo(const void *a, const void *b) {
			return Compare( Sort_Pts[*(int*)a], Sort_Pts[*(int*)b] );
		}

		//////////////////////////////////////////////////////////////////////////

		int QHull_Internal(int N, const VECT2 *Pts, int *Ind,
			int iA, int iB)
		{
			int n, nn, n2, m, Split1, Split2, iMid;
			int Mid_Pt;
			double dMax;

			if (N<=2) return N;
			// As middle point, search the farthest away from line [A-->B]
			Mid_Pt = -1;
			dMax = 0.0;
			for(n=1; n<N; ++n) {
				double d = fabs( Angle( Pts[Ind[n]], Pts[iA], Pts[iB] ) );
				if (d>=dMax) { dMax = d; Mid_Pt = n; }
			}
			// Partition against midpoint
			iMid = Ind[Mid_Pt];
			Ind[Mid_Pt] = Ind[N-1];
			//Ind = [A|...........|xxx] (B)
			//  n = [0|1.......N-2|N-1|

			nn = N-2;
			n  = 1;
			while(n<=nn) {
				float d = Angle( Pts[Ind[n]], Pts[iA], Pts[iMid] );
				if (d>=0.0) { SWAP(n,nn); nn--; }
				else n++;
			}
			Ind[N-1] = Ind[n];
			Ind[n] = iMid;
			Split1 = n++;
			// Partition Status:
			//Ind = (A) [..(+)..| M |...........] (B)
			//  n =     [1......|Sp1|........N-1]
			nn = N-1;
			while(n<=nn) {
				float d = Angle( Pts[Ind[n]], Pts[iMid], Pts[iB] );
				if (d>=0.0) { SWAP(n,nn); nn--; }
				else n++;
			}
			Split2 = n;
			// Partition Status:
			//Ind = (A) [....(+)...| M |....(-)....|(trash)......] (B)
			//  N =     [1.........|Sp1|...........|Sp2.......N-1]

			// Recurse each sub-partition

			n  = QHull_Internal(Split1,        Pts, Ind       , iA, iMid);
			n2 = QHull_Internal(Split2-Split1, Pts, Ind+Split1, iMid, iB);
			m = Split1;
			while(n2-->0) {
				SWAP(n,m);
				m++; n++;
			}

			return n;
		}

		//////////////////////////////////////////////////////////////////////////

		int QuickHullAlgo(int Nb, const VECT2 *Pts, int *Ind)
		{
			int n, nn, m, iA, iB;

			if (Nb<=2) return Nb;

			Sort_Pts = Pts; // nasty. Only to overcome qsort()'s API
			qsort(Ind, Nb, sizeof(int), Cmp_Lo);
			// first partitioning: classify points with respect to
			// the line joining the extreme points #0 and #Nb-1
			iA = Ind[0];
			iB = Ind[Nb-1];

			m = Nb-2;
			n = 1;
			while(n<=m) {
				float d = Angle( Pts[Ind[n]], Pts[iA], Pts[iB] );
				if (d>=0.0) { SWAP(n,m); m--; }
				else n++;
			}
			Ind[Nb-1] = Ind[n];
			Ind[n] = iB;
			// Partition is now:
			//  Ind = [ A|...(+)....[B]...(-)...|A ]
			//   n  = [0 |1.........[n].........|Nb]
			// We now process the two halves [A..(+)..B] and [B..(-)..A]
			m  = QHull_Internal(   n, Pts, Ind  , iA, iB); // 1st half [A..(+)..B]
			nn = QHull_Internal(Nb-n, Pts, Ind+n, iB, iA); // 2nd half [B..(-)..A]

			while(nn-->0) {
				SWAP(m,n);
				m++; n++;
			}
			return m;
		}
	}

//////////////////////////////////////////////////////////////////////////

	ConvexRegionsManager::ConvexRegionsManager()
	: hightlightedRegion(0)
	, doDebugDraw(false)
	{

	}

//////////////////////////////////////////////////////////////////////////

	void ConvexRegionsManager::AddPointToRegion(const r3dPoint3D &pt, uint32_t idx)
	{
		if (regions.size() == 0 || idx >= regions.size())
			return;

		Convex &c = regions[idx];

		r3dPoint2D pt2D(pt.x, pt.z);

		c.pts.push_back(pt2D);

		VECT2 *pts = reinterpret_cast<VECT2*>(&c.pts[0].x);
		int *indices = game_new int[c.pts.size()];

		for (int i = 0; i < static_cast<int>(c.pts.size()); ++i)
		{
			indices[i] = i;
		}

		int n = QuickHullAlgo(c.pts.size(), pts, indices);

		Convex::Points newC;
		for (int i = 0; i < n; ++i)
		{
			newC.push_back(c.pts[indices[i]]);
		}
		c.pts = newC;
		c.CalcBBox();

		ResetCachedLevelGeometry();
	}

//////////////////////////////////////////////////////////////////////////

	void ConvexRegionsManager::CreateNewRegion()
	{
		regions.resize(regions.size() + 1);
	}

//////////////////////////////////////////////////////////////////////////

	void ConvexRegionsManager::DeleteRegion(uint32_t idx)
	{
		if (regions.size() <= idx)
			return;

		regions.erase(regions.begin() + idx);

		ResetCachedLevelGeometry();
	}

//////////////////////////////////////////////////////////////////////////

#ifndef WO_SERVER
	void ConvexRegionsManager::VisualizeRegions()
	{
		if (!doDebugDraw)
			return;

		r3dRenderer->SetCullMode(D3DCULL_NONE);
		int psIdx = r3dRenderer->GetCurrentPixelShaderIdx();
		int vsIdx = r3dRenderer->GetCurrentVertexShaderIdx();

		r3dRenderer->SetPixelShader();
		r3dRenderer->SetVertexShader();

		int rm = r3dRenderer->GetRenderingMode();
		r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_ZC);

		const float fenceHeight = 5.0f;

		const r3dColor fenceCl(0, 255, 0, 90);
		const r3dColor brightfenceCl(255, 255, 0, 90);

		float baseFenceHeight = g_NoTerrainLevelBaseHeight;

		for (size_t i = 0; i < regions.size(); ++i)
		{
			Convex &c = regions[i];

			const r3dColor &modCl = (i == hightlightedRegion ? brightfenceCl : fenceCl);

			for (size_t j = 0; j < c.pts.size(); ++j)
			{
				r3dPoint2D &p2D0 = c.pts[j];
				r3dPoint2D &p2D1 = c.pts[(j + 1) % c.pts.size()];

				if (Terrain)
					baseFenceHeight = Terrain->GetHeight(r3dPoint3D(p2D0.x, 0, p2D0.y));

				r3dPoint3D p0(p2D0.x, baseFenceHeight, p2D0.y);
				r3dPoint3D p2(p2D0.x, fenceHeight + baseFenceHeight, p2D0.y);

				if (Terrain)
					baseFenceHeight = Terrain->GetHeight(r3dPoint3D(p2D1.x, 0, p2D1.y));

				r3dPoint3D p1(p2D1.x, baseFenceHeight, p2D1.y);
				r3dPoint3D p3(p2D1.x, fenceHeight + baseFenceHeight, p2D1.y);

				r3dDrawTriangle3D(p0, p1, p2, gCam, modCl, 0, 0, true);
				r3dDrawTriangle3D(p1, p3, p2, gCam, modCl, 0, 0, true);

				r3dDrawTriangle3D(p0, p2, p1, gCam, modCl, 0, 0, true);
				r3dDrawTriangle3D(p2, p3, p1, gCam, modCl, 0, 0, true);
			}
			
			if(c.haveSeed)
			{
				r3dBoundBox bb;
				bb.Size.Assign(0.3f, 2.0f, 0.3f);
				bb.Org = c.seedPoint - bb.Size / 2;
				bb.Org.y += bb.Size.y / 2;

				r3dDrawBoundBox(bb, gCam, r3dColor::green);
			}
		}

		r3dRenderer->Flush();
		r3dRenderer->SetRenderingMode(rm);

		r3dRenderer->SetPixelShader(psIdx);
		r3dRenderer->SetVertexShader(vsIdx);

		r3dRenderer->RestoreCullMode();
	}
#endif // WO_SERVER

//////////////////////////////////////////////////////////////////////////

	bool ConvexRegionsManager::IsPointInsideConvex(const r3dPoint2D &p, const Convex &polygon) const
	{
		if (polygon.pts.empty())
			return false;

		int counter = 0;
		float xinters;
		r3dPoint2D p1, p2;

		int N = static_cast<int>(polygon.pts.size());

		p1 = polygon.pts[0];
		for (int i = 1; i <= N; i++)
		{
			p2 = polygon.pts[i % N];
			if (p.y > std::min(p1.y,p2.y))
			{
				if (p.y <= std::max(p1.y,p2.y))
				{
					if (p.x <= std::max(p1.x,p2.x))
					{
						if (p1.y != p2.y)
						{
							xinters = (p.y - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + p1.x;
							if (p1.x == p2.x || p.x <= xinters)
								counter++;
						}
					}
				}
			}
			p1 = p2;
		}

		return counter % 2 != 0;
	}

//////////////////////////////////////////////////////////////////////////

	r3dBoundBox ConvexRegionsManager::CalcBBox() const
	{
		r3dPoint2D minPt, maxPt;
		for (size_t j = 0; j < regions.size(); ++j)
		{
			const Convex &c = regions[j];

			const r3dPoint2D & polyMin(c.minPt), polyMax(c.maxPt);

			if (j == 0)
			{
				maxPt = polyMax;
				minPt = polyMin;
			}
			else
			{
				maxPt.x = std::max(maxPt.x, polyMax.x);
				maxPt.y = std::max(maxPt.y, polyMax.y);
				minPt.x = std::min(minPt.x, polyMin.x);
				minPt.y = std::min(minPt.y, polyMin.y);
			}
		}

		r3dBoundBox bb;
		bb.Org.x   = minPt.x;
		bb.Org.z   = minPt.y;
		bb.Org.y   = -100;
		bb.Size.x = maxPt.x - bb.Org.x;
		bb.Size.z = maxPt.y - bb.Org.y;
		bb.Size.y = 200;
		if (Terrain)
		{
			const r3dTerrainDesc &td = Terrain->GetDesc();
			bb.Org.y  = td.MinHeight;
			bb.Size.y = td.MaxHeight - bb.Org.y;
		}
		return bb;
	}

//////////////////////////////////////////////////////////////////////////

	void ConvexRegionsManager::Convex::CalcBBox()
	{
		if (pts.empty())
			return;

		minPt = pts[0];
		maxPt = pts[0];
		for (size_t i = 1; i < pts.size(); ++i)
		{
			minPt.x = std::min(pts[i].x, minPt.x);
			minPt.y = std::min(pts[i].y, minPt.y);

			maxPt.x = std::max(pts[i].x, maxPt.x);
			maxPt.y = std::max(pts[i].y, maxPt.y);
		}
	}

//////////////////////////////////////////////////////////////////////////
	
	void ConvexRegionsManager::FillSeedPoints(r3dTL::TArray<r3dVector>& seedPoints)
	{
		for (size_t j = 0; j < regions.size(); ++j)
		{
			const Convex &c = regions[j];
			if (c.haveSeed)
				seedPoints.PushBack(c.seedPoint);
		}
	}
	
//////////////////////////////////////////////////////////////////////////

	bool ConvexRegionsManager::IsPointInsideRegions(const r3dPoint2D &p) const
	{
		for (size_t j = 0; j < regions.size(); ++j)
		{
			const Convex &c = regions[j];
			if (c.pts.empty())
				continue;

			//	Calc AABB for quick rejection
			const r3dPoint2D& minPt(c.minPt), maxPt(c.maxPt);

			//	Quick AABB reject
			if (minPt.x > p.x || minPt.y > p.y || maxPt.x < p.x || maxPt.y < p.y)
				continue;

			if (IsPointInsideConvex(p, c))
				return true;
		}
		return false;
	}

//////////////////////////////////////////////////////////////////////////

	void ConvexRegionsManager::GetRegionBox(uint32_t idx, r3dPoint2D &minPt, r3dPoint2D &maxPt) const
	{
		if (idx >= regions.size())
			return;

		const Convex &c = regions[idx];
		minPt = c.minPt;
		maxPt = c.maxPt;
	}

//////////////////////////////////////////////////////////////////////////

	bool ConvexRegionsManager::Load(const pugi::xml_document &xmlDoc)
	{
		regions.clear();
		pugi::xml_node n = xmlDoc.child("convex");
		while (!n.empty())
		{
			Convex c;
			pugi::xml_node pts = n.child("point");
			while (!pts.empty())
			{
				r3dPoint2D pt;
				pt.x = pts.attribute("x").as_float();
				pt.y = pts.attribute("y").as_float();
				c.pts.push_back(pt);
				pts = pts.next_sibling("point");
			}
			c.CalcBBox();
			
			pugi::xml_node seed = n.child("seed");
			if(seed.attribute("set").as_int() > 0)
			{
				c.haveSeed = true;
				c.seedPoint.x = seed.attribute("x").as_float();
				c.seedPoint.y = seed.attribute("y").as_float();
				c.seedPoint.z = seed.attribute("z").as_float();
			}
			
			regions.push_back(c);
			n = n.next_sibling("convex");
		}
		return true;
	}

//////////////////////////////////////////////////////////////////////////

	bool ConvexRegionsManager::Save(pugi::xml_document& xmlDoc)
	{
		for (uint32_t i = 0; i < regions.size(); ++i)
		{
			Convex &c = regions[i];
			if (c.pts.empty())
				continue;

			pugi::xml_node n = xmlDoc.append_child();
			n.set_name("convex");

			for (uint32_t j = 0; j < c.pts.size(); ++j)
			{
				SetXMLVal("point", n, &c.pts[j]);
			}

			if(c.haveSeed)
			{
				pugi::xml_node seed = n.append_child();
				seed.set_name("seed");
				seed.append_attribute("set") = 1;
				seed.append_attribute("x") = c.seedPoint.x;
				seed.append_attribute("y") = c.seedPoint.y;
				seed.append_attribute("z") = c.seedPoint.z;
			}
		}

		return true;
	}

//////////////////////////////////////////////////////////////////////////

	uint32_t ConvexRegionsManager::FilterOutsideTriangles(const r3dVector *verices, int *indices, PxU32 numIndices)
	{
		r3d_assert(numIndices % 3 == 0);

		int *indEnd = indices + numIndices;
		int *indCur = indices;
		uint32_t resultIndices = numIndices;

		while (indCur < indEnd)
		{
			const r3dVector &v0 = verices[indCur[0]];
			const r3dVector &v1 = verices[indCur[1]];
			const r3dVector &v2 = verices[indCur[2]];

			r3dPoint2D pt0(v0.x, v0.z);
			r3dPoint2D pt1(v1.x, v1.z);
			r3dPoint2D pt2(v2.x, v2.z);

			bool inside =
				IsPointInsideRegions(pt0) ||
				IsPointInsideRegions(pt1) ||
				IsPointInsideRegions(pt2);

			//	If all vertices are outside, delete this indices
			if (!inside)
			{
				indCur[0] = indEnd[-3];
				indCur[1] = indEnd[-2];
				indCur[2] = indEnd[-1];
				indEnd -= 3;
				resultIndices -= 3;
			}
			else
			{
				indCur += 3;
			}
		}
		return resultIndices;
	}

//////////////////////////////////////////////////////////////////////////

	bool ConvexRegionsManager::IsIntersectSomeRegion(const r3dBoundBox &bb)
	{
		for (uint32_t i = 0; i < GetNumRegions(); ++i)
		{
			r3dPoint2D minPt, maxPt;
			GetRegionBox(i, minPt, maxPt);

			if (minPt.x <= bb.Org.x + bb.Size.x && maxPt.x > bb.Org.x &&
				minPt.y <= bb.Org.z + bb.Size.z && maxPt.y > bb.Org.z)
				return true;
		}
		return false;
	}
}

