/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_PathFinderAbstractGraphMode_H
#define Navigation_PathFinderAbstractGraphMode_H


namespace Kaim
{

/// Enumerates the possible way of dealing with AbstractGraphs.
enum PathFinderAbstractGraphTraversalMode
{
	PATHFINDER_DO_NOT_TRAVERSE_ABSTRACT_GRAPHS       = 0,
	PATHFINDER_TRAVERSE_ABSTRACT_GRAPHS
};

}

#endif //Navigation_PathFinderAbstractGraphMode_H

