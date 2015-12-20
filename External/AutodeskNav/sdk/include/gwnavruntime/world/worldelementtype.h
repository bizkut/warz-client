/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Authors: GUAL
#ifndef Navigation_WorldElementType_H
#define Navigation_WorldElementType_H

namespace Kaim
{

/// Enumerates the WorldElement types.
enum WorldElementType
{
	TypeUnknown = 0,
	TypeBot,
	TypeCylinderObstacle,
	TypeBoxObstacle,
	TypeTagVolume,
	TypePointOfInterest,
	TypeQueryQueueArray,
	TypeDatabase,
	TypeNavData,
	TypeColData,

	WorldElementType_Count
};

}

#endif
