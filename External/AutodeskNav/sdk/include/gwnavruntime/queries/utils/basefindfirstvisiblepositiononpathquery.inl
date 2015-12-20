/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: LAPA - secondary contact: NOBODY


namespace Kaim
{

KY_INLINE BaseFindFirstVisiblePositionOnPathQuery::BaseFindFirstVisiblePositionOnPathQuery() :
	m_result(FINDFIRSTVISIBLEPOSITIONONPATH_NOT_INITIALIZED),
	m_costToVisibleTargetOnPath(0.f)
{}

KY_INLINE void BaseFindFirstVisiblePositionOnPathQuery::SetVisibilityStartTrianglePtr(const NavTrianglePtr& visibilityStartTrianglePtr) { m_visibilityStartTrianglePtr = visibilityStartTrianglePtr; }
KY_INLINE void BaseFindFirstVisiblePositionOnPathQuery::SetSamplingDistance(KyFloat32 samplingDistance)                                 { m_samplingDistance           = samplingDistance;           }
KY_INLINE void BaseFindFirstVisiblePositionOnPathQuery::SetRayCanGoMarginMode(RayCanGoMarginMode rayCanGoMarginMode)                    { m_rayCanGoMarginMode         = rayCanGoMarginMode;         }
KY_INLINE void BaseFindFirstVisiblePositionOnPathQuery::SetResult(FindFirstVisiblePositionOnPathQueryResult result)                     { m_result                     = result;                     }

KY_INLINE FindFirstVisiblePositionOnPathQueryResult BaseFindFirstVisiblePositionOnPathQuery::GetResult() const { return m_result; }

KY_INLINE const PositionOnPath& BaseFindFirstVisiblePositionOnPathQuery::GetStartPositionOnPath()          const { return m_startPositionOnPath;        }
KY_INLINE const PositionOnPath& BaseFindFirstVisiblePositionOnPathQuery::GetEndPositionOnPath()            const { return m_endPositionOnPath;          }
KY_INLINE const Vec3f&          BaseFindFirstVisiblePositionOnPathQuery::GetVisibilityStartPoint()         const { return m_visibilityStartPos3f;       }
KY_INLINE const NavTrianglePtr& BaseFindFirstVisiblePositionOnPathQuery::GetVisibilityStartTrianglePtr()   const { return m_visibilityStartTrianglePtr; }
KY_INLINE KyFloat32             BaseFindFirstVisiblePositionOnPathQuery::GetSamplingDistance()             const { return m_samplingDistance;           }
KY_INLINE RayCanGoMarginMode    BaseFindFirstVisiblePositionOnPathQuery::GetRayCanGoMarginMode()           const { return m_rayCanGoMarginMode;         }
KY_INLINE const PositionOnPath& BaseFindFirstVisiblePositionOnPathQuery::GetFirstVisiblePositionOnPath()   const { return m_firstVisiblePositionOnPath; }
KY_INLINE KyFloat32             BaseFindFirstVisiblePositionOnPathQuery::GetCostToVisiblePositionOnPath() const { return m_costToVisibleTargetOnPath;  }

} // namespace Kaim
