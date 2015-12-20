/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_KyGuid_H
#define Navigation_KyGuid_H

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/base/endianness.h"
#include "gwnavruntime/blob/blobtypes.h"
#include "gwnavruntime/navdata/navdatablobcategory.h"


namespace Kaim
{

/// The KyGuid class represents a globally unique ID. It is used by the NavData generation framework to uniquely
/// identify each sector treated by the Generator.
class KyGuid
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_ROOT_BLOB_CLASS(NavData, KyGuid, 0)
public:
	static const KyGuid& GetDefaultGuid(); ///< Returns the default KyGuid value
	static const KyGuid& GetInvalidGuid(); ///< Returns the invalid KyGuid value.

	/// Creates an invalid KyGuid.
	KyGuid();

	/// Creates a KyGuid from guidString.
	/// Asserts when guidString is not valid (see IsValidGuidString()).
	/// \param guidString		An array of 37 characters that will make up the GUID. 
	explicit KyGuid(const char guidString[/*37*/]);

	/// Creates a new KyGuid copying the specified KyGuid.
	KyGuid(const KyGuid& rhs) { *this = rhs; }

	/// Creates a new KyGuid from the 16 provided KyUInt8 values.
	/// \param values			An array of 16 KyUInt8 objects that will make up the GUID. 
	explicit KyGuid(KyUInt8 values[16]);

	/// Creates a new KyGuid from the 8 provided KyUInt16 values.
	/// \param values			An array of 8 KyUInt16 objects that will make up the GUID. 
	explicit KyGuid(KyUInt16 values[8]);

	/// Creates a new KyGuid from the 4 provided KyUInt32 values.
	/// \param values			An array of 4 KyUInt32 objects that will make up the GUID. 
	explicit KyGuid(KyUInt32 values[4]);

	/// Creates a new KyGuid from the 2 provided KyUInt64 values.
	/// \param values			An array of 2 KyUInt64 objects that will make up the GUID. 
	explicit KyGuid(KyUInt64 values[2]);

	/// Creates a new KyGuid, copying the memory from the src buffer to the newly constructed object.
	explicit KyGuid(void* src); //will memcpy from src to &m_uuid on sizeof(m_uuid)

	KyGuid(KyUInt32 a, KyUInt32 b, KyUInt32 c, KyUInt32 d);
	void Init(KyUInt32 a, KyUInt32 b, KyUInt32 c, KyUInt32 d);

	bool operator < (const KyGuid& rhs) const { return memcmp(m_uuid, rhs.m_uuid, sizeof (m_uuid)) < 0; }
	bool operator <=(const KyGuid& rhs) const { return memcmp(m_uuid, rhs.m_uuid, sizeof (m_uuid)) <= 0; }
	bool operator > (const KyGuid& rhs) const { return !operator<=(rhs); }
	bool operator >=(const KyGuid& rhs) const { return !operator<(rhs); }
	bool operator ==(const KyGuid& rhs) const { return memcmp(m_uuid, rhs.m_uuid, sizeof (m_uuid)) == 0; }
	bool operator !=(const KyGuid& rhs) const { return !operator==(rhs); }

	KyGuid& operator = (const KyGuid& rhs) { memcpy(m_uuid, rhs.m_uuid, sizeof (m_uuid)); return *this; }

	/// Indicates whether the specified KyGuid is valid. 
	bool IsValid() const { return *this != GetInvalidGuid(); }

	/// Converts this GUID to a string.
	void ToString(char guidString[/*37*/]) const;

	/// Initializes this GUID using the specified string.
	void InitFromString(const char guidString[/*37*/]);


private:
	// set [... a, b, c, d ...] a = value_most_significant_byte d = value_least_significant_byte
	//          |
	//        idx*4     idx must be in [0..3]
	void SetUInt32(KyUInt32 idx, KyUInt32 value);

	// set [... a, b, ...] a = value_most_significant_byte d = value_least_significant_byte
	//          |
	//        idx*2     idx must be in [0..7]
	void SetUInt16(KyUInt32 idx, KyUInt16 value);

private:
	KyUInt8 m_uuid[16];
};

inline void SwapEndianness(Endianness::Target /*e*/, KyGuid& /*self*/)
{
	//no swap needed on guid
}

/// \return true if guidStr is compounded of 36 char and is null-terminated.
bool IsValidGuidString(const char guidStr[/*37*/]);

template <class OSTREAM>
inline OSTREAM& operator<<(OSTREAM& os, const KyGuid& guid)
{
	char guidString[37];
	guid.ToString(guidString);
	os << guidString;
	return os;
}


} // namespace Kaim

#endif
