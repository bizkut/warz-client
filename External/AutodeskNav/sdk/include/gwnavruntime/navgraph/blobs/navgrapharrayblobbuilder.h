/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// Primary contact: LASI - secondary contact: NONE
#ifndef Navigation_NavGraphArrayBlobBuilder_H
#define Navigation_NavGraphArrayBlobBuilder_H

#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/navgraph/blobs/navgrapharray.h"

namespace Kaim
{

class NavData;

class NavGraphArrayBlobBuilder : public BaseBlobBuilder<NavGraphArray>
{
public:
	NavGraphArrayBlobBuilder(const NavData* navData) : m_navData(navData) {};

private:
	virtual void DoBuild();

private:
	const NavData* m_navData;
};


}

#endif // Navigation_NavGraphArrayBlobBuilder_H
