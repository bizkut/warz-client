/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: LAPA - secondary contact: NOBODY
#ifndef Navigation_TrajectoryDisplay_H
#define Navigation_TrajectoryDisplay_H

#include "gwnavruntime/visualsystem/idisplaylistbuilder.h"

namespace Kaim
{

class TrajectoryBlob;
class BotConfigBlob;
class SpatializedCylinderBlob;
class SpatializedCylinderConfigBlob;
class Vec3f;
class FrontDirectionBlob;

class TrajectoryDisplayListBuilder
{
public:
	static void DisplayTrajectory(
		ScopedDisplayList* displayList, 
		const BotConfigBlob* botConfigBlob, 
		const TrajectoryBlob* trajectoryBlob, 
		const SpatializedCylinderBlob* spatializedCylinderBlob, 
		const SpatializedCylinderConfigBlob* spatializedCylinderConfigBlob, 
		const FrontDirectionBlob* frontDirectionBlob, bool displayWireframe);
};

} // namespace Kaim

#endif // Navigation_TrajectoryDisplay_H
