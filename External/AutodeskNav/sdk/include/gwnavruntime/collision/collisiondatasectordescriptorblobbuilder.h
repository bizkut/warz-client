/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: JAPA - secondary contact: GUAL
#ifndef Navigation_SectorDescriptorBlobBuilder_H
#define Navigation_SectorDescriptorBlobBuilder_H

#include "gwnavruntime/collision/collisiondatasectordescriptor.h"
#include "gwnavruntime/blob/baseblobbuilder.h"

namespace Kaim
{

class ColDataSectorDescriptorBlobBuilder : public BaseBlobBuilder<ColDataSectorDescriptorBlob>
{
public:
	ColDataSectorDescriptorBlobBuilder(ColDataSectorDescriptor* sectorDescriptor)
		: m_sectorDescriptor(sectorDescriptor) {}

private:
	virtual void DoBuild();
	ColDataSectorDescriptor* m_sectorDescriptor;
};

}


#endif

