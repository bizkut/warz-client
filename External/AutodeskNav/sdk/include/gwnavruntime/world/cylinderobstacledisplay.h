/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: LAPA - secondary contact: BRGR
#ifndef KyRuntime_CylinderObstacleDisplay_H
#define KyRuntime_CylinderObstacleDisplay_H

#include "gwnavruntime/visualsystem/visualcolor.h"

namespace Kaim
{

class CylinderObstacleBlob;
class SpatializedCylinderBlob;
class SpatializedCylinderConfigBlob;
class ScopedDisplayList;

class CylinderObstacleDisplayListBuilder
{
public:
	CylinderObstacleDisplayListBuilder() {}

	void DisplayStatus(ScopedDisplayList* displayList,
		const CylinderObstacleBlob* cylinderObstacleBlob,
		const SpatializedCylinderBlob* spatializedCylinderBlob,
		const SpatializedCylinderConfigBlob* spatializedCylinderConfigBlob);
};

} // namespace Kaim

#endif // Navigation_CylinderObstacleDisplay_H
