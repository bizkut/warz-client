/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_BotDisplay_H
#define Navigation_BotDisplay_H

#include "gwnavruntime/visualsystem/visualcolor.h"
#include "gwnavruntime/world/bot.h"

namespace Kaim
{

class ScopedDisplayList;
class BotConfigBlob;
class LivePathBlob;
class PathFollowerBlob;
class SpatializationResultBlob;
class SpatializedCylinderConfigBlob;
class SpatializedCylinderBlob;
class PositionOnLivePathBlob;
class FrontDirectionBlob;
class TrajectoryBlob;


/// Defines the colors used to render the Bot accordingly to its internal
/// statuses. Possible values are:
/// - Red     =>  m_botColor_PathNotFound         =>  No Path has been found for this Bot.
/// - Orange  =>  m_botColor_ComputingPath        =>  Path is under computation.
/// - Lime    =>  m_botColor_NoPathSet            =>  No Path is set nor under computation (the Bot idles).
/// - Yellow  =>  m_botColor_FullPathFollower     =>  The Bot computes all in the PathFollower.
/// - Blue    =>  m_botColor_CustomFollowing      =>  The at least one component of path following is done by the client.
class BotColors
{
public:
	BotColors()
	{
		m_botColor_PathNotFound        = VisualColor::Red;
		m_botColor_ComputingPath       = VisualColor::Orange;
		m_botColor_NoPathSet           = VisualColor::Lime;
		m_botColor_FullPathFollower    = VisualColor::Yellow;
		m_botColor_CustomFollowing     = VisualColor::Blue;
	}

	// - path status
	VisualColor m_botColor_PathNotFound;
	VisualColor m_botColor_ComputingPath;
	VisualColor m_botColor_NoPathSet;
	// - pathfollower status
	VisualColor m_botColor_FullPathFollower;
	VisualColor m_botColor_CustomFollowing;
};

class BotDisplayListBuilder
{
public:
	static VisualColor GetBotRenderColor(const PathFollowerBlob* pathFollowerBlob, const LivePathBlob* livePathBlob, BotColors& botColors);
	static VisualColor GetProgressOnPathColor(const ProgressOnPathStatus status);

	static void DisplayProgressOnPath(ScopedDisplayList* displayList, const SpatializedCylinderBlob* spatilizedCylinderBlob,
		const SpatializedCylinderConfigBlob* bodyBlob, const PathFollowerBlob* pathFollowerBlob, bool isLodMax);

	static void DisplayPositionOnPath(ScopedDisplayList* displayList, const SpatializedCylinderBlob* spatilizedCylinderBlob,
		const SpatializedCylinderConfigBlob* spatializedCylinderConfigBlob, const PositionOnLivePathBlob& positionOnLivePathBlob, VisualColor lineColor, bool isLodMax);

	static void DisplayStatus(ScopedDisplayList* displayList, const BotConfigBlob* botBlob, const SpatializedCylinderBlob* spatilizedCylinderBlob,
		const SpatializedCylinderConfigBlob* bodyBlob,const PathFollowerBlob* pathFollowerBlob,
		const SpatializationResultBlob* spatializationResultBlob, const Kaim::LivePathBlob* livePathBlob);

	static void DisplayFrontDirection(ScopedDisplayList* displayList, const FrontDirectionBlob* frontDirectionBlob, const SpatializedCylinderBlob* spatializedCylinderBlob, const SpatializedCylinderConfigBlob* spatializedCylinderConfigBlob, bool displayWireframe);

};

} // namespace Kaim

#endif // Navigation_BotDisplay_H
