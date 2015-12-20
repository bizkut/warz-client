/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: MAMU - secondary contact: NOBODY

#ifndef Navigation_ChannelTrajectory_Display_H
#define Navigation_ChannelTrajectory_Display_H

#include "gwnavruntime/pathfollower/circlearcsplinedisplay.h"


namespace Kaim
{

class ScopedDisplayList;
class ChannelTrajectoryBlob;
class ChannelArrayBlob;

class ChannelTrajectoryDisplayConfig
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualDebug)

public:
	ChannelTrajectoryDisplayConfig() { SetDefaults(); }

	void SetDefaults();

	bool m_displaySpline;
	bool m_displaySplineRadius;
	bool m_displayPositionOnSpline;
	bool m_displayTargetOnSpline;
	bool m_displayRadiusProfile;

	CircleArcSplineDisplayConfig m_splineDisplayConfig;
	PositionOnCircleArcSplineDisplayConfig m_positionOnSplineDisplayConfig;
	PositionOnCircleArcSplineDisplayConfig m_targetOnSplineDisplayConfig;
};

class ChannelTrajectoryDisplayListBuilder
{
public:
	static void DisplayCurrentChannelSection(ScopedDisplayList* displayList, bool currentChannelIsValid, KyUInt32 currentChannelIdx,
		KyUInt32 currentGateIdx, const ChannelArrayBlob* channelArray);

	static void DisplayChannelTrajectory(ScopedDisplayList* displayList, const ChannelTrajectoryBlob* trajectory, const ChannelTrajectoryDisplayConfig& displayConfig);

private:
	static void RenderClosedPolyline(const BlobArray<Vec3f> &poylineBlob, ScopedDisplayList* displayList, const VisualColor lineColor);
};

} // namespace Kaim

#endif
