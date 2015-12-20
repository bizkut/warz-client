/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: MAMU - secondary contact: LAPA
#ifndef Navigation_LivePathDisplayListBuilder_H
#define Navigation_LivePathDisplayListBuilder_H

#include "gwnavruntime/base/types.h"
#include "gwnavruntime/math/vec3f.h"
#include "gwnavruntime/visualsystem/visualcolor.h"

namespace Kaim
{

class ScopedDisplayList;
class LivePathBlob;
class LivePathDetailsBlob;
class PositionOnPathBlob;
class PathBlob;

class LivePathParameters
{
public:
	LivePathParameters() { SetDefaults(); }

	void SetDefaults()
	{
		m_pathEventMastHeight = 2.5f;
		m_pathEventFlagHeight = 0.45f;
		m_pathEventRadius = 0.20f;
		m_pathBoundFlagHeight = 0.40f;
		m_pathBoundRadius = 0.25f;
	}

	KyFloat32 m_pathEventMastHeight;
	KyFloat32 m_pathEventFlagHeight;
	KyFloat32 m_pathBoundFlagHeight;
	KyFloat32 m_pathEventRadius;
	KyFloat32 m_pathBoundRadius;
};

class LivePathDisplayListBuilder
{
public:
	/// Calls DisplayPathfinderStatus, DisplayPathEventList, and DisplayPathFinderResult
	static void DisplayLivePath(Kaim::ScopedDisplayList* displayList, const Kaim::LivePathBlob* livePathBlob, const Kaim::PathBlob* pathBlob);

	static void DisplayPathfinderStatus(Kaim::ScopedDisplayList* displayList, const Kaim::LivePathBlob* livePathBlob);

	/// Calls internally DisplayPathAndLivePathBounds
	static void DisplayPathEventList(Kaim::ScopedDisplayList* displayList,
		const Kaim::LivePathDetailsBlob* livePathDetailsBlob,
		LivePathParameters& parameters);

	static void DisplayPathFinderResult(Kaim::ScopedDisplayList* displayList, KyUInt32 pathFinderResult,
		const Kaim::Vec3f& pathFinderStart, const Kaim::Vec3f* pathFinderDest);

	static void DisplayPathAndLivePathBounds(Kaim::ScopedDisplayList* displayList, KyUInt32 pathType,
		KyUInt32 pathValidityStatus, const Kaim::PositionOnPathBlob* backwardValidityBound,
		const Kaim::PositionOnPathBlob* forwardValidityBound, const Kaim::PathBlob* pathBlob,
		LivePathParameters& parameters);
};

} // namespace Kaim

#endif
