/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: JODA
#ifndef Navigation_GuidCompound_H
#define Navigation_GuidCompound_H

#include "gwnavruntime/base/kyguid.h"
#include "gwnavruntime/blob/blobarray.h"

namespace Kaim
{

class GuidCompound
{
	KY_CLASS_WITHOUT_COPY(GuidCompound)
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	GuidCompound() {}

	KY_INLINE const KyGuid& GetMainGuid() const { return m_guids.GetCount() == 0 ? KyGuid::GetDefaultGuid() : m_guids.GetValues()[0]; }

	bool operator<(const GuidCompound& other) const;

	bool AreGuidsEqual(const GuidCompound& other) const;

	bool operator==(const GuidCompound& other) const { return AreGuidsEqual(other) && m_timeStamp == other.m_timeStamp; }

	bool operator!=(const GuidCompound& other) const { return !operator==(other); }

	void Sort();

	KY_INLINE KyUInt32 GetKyGuidCount() const { return m_guids.GetCount(); }

	KY_INLINE const KyGuid& GetKyGuid(KyUInt32 index) const { return m_guids.GetValues()[index]; }

	bool DoesContainAllGuidsOfGuidCompound(const Kaim::GuidCompound& other) const;

public:
	BlobArray<KyGuid> m_guids;
	KyUInt32 m_timeStamp;
};

inline void SwapEndianness(Endianness::Target e, GuidCompound& self)
{
	SwapEndianness(e, self.m_guids);
	SwapEndianness(e, self.m_timeStamp);
}

} // namespace Kaim

#endif
