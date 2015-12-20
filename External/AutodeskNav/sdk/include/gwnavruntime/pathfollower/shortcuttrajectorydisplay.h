/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: MAMU - secondary contact: NOBODY

#ifndef Navigation_ShortcutTrajectory_Display_H
#define Navigation_ShortcutTrajectory_Display_H

namespace Kaim
{

class ScopedDisplayList;
class ShortcutTrajectoryBlob;
class SpatializedCylinderBlob;
class SpatializedCylinderConfigBlob;

class ShortcutTrajectoryDisplayListBuilder
{
public:
	static void DisplayTarget(ScopedDisplayList* displayList, const ShortcutTrajectoryBlob* trajectory,
		const SpatializedCylinderBlob* spatializedCylinder, const SpatializedCylinderConfigBlob* spatializedCylinderConfig);
};

} // namespace Kaim

#endif
