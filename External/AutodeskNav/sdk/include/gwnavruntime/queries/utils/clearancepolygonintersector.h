/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: NOBODY
#ifndef Navigation_TrapezoidIntersector_H
#define Navigation_TrapezoidIntersector_H

#include "gwnavruntime/database/database.h"
#include "gwnavruntime/navmesh/intersections.h"
#include "gwnavruntime/navmesh/closestpoint.h"
#include "gwnavruntime/navmesh/identifiers/navhalfedgerawptr.h"
#include "gwnavruntime/containers/sharedpoollist.h"

namespace Kaim
{

/*
class TrapezoidIntersector
*/
class ClearancePolygonIntersector
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_Query)

public:
	class BorderEdge
	{
	public:
		BorderEdge() :
		m_axisStart_f(0.f),
		m_axisEnd_f(0.f),
		m_ordinateStart_f(0.f),
		m_ordinateEnd_f(0.f)
		{}

	public:
		KyFloat32 m_axisStart_f;
		KyFloat32 m_axisEnd_f;
		KyFloat32 m_ordinateStart_f;
		KyFloat32 m_ordinateEnd_f;
		Vec3f m_startPos;
		Vec3f m_endPos;
		NavHalfEdgeRawPtr m_edgeRawPtr;
	};

	/*
	        onStartLeft                       onEndLeft
	--------+--------------------------------+-------------------------------------
	         \                              /
	          \                           /
	           \                        /
	            \                     /
	             \                  /
	--------------+---------------+------------------------------------------------
	             edgeStart         edgeEnd
	*/
	ClearancePolygonIntersector() : m_borderEdgePool(Memory::GetGlobalHeap(), MemStat_Channel, PoolChunkSize::SlotCount, 256)
	{
		m_borderEdgesOnLeft.SetNodePool(&m_borderEdgePool);
		m_borderEdgesOnRight.SetNodePool(&m_borderEdgePool);
	}

	void InitTrapezoid(const Vec3f& pathEdgeStartPos3f, const Vec3f& pathEdgeEndPos3f, const Vec3f& onStartLeft, const Vec3f& onEndLeft, const DatabaseGenMetrics& genMetrics)
	{
		m_type = Trapeze;
		m_integerPrecision = genMetrics.m_integerPrecision;
		m_onStartLeftPos3f  =  onStartLeft;
		m_onEndLeftPos3f    =    onEndLeft;
		m_pathEdgeStartPos3f = pathEdgeStartPos3f;
		m_pathEdgeEndPos3f =   pathEdgeEndPos3f;
		m_onStartRightPos3f = pathEdgeStartPos3f - (m_onStartLeftPos3f-m_pathEdgeStartPos3f);
		m_onEndRightPos3f   = m_pathEdgeEndPos3f - (m_onEndLeftPos3f-m_pathEdgeEndPos3f);

		InitFirstBorders(0.f, 1.f);

		m_axisMultFactor = m_crossProdAB_CD_f / m_crossProdAC_CD_f;
	}

	void InitTriangleAroundRight(const Vec3f& pathEdgeStartPos3f, const Vec3f& pathEdgeEndPos3f, const Vec3f& onStartLeft, const Vec3f& onEndLeft, const Vec3f& onRight, const DatabaseGenMetrics& genMetrics)
	{
		m_type = TriangleAroundRight;
		m_integerPrecision = genMetrics.m_integerPrecision;
		m_onStartLeftPos3f  =  onStartLeft;
		m_onEndLeftPos3f    =    onEndLeft;
		m_pathEdgeStartPos3f = pathEdgeStartPos3f;
		m_pathEdgeEndPos3f =   pathEdgeEndPos3f;
		m_onStartRightPos3f = onRight;
		m_onEndRightPos3f   = onRight;

		InitFirstBorders(0.f, 1.f);

		m_axisMultFactor = m_crossProdAB_CD_f / m_crossProdAC_CD_f;
	}

	void InitTriangleAroundLeft(const Vec3f& pathEdgeStartPos3f, const Vec3f& pathEdgeEndPos3f, const Vec3f& onLeft, const Vec3f& onStartRight, const Vec3f& onEndRight, const DatabaseGenMetrics& genMetrics)
	{
		m_type = TriangleAroundLeft;
		m_integerPrecision = genMetrics.m_integerPrecision;
		m_onStartLeftPos3f  =  onLeft;
		m_onEndLeftPos3f    =    onLeft;
		m_pathEdgeStartPos3f = pathEdgeStartPos3f;
		m_pathEdgeEndPos3f =   pathEdgeEndPos3f;
		m_onStartRightPos3f = onStartRight;
		m_onEndRightPos3f   = onEndRight;

		InitFirstBorders(0.f, 1.f);

		m_axisMultFactor = 1.f;
	}

	void InitFirstBorders(KyFloat32 startAxis, KyFloat32 endAxis)
	{
		m_distance2d = (m_pathEdgeEndPos3f-m_pathEdgeStartPos3f).GetLength2d();
		const KyFloat32 distanceStep = 0.05f; // 5 cm
		m_axisStepMin = distanceStep / m_distance2d;

		m_borderEdgesOnLeft.Clear();
		m_borderEdgesOnRight.Clear();

		m_crossProdAB_AC_f = CrossProduct_z(m_onStartLeftPos3f-m_pathEdgeStartPos3f, m_pathEdgeEndPos3f-m_pathEdgeStartPos3f);
		m_crossProdAB_CD_f = CrossProduct_z(m_onStartLeftPos3f-m_pathEdgeStartPos3f, m_onEndLeftPos3f-m_pathEdgeEndPos3f);
		m_crossProdAC_CD_f = CrossProduct_z(m_pathEdgeEndPos3f-m_pathEdgeStartPos3f, m_onEndLeftPos3f-m_pathEdgeEndPos3f);

		BorderEdge startBorderEdgeLeft;
		startBorderEdgeLeft.m_axisStart_f = startAxis;
		startBorderEdgeLeft.m_axisEnd_f   = endAxis;

		startBorderEdgeLeft.m_startPos = m_onStartLeftPos3f;
		startBorderEdgeLeft.m_endPos = m_onEndLeftPos3f;

		BorderEdge startBorderEdgeRight;
		startBorderEdgeRight.m_axisStart_f = startBorderEdgeLeft.m_axisStart_f;
		startBorderEdgeRight.m_axisEnd_f   = startBorderEdgeLeft.m_axisEnd_f;
		startBorderEdgeRight.m_startPos = m_onStartRightPos3f;
		startBorderEdgeRight.m_endPos = m_onEndRightPos3f;

		switch (m_type)
		{
		case Trapeze :
			{
				startBorderEdgeLeft.m_ordinateStart_f = -m_crossProdAB_AC_f;
				startBorderEdgeLeft.m_ordinateEnd_f   = -m_crossProdAB_AC_f;
				startBorderEdgeRight.m_ordinateStart_f = m_crossProdAB_AC_f;
				startBorderEdgeRight.m_ordinateEnd_f   = m_crossProdAB_AC_f;
				break;
			}
		case TriangleAroundLeft :
			{
				startBorderEdgeLeft.m_ordinateStart_f = GetOrdinateValue(m_onStartLeftPos3f);
				startBorderEdgeLeft.m_ordinateEnd_f   = startBorderEdgeLeft.m_ordinateStart_f;
				startBorderEdgeRight.m_ordinateStart_f = GetOrdinateValue(m_onStartRightPos3f);
				startBorderEdgeRight.m_ordinateEnd_f   = GetOrdinateValue(m_onEndRightPos3f);
				break;
			}
		case TriangleAroundRight :
			{
				startBorderEdgeLeft.m_ordinateStart_f = GetOrdinateValue(m_onStartLeftPos3f);
				startBorderEdgeLeft.m_ordinateEnd_f   = GetOrdinateValue(m_onEndLeftPos3f);
				startBorderEdgeRight.m_ordinateStart_f = GetOrdinateValue(m_onStartRightPos3f);
				startBorderEdgeRight.m_ordinateEnd_f   = startBorderEdgeRight.m_ordinateStart_f;
				break;
			}
		}

		m_borderEdgesOnLeft.PushBack(startBorderEdgeLeft);
		m_borderEdgesOnRight.PushBack(startBorderEdgeRight);
	}



	~ClearancePolygonIntersector()
	{
		m_borderEdgesOnLeft.Clear();
		m_borderEdgesOnRight.Clear();
	}

	
	bool IsVisibleForBorderLeft(const Vec3f& startPosForThisBorder, const Vec3f& endPosForThisBorder, ClearancePolygonIntersector::BorderEdge* borderEdgeOnLeft)
	{
		if (m_type == Trapeze || m_type == TriangleAroundRight || borderEdgeOnLeft->m_startPos != borderEdgeOnLeft->m_endPos)
		{
			if (borderEdgeOnLeft->m_edgeRawPtr.IsValid() == false)
				return true;

			KyFloat32 crossProd1 = CrossProduct_z(borderEdgeOnLeft->m_endPos - borderEdgeOnLeft->m_startPos, startPosForThisBorder - borderEdgeOnLeft->m_startPos);
			KyFloat32 crossProd2 = CrossProduct_z(borderEdgeOnLeft->m_endPos - borderEdgeOnLeft->m_startPos,   endPosForThisBorder - borderEdgeOnLeft->m_startPos);

			return crossProd1 < 0.f || crossProd2 < 0.f;
		}
		else
		{
			return true;
		}
	}

	bool IsVisibleForBorderRight(const Vec3f& startPosForThisBorder, const Vec3f& endPosForThisBorder, ClearancePolygonIntersector::BorderEdge* borderEdgeOnRight)
	{
		if (m_type == Trapeze || m_type == TriangleAroundLeft || borderEdgeOnRight->m_startPos != borderEdgeOnRight->m_endPos)
		{
			if (borderEdgeOnRight->m_edgeRawPtr.IsValid() == false)
				return true;

			KyFloat32 crossProd1 = CrossProduct_z(borderEdgeOnRight->m_endPos - borderEdgeOnRight->m_startPos, startPosForThisBorder - borderEdgeOnRight->m_startPos);
			KyFloat32 crossProd2 = CrossProduct_z(borderEdgeOnRight->m_endPos - borderEdgeOnRight->m_startPos,   endPosForThisBorder - borderEdgeOnRight->m_startPos);

			return crossProd1 > 0.f || crossProd2 > 0.f;
		}
		else
		{
			return true;
		}
	}

	bool DoesEdgeIntersect(const Vec3f& startEdgePos, const Vec3f& endEdgePos)
	{
		switch (m_type)
		{
		case Trapeze :
			return Intersections::SegmentVsConvexQuad2D(startEdgePos, endEdgePos, m_onStartLeftPos3f, m_onStartRightPos3f, m_onEndLeftPos3f, m_onEndRightPos3f);
		case TriangleAroundLeft :
			return Intersections::SegmentVsTriangle2d(startEdgePos, endEdgePos, m_onStartLeftPos3f, m_onStartRightPos3f, m_onEndRightPos3f);
		case TriangleAroundRight :
			return Intersections::SegmentVsTriangle2d(startEdgePos, endEdgePos, m_onStartLeftPos3f, m_onStartRightPos3f, m_onEndLeftPos3f);
		}

		return true;
	}

	bool IsEdgeVisible(const CoordPos64& /*startEdgeCoordPos*/, const CoordPos64& /*endEdgeCoordPos*/, const NavHalfEdgeRawPtr& edgeRawPtr, bool edgeIsCrossable)
	{
		Vec3f startEdgePos, endEdgePos;
		edgeRawPtr.GetVerticesPos3f(startEdgePos, endEdgePos);

		if (DoesEdgeIntersect(startEdgePos, endEdgePos) == false)
			return false;

		if (edgeIsCrossable)
		{
			if (Intersections::SegmentVsSegment2d(startEdgePos, endEdgePos,
				m_pathEdgeStartPos3f - 45.f * m_integerPrecision * (m_pathEdgeEndPos3f-m_pathEdgeStartPos3f)/m_distance2d,
				m_pathEdgeEndPos3f  +  45.f * m_integerPrecision * (m_pathEdgeEndPos3f-m_pathEdgeStartPos3f)/m_distance2d))
				return true;
		}

		if (BoundEdgeToTrapezoidLimit(startEdgePos, endEdgePos) == false)
			return false;

		KyFloat32 axisEdgeStart = GetAxisValue(startEdgePos);
		KyFloat32 axisEdgeEnd   = GetAxisValue(endEdgePos);

		if (axisEdgeStart < 0.f)
			axisEdgeStart = 0.f;
		else if (axisEdgeStart > 1.f)
			axisEdgeStart = 1.f;

		if (axisEdgeEnd < 0.f)
			axisEdgeEnd = 0.f;
		else if (axisEdgeEnd > 1.f)
			axisEdgeEnd = 1.f;

		Vec3f firstPos, lastPos;
		KyFloat32 firstAxis, lastAxis;

		if (axisEdgeStart <= axisEdgeEnd)
		{
			firstPos = startEdgePos;
			lastPos  = endEdgePos;
			firstAxis = axisEdgeStart;
			lastAxis  = axisEdgeEnd;
		}
		else
		{
			firstPos = endEdgePos;
			lastPos  = startEdgePos;
			firstAxis = axisEdgeEnd;
			lastAxis  = axisEdgeStart;
		}

		SharedPoolList<BorderEdge>::Iterator currentBorderEdgeOnLeft = m_borderEdgesOnLeft.Begin();
		SharedPoolList<BorderEdge>::Iterator currentBorderEdgeOnRight = m_borderEdgesOnRight.Begin();

		if (edgeIsCrossable)
		{
			for(; currentBorderEdgeOnLeft != m_borderEdgesOnLeft.End();currentBorderEdgeOnLeft++, currentBorderEdgeOnRight++)
			{
				BorderEdge* borderEdgeOnLeft = &(*currentBorderEdgeOnLeft);
				BorderEdge* borderEdgeOnRight = &(*currentBorderEdgeOnRight);

				if (firstAxis >= borderEdgeOnLeft->m_axisEnd_f)
					// this border does not impact the visibility
					continue;

				if (lastAxis <= borderEdgeOnLeft->m_axisStart_f)
					// this border does not impact the visibility so the next won't so
					break; 

				Vec3f startPosForThisBorder = firstPos;
				Vec3f endPosForThisBorder = lastPos;

				if (firstAxis < borderEdgeOnLeft->m_axisStart_f)
					startPosForThisBorder = GetPointAtAxisOnEdge(firstPos, lastPos, borderEdgeOnLeft->m_axisStart_f);

				if (lastAxis > borderEdgeOnLeft->m_axisEnd_f)
					endPosForThisBorder = GetPointAtAxisOnEdge(firstPos, lastPos, borderEdgeOnLeft->m_axisEnd_f);

				KyFloat32 ordinateForStartPosForThisBorder = GetOrdinateValue(startPosForThisBorder);

				bool onLeft = ordinateForStartPosForThisBorder > 0.f;
				if (onLeft)
				{
					if (IsVisibleForBorderLeft(startPosForThisBorder, endPosForThisBorder, borderEdgeOnLeft) == false)
						continue;
				}
				else
				{
					if (IsVisibleForBorderRight(startPosForThisBorder, endPosForThisBorder, borderEdgeOnRight) == false)
						continue;
				}

				return true;
			}

			return false;
		}

		if (firstAxis == lastAxis)
			return false;

		for(; currentBorderEdgeOnLeft != m_borderEdgesOnLeft.End();currentBorderEdgeOnLeft++, currentBorderEdgeOnRight++)
		{
			BorderEdge* borderEdgeOnLeft = &(*currentBorderEdgeOnLeft);
			BorderEdge* borderEdgeOnRight = &(*currentBorderEdgeOnRight);

			KY_ASSERT(borderEdgeOnLeft->m_axisStart_f == borderEdgeOnLeft->m_axisStart_f);
			KY_ASSERT(borderEdgeOnRight->m_axisEnd_f == borderEdgeOnRight->m_axisEnd_f);
			KY_ASSERT(borderEdgeOnLeft->m_axisStart_f != borderEdgeOnLeft->m_axisEnd_f);
			KY_ASSERT(borderEdgeOnRight->m_axisStart_f != borderEdgeOnRight->m_axisEnd_f);

			if (lastAxis <= borderEdgeOnLeft->m_axisStart_f)
				// this border does not impact the visibility so the next won't so
				break;

			if (firstAxis >= borderEdgeOnLeft->m_axisEnd_f)
				// this border does not impact the visibility
				continue;

			Vec3f startPosForThisBorder = firstPos;
			Vec3f endPosForThisBorder = lastPos;

			if (firstAxis < borderEdgeOnLeft->m_axisStart_f)
				startPosForThisBorder = GetPointAtAxisOnEdge(firstPos, lastPos, borderEdgeOnLeft->m_axisStart_f);

			if (lastAxis > borderEdgeOnLeft->m_axisEnd_f)
				endPosForThisBorder = GetPointAtAxisOnEdge(firstPos, lastPos, borderEdgeOnLeft->m_axisEnd_f);

			KyFloat32 ordinateForStartPosForThisBorder = GetOrdinateValue(startPosForThisBorder);
			KyFloat32 ordinateForEndPosForThisBorder = GetOrdinateValue(endPosForThisBorder);

			bool onLeft = ordinateForStartPosForThisBorder > 0.f;

			// check orientation and visibility
			if (onLeft)
			{
				if (firstAxis == axisEdgeStart)
					return false;

				if (IsVisibleForBorderLeft(startPosForThisBorder, endPosForThisBorder, borderEdgeOnLeft) == false)
					continue;
			}
			else
			{
				if (firstAxis == axisEdgeEnd)
					return false;

				if (IsVisibleForBorderRight(startPosForThisBorder, endPosForThisBorder, borderEdgeOnRight) == false)
					continue;
			}

			KyFloat32 snappedFirstAxis = firstAxis;
			KyFloat32 snappedLastAxis = lastAxis;
			BorderEdge* currentBorderEdge = onLeft ? borderEdgeOnLeft : borderEdgeOnRight;
			if (firstAxis > currentBorderEdge->m_axisStart_f && firstAxis < currentBorderEdge->m_axisStart_f + m_axisStepMin)
			{
				KyFloat32 crossProd = CrossProduct_z(firstPos-lastPos, currentBorderEdge->m_startPos-lastPos);
				if ((onLeft && crossProd >= 0.f) || (onLeft == false && crossProd <= 0.f))
				{
					startPosForThisBorder            = currentBorderEdge->m_startPos;
					ordinateForStartPosForThisBorder = currentBorderEdge->m_ordinateStart_f;
					snappedFirstAxis                 = currentBorderEdge->m_axisStart_f;
				}
				else
				{
					Vec3f newStart = GetPointAtAxisOnEdge(firstPos, lastPos, currentBorderEdge->m_axisStart_f);
					KyFloat32 newOrdinate = GetOrdinateValue(newStart);

					if ((onLeft          && newOrdinate > 0.f && CrossProduct_z(m_onStartLeftPos3f - m_onEndLeftPos3f, newStart - m_onEndLeftPos3f) >= 0) ||
						(onLeft == false && newOrdinate < 0.f && CrossProduct_z(m_onEndRightPos3f - m_onStartRightPos3f, newStart - m_onStartRightPos3f) >= 0 ))
					{
						startPosForThisBorder            = newStart;
						ordinateForStartPosForThisBorder = newOrdinate;
						snappedFirstAxis                 = currentBorderEdge->m_axisStart_f;
					}
				}
			}

			if (lastAxis < currentBorderEdge->m_axisEnd_f && lastAxis > currentBorderEdge->m_axisEnd_f - m_axisStepMin)
			{
				KyFloat32 crossProd = CrossProduct_z(firstPos-lastPos, currentBorderEdge->m_endPos-lastPos);
				if ((onLeft && crossProd >= 0.f) || (onLeft == false && crossProd <= 0.f))
				{
					endPosForThisBorder            = currentBorderEdge->m_endPos;
					ordinateForEndPosForThisBorder = currentBorderEdge->m_ordinateEnd_f;
					snappedLastAxis                = currentBorderEdge->m_axisEnd_f;
				}
				else
				{
					Vec3f newEnd = GetPointAtAxisOnEdge(firstPos, lastPos, currentBorderEdge->m_axisEnd_f);
					KyFloat32 newOrdinate = GetOrdinateValue(newEnd);
					if ((onLeft          && newOrdinate > 0.f &&  CrossProduct_z(m_onStartLeftPos3f - m_onEndLeftPos3f, newEnd - m_onEndLeftPos3f) >= 0) ||
						(onLeft == false && newOrdinate < 0.f &&  CrossProduct_z(m_onEndRightPos3f - m_onStartRightPos3f, newEnd - m_onStartRightPos3f) >= 0 ))
					{
						endPosForThisBorder = newEnd;
						ordinateForEndPosForThisBorder = newOrdinate;
						snappedLastAxis = currentBorderEdge->m_axisEnd_f;
					}
				}
			}

			if (snappedFirstAxis > borderEdgeOnLeft->m_axisStart_f)
			{
				const Vec3f splitPosOnLeft  = GetPointAtAxisOnEdge(borderEdgeOnLeft->m_startPos, borderEdgeOnLeft->m_endPos, snappedFirstAxis);
				const Vec3f splitPosOnRight = GetPointAtAxisOnEdge(borderEdgeOnRight->m_startPos, borderEdgeOnRight->m_endPos, snappedFirstAxis);

				const KyFloat32 ordinateSplitOnLeft  = GetOrdinateValue(splitPosOnLeft);
				const KyFloat32 ordinateSplitOnRight = GetOrdinateValue(splitPosOnRight);

				BorderEdge newBorderEdgeLeft = *borderEdgeOnLeft;
				BorderEdge newBorderEdgeRight = *borderEdgeOnRight;

				borderEdgeOnLeft->m_endPos        = splitPosOnLeft;
				borderEdgeOnLeft->m_ordinateEnd_f = ordinateSplitOnLeft;
				borderEdgeOnLeft->m_axisEnd_f     = snappedFirstAxis;

				newBorderEdgeLeft.m_startPos        = splitPosOnLeft;
				newBorderEdgeLeft.m_ordinateStart_f = ordinateSplitOnLeft;
				newBorderEdgeLeft.m_axisStart_f     = snappedFirstAxis;

				borderEdgeOnRight->m_endPos        = splitPosOnRight;
				borderEdgeOnRight->m_ordinateEnd_f = ordinateSplitOnRight;
				borderEdgeOnRight->m_axisEnd_f     = snappedFirstAxis;

				newBorderEdgeRight.m_startPos        = splitPosOnRight;
				newBorderEdgeRight.m_ordinateStart_f = ordinateSplitOnRight;
				newBorderEdgeRight.m_axisStart_f     = snappedFirstAxis;

				currentBorderEdgeOnLeft  = m_borderEdgesOnLeft.InsertAfter( currentBorderEdgeOnLeft,  newBorderEdgeLeft);
				currentBorderEdgeOnRight = m_borderEdgesOnRight.InsertAfter(currentBorderEdgeOnRight, newBorderEdgeRight);

				borderEdgeOnLeft = &(*currentBorderEdgeOnLeft);
				borderEdgeOnRight = &(*currentBorderEdgeOnRight);
			}

			
			if (snappedLastAxis < borderEdgeOnLeft->m_axisEnd_f)
			{
				const Vec3f splitPosOnLeft  = GetPointAtAxisOnEdge(borderEdgeOnLeft->m_startPos, borderEdgeOnLeft->m_endPos, snappedLastAxis);
				const Vec3f splitPosOnRight = GetPointAtAxisOnEdge(borderEdgeOnRight->m_startPos, borderEdgeOnRight->m_endPos, snappedLastAxis);
				const KyFloat32 ordinateSplitOnLeft  = GetOrdinateValue(splitPosOnLeft);
				const KyFloat32 ordinateSplitOnRight = GetOrdinateValue(splitPosOnRight);

				if (onLeft)
				{
					BorderEdge newBorderEdgeOnLeft = *borderEdgeOnLeft;

					newBorderEdgeOnLeft.m_startPos        = startPosForThisBorder;
					newBorderEdgeOnLeft.m_endPos          = endPosForThisBorder;
					newBorderEdgeOnLeft.m_ordinateStart_f = ordinateForStartPosForThisBorder;
					newBorderEdgeOnLeft.m_ordinateEnd_f   = ordinateForEndPosForThisBorder;
					newBorderEdgeOnLeft.m_edgeRawPtr      = edgeRawPtr;
					newBorderEdgeOnLeft.m_axisEnd_f       = snappedLastAxis;

					m_borderEdgesOnLeft.InsertBefore(newBorderEdgeOnLeft, currentBorderEdgeOnLeft);
					borderEdgeOnLeft->m_startPos        = splitPosOnLeft;
					borderEdgeOnLeft->m_axisStart_f     = snappedLastAxis;
					borderEdgeOnLeft->m_ordinateStart_f = ordinateSplitOnLeft;

					BorderEdge newBorderEdgeOnRight      = *borderEdgeOnRight;
					newBorderEdgeOnRight.m_endPos        = splitPosOnRight;
					newBorderEdgeOnRight.m_ordinateEnd_f = ordinateSplitOnRight;
					newBorderEdgeOnRight.m_edgeRawPtr    = edgeRawPtr;
					newBorderEdgeOnRight.m_axisEnd_f     = snappedLastAxis;

					m_borderEdgesOnRight.InsertBefore(newBorderEdgeOnRight, currentBorderEdgeOnRight);

					borderEdgeOnRight->m_startPos        = splitPosOnRight;
					borderEdgeOnRight->m_axisStart_f     = snappedLastAxis;
					borderEdgeOnRight->m_ordinateStart_f = ordinateSplitOnRight;
				}
				else
				{
					BorderEdge newBorderEdgeOnRight = *borderEdgeOnLeft;

					newBorderEdgeOnRight.m_startPos        = startPosForThisBorder;
					newBorderEdgeOnRight.m_endPos          = endPosForThisBorder;
					newBorderEdgeOnRight.m_ordinateStart_f = ordinateForStartPosForThisBorder;
					newBorderEdgeOnRight.m_ordinateEnd_f   = ordinateForEndPosForThisBorder;
					newBorderEdgeOnRight.m_edgeRawPtr      = edgeRawPtr;
					newBorderEdgeOnRight.m_axisEnd_f       = snappedLastAxis;

					m_borderEdgesOnRight.InsertBefore(newBorderEdgeOnRight, currentBorderEdgeOnRight);

					borderEdgeOnRight->m_startPos        = splitPosOnRight;
					borderEdgeOnRight->m_axisStart_f     = snappedLastAxis;
					borderEdgeOnRight->m_ordinateStart_f = ordinateSplitOnRight;


					BorderEdge newBorderEdgeOnLeft      = *borderEdgeOnLeft;
					newBorderEdgeOnLeft.m_endPos        = splitPosOnLeft;
					newBorderEdgeOnLeft.m_ordinateEnd_f = ordinateSplitOnLeft;
					newBorderEdgeOnLeft.m_edgeRawPtr    = edgeRawPtr;
					newBorderEdgeOnLeft.m_axisEnd_f     = snappedLastAxis;

					m_borderEdgesOnLeft.InsertBefore(newBorderEdgeOnLeft, currentBorderEdgeOnLeft);

					borderEdgeOnLeft->m_startPos        = splitPosOnLeft;
					borderEdgeOnLeft->m_axisStart_f     = snappedLastAxis;
					borderEdgeOnLeft->m_ordinateStart_f = ordinateSplitOnLeft;
				}

				return true;
			}

			
			if (onLeft)
			{
				borderEdgeOnLeft->m_startPos        = startPosForThisBorder;
				borderEdgeOnLeft->m_endPos          = endPosForThisBorder;
				borderEdgeOnLeft->m_ordinateStart_f = ordinateForStartPosForThisBorder;
				borderEdgeOnLeft->m_ordinateEnd_f   = ordinateForEndPosForThisBorder;
				borderEdgeOnLeft->m_edgeRawPtr      = edgeRawPtr;
			}
			else
			{
				borderEdgeOnRight->m_startPos        = startPosForThisBorder;
				borderEdgeOnRight->m_endPos          = endPosForThisBorder;
				borderEdgeOnRight->m_ordinateStart_f = ordinateForStartPosForThisBorder;
				borderEdgeOnRight->m_ordinateEnd_f   = ordinateForEndPosForThisBorder;
				borderEdgeOnRight->m_edgeRawPtr      = edgeRawPtr;
			}
		}

		return false;
	}

	void ComputeTriangleCost(const CoordPos64& v0CoordPos, const CoordPos64& v1CoordPos, const CoordPos64& v2CoordPos, KyFloat32& cost)
	{
		const Vec3f v0(v0CoordPos.x * m_integerPrecision, v0CoordPos.y * m_integerPrecision, 0.f);
		const Vec3f v1(v1CoordPos.x * m_integerPrecision, v1CoordPos.y * m_integerPrecision, 0.f);
		const Vec3f v2(v2CoordPos.x * m_integerPrecision, v2CoordPos.y * m_integerPrecision, 0.f);

		cost = 0.f;
		if (Intersections::SegmentVsTriangle2d(m_pathEdgeEndPos3f, m_pathEdgeStartPos3f, v0, v1, v2) == false)
		{

			KyFloat32 ordinate0 = GetOrdinateValue(v0);
			KyFloat32 ordinate1 = GetOrdinateValue(v1);
			KyFloat32 ordinate2 = GetOrdinateValue(v2);

			ordinate0 *= ordinate0;
			ordinate1 *= ordinate1;
			ordinate2 *= ordinate2;

			cost = Kaim::Min(Kaim::Min(ordinate0, ordinate1), ordinate2);
		}
	}

	KyFloat32 GetAxisValue(const Vec3f& pos)
	{
	/*
	//        B                                 D
	//  ------+--------------------------------+---
	//         \                              /
	//          \                           /
	//           \                        /
	//            \                     /
	//             \                  /
	//     ---------+---------------+---
	//             A               C
	*/
	

/*
		KyFloat32 returnValueTest = 0.f;
		switch(m_type)
		{
		case Trapeze :
			{
				const KyFloat32 crossProdAC_AP_f = CrossProduct_z(m_pathEdgeEndPos3f-m_pathEdgeStartPos3f, pos-m_pathEdgeStartPos3f);

				KyFloat32 numerator = CrossProduct_z(m_onStartLeftPos3f-m_pathEdgeStartPos3f, pos-m_pathEdgeStartPos3f) * m_crossProdAC_CD_f;
				KyFloat32 denominator = crossProdAC_AP_f*m_crossProdAB_CD_f + m_crossProdAB_AC_f*m_crossProdAC_CD_f;
				returnValueTest = numerator / denominator;
				break;
			}
		case TriangleAroundRight :
			{
				KyFloat32 numerator = CrossProduct_z(m_onStartRightPos3f-m_pathEdgeStartPos3f, pos-m_pathEdgeStartPos3f);
				KyFloat32 denominator = CrossProduct_z(m_pathEdgeEndPos3f-m_pathEdgeStartPos3f, pos-m_onStartRightPos3f);
				returnValueTest = numerator / denominator;
				break;
			}
		case TriangleAroundLeft :
			{
				KyFloat32 numerator = CrossProduct_z(m_onStartLeftPos3f-m_pathEdgeStartPos3f, pos-m_pathEdgeStartPos3f);
				KyFloat32 denominator = CrossProduct_z(m_pathEdgeEndPos3f-m_pathEdgeStartPos3f, pos-m_onStartLeftPos3f);
				returnValueTest = numerator / denominator;
				break;
			}
		default:
			break;
		}*/

		KyFloat32 numerator = CrossProduct_z(m_onStartLeftPos3f-m_pathEdgeStartPos3f, pos-m_pathEdgeStartPos3f);
		KyFloat32 denominator = CrossProduct_z(m_pathEdgeEndPos3f-m_pathEdgeStartPos3f, pos-m_pathEdgeStartPos3f)*m_axisMultFactor + m_crossProdAB_AC_f;
		return numerator / denominator;
	}

	KyFloat32 GetPointAtAxisOnEdgeRatio(const Vec3f& startEdgePos, const Vec3f& endEdgePos, KyFloat32 wantedAxis)
	{
		const Vec3f AB = m_onStartLeftPos3f - m_pathEdgeStartPos3f;
		const Vec3f AC = m_pathEdgeEndPos3f - m_pathEdgeStartPos3f;
		const Vec3f AP = startEdgePos - m_pathEdgeStartPos3f;
		const Vec3f PQ = endEdgePos - startEdgePos;
		KyFloat32 numerator = CrossProduct_z(AB, AP - wantedAxis * AC) - wantedAxis*m_axisMultFactor*CrossProduct_z(AC,AP);
		KyFloat32 denominator = CrossProduct_z(wantedAxis*m_axisMultFactor*AC - AB, PQ);
		return numerator / denominator;
	}

	Vec3f GetPointAtAxisOnEdge(const Vec3f& startEdgePos, const Vec3f& endEdgePos, KyFloat32 wantedAxis)
	{
		if (endEdgePos == startEdgePos)
			return startEdgePos;

		return startEdgePos + (endEdgePos-startEdgePos) * GetPointAtAxisOnEdgeRatio(startEdgePos, endEdgePos, wantedAxis);
	}

	KyFloat32 GetOrdinateValue(const Vec3f& pos)
	{
		return CrossProduct_z(m_pathEdgeEndPos3f-m_pathEdgeStartPos3f, pos-m_pathEdgeStartPos3f);
	}

	bool BoundEdgeToTrapezoidEdge(Vec3f& startEdgePos, Vec3f& endEdgePos, Vec3f trapezoidEdgeStart, Vec3f trapezoidEdgeEnd)
	{
		KyFloat32 crossProduct1 = CrossProduct_z(trapezoidEdgeEnd - trapezoidEdgeStart, startEdgePos - trapezoidEdgeStart);
		KyFloat32 crossProduct2 = CrossProduct_z(trapezoidEdgeEnd - trapezoidEdgeStart,   endEdgePos - trapezoidEdgeStart);
		if (crossProduct1 > 0.f)
		{
			crossProduct2 = Kaim::Min(crossProduct2, 0.00001f);
			KY_ASSERT(crossProduct2 <= 0.0001f); // epsilon...
			if (crossProduct2 >= 0)
				return false;

			Vec3f newStart;
			Intersections::LineVsLine2d(trapezoidEdgeStart, trapezoidEdgeEnd, startEdgePos, endEdgePos, newStart);
			startEdgePos = newStart;
		}
		else if (crossProduct2 > 0.f)
		{
			KY_ASSERT(crossProduct1 <= 0.0001f); // epsilon...
			if (crossProduct1 >= 0)
				return false;

			Vec3f newEnd;
			Intersections::LineVsLine2d(trapezoidEdgeStart, trapezoidEdgeEnd, startEdgePos, endEdgePos, newEnd);
			endEdgePos = newEnd;
		}

		return true;
	}
	bool BoundEdgeToTrapezoidLimit(Vec3f& startEdgePos, Vec3f& endEdgePos)
	{
		if (BoundEdgeToTrapezoidEdge(startEdgePos, endEdgePos, m_onStartRightPos3f, m_onStartLeftPos3f) == false)
			return false;

		if (BoundEdgeToTrapezoidEdge(startEdgePos, endEdgePos, m_onEndLeftPos3f, m_onEndRightPos3f) == false)
			return false;

		if (m_type == Trapeze || m_type == TriangleAroundRight)
		{
			if (BoundEdgeToTrapezoidEdge(startEdgePos, endEdgePos, m_onStartLeftPos3f, m_onEndLeftPos3f) == false)
				return false;
		}

		if (m_type == Trapeze || m_type == TriangleAroundLeft)
		{
			if (BoundEdgeToTrapezoidEdge(startEdgePos, endEdgePos, m_onEndRightPos3f, m_onStartRightPos3f) == false)
				return false;
		}

		return true;
	}

public:
	Vec3f m_pathEdgeStartPos3f;
	Vec3f m_pathEdgeEndPos3f;

	Vec3f m_onStartLeftPos3f;
	Vec3f m_onEndLeftPos3f;
	Vec3f m_onStartRightPos3f;
	Vec3f m_onEndRightPos3f;

	KyFloat32 m_radius;
	KyInt64 m_radiusInt;

	KyFloat32 m_crossProdAB_AC_f;
	KyFloat32 m_crossProdAB_CD_f;
	KyFloat32 m_crossProdAC_CD_f;
	KyFloat32 m_axisMultFactor;
	KyFloat32 m_distance2d;
	KyFloat32 m_axisStepMin;
	KyFloat32 m_integerPrecision;

	enum VolumeType
	{
		Trapeze,
		TriangleAroundRight,
		TriangleAroundLeft,
	} m_type;


	SharedPoolList<BorderEdge> m_borderEdgesOnLeft;
	SharedPoolList<BorderEdge> m_borderEdgesOnRight;
	SharedPoolList<BorderEdge>::NodePool m_borderEdgePool;
};

}

#endif //Navigation_TrapezoidIntersector_H

