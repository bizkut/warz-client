/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: LASI - secondary contact: GUAL
#ifndef GwNavGen_GuidCompoundBlobBuilder_H
#define GwNavGen_GuidCompoundBlobBuilder_H

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavruntime/base/guidcompound.h"


namespace Kaim
{

//class GuidCompoundBlobBuilder_trivial : public BaseBlobBuilder<GuidCompound>
//{
//public:
//	GuidCompoundBlobBuilder_trivial(const GuidCompound& guidCompound) :
//		m_guidCompound(&guidCompound)
//	{}
//
//private:
//	virtual void DoBuild();
//
//private:
//	const GuidCompound* m_guidCompound;
//};

class GuidCompoundBlobBuilder : public BaseBlobBuilder<GuidCompound>
{
public:
	GuidCompoundBlobBuilder(const KyGuid* guids, KyUInt32 guidsCount, KyUInt32 timeStamp) :
		m_guids(guids), m_guidsCount(guidsCount), m_timeStamp(timeStamp) {}

private:
	virtual void DoBuild();

	const KyGuid* m_guids;
	KyUInt32 m_guidsCount;
	KyUInt32 m_timeStamp;
};


}


#endif
