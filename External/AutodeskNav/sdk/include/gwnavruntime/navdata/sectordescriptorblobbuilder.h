/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: LASI
#ifndef GwNavGen_SectorDescriptorBlobBuilder_H
#define GwNavGen_SectorDescriptorBlobBuilder_H

#include "gwnavruntime/navdata/sectordescriptor.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/kernel/SF_RefCount.h"

namespace Kaim
{

class SectorDescriptorBlobBuilder : public BaseBlobBuilder<SectorDescriptorBlob>
{
public:
	SectorDescriptorBlobBuilder(SectorDescriptor* sectorDescriptor)
		: m_sectorDescriptor(sectorDescriptor) {}

private:
	virtual void DoBuild();
	SectorDescriptor* m_sectorDescriptor;
};

}


#endif

