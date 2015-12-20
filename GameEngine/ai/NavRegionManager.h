//=========================================================================
//	Module: NavGenerationHelpers.h
//	Copyright (C) Online Warmongers Group Inc. 2012.
//=========================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////

namespace Nav
{
	class ConvexRegionsManager
	{
		friend struct Editor_Level;
		struct Convex
		{
			typedef r3dgameVector(r3dPoint2D) Points;
			Points pts;
			//	BBox
			r3dPoint2D minPt;
			r3dPoint2D maxPt;
			
			bool haveSeed;
			r3dPoint3D seedPoint;

			Convex(): minPt(0, 0), maxPt(0, 0), haveSeed(false) { }
			void CalcBBox();
		};
		typedef r3dgameVector(Convex) Regions;

		Regions regions;
		uint32_t hightlightedRegion;

		bool IsPointInsideConvex(const r3dPoint2D &p, const Convex &polygon) const;

	public:
		ConvexRegionsManager();
		void CreateNewRegion();
		void DeleteRegion(uint32_t idx);
		uint32_t GetNumRegions() const { return regions.size(); }
		void AddPointToRegion(const r3dPoint3D &pt, uint32_t idx);
		void VisualizeRegions();
		void HightlightRegion(uint32_t idx) { hightlightedRegion = idx; }
		bool IsPointInsideRegions(const r3dPoint2D &p) const;
		void GetRegionBox(uint32_t idx, r3dPoint2D &minPt, r3dPoint2D &maxPt) const;
		r3dBoundBox CalcBBox() const;
		void FillSeedPoints(r3dTL::TArray<r3dVector>& seedPoints);

		/**
		* Reject all triangles that lie outside of user defined nav region.
		* @return new index count.
		*/
		uint32_t FilterOutsideTriangles(const r3dVector *verices, int *indices, PxU32 numIndices);

		/**	Quick conservative object-in-region AABB based test. */
		bool IsIntersectSomeRegion(const r3dBoundBox &bb);

		bool Save(pugi::xml_document& xmlDoc);
		bool Load(const pugi::xml_document &xmlDoc);

		bool doDebugDraw;
	};

	extern ConvexRegionsManager gConvexRegionsManager;
}

