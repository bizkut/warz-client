/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_GeneratorGuidCompound_H
#define GwNavGen_GeneratorGuidCompound_H


#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/base/guidcompound.h"
#include "gwnavruntime/containers/kyarray.h"


namespace Kaim
{

/// This class is a wrapper around an array of KyGuid objects.
class GeneratorGuidCompound
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	/// Constructs a new GeneratorGuidCompound with an empty KyGuid array.
	GeneratorGuidCompound() : m_timeStamp(0) {}

	/// Constructs GeneratorGuidCompound from guidCompound
	GeneratorGuidCompound(const GuidCompound& guidCompound) : m_timeStamp(0) { InitFromBlob(guidCompound); }

	/// Initialize from guidCompound (blob)
	void InitFromBlob(const GuidCompound& guidCompound);

	/// \returns The first KyGuid in the array, or KyGuid::GetDefaultGuid() if none.
	KyGuid GetMainGuid() const { return m_guids.GetCount() == 0 ?  KyGuid::GetDefaultGuid() : m_guids[0]; }

	/// \return true if this object has more KyGuid objects in its array than the specified GeneratorGuidCompound, or false if it has fewer.
	/// If they have the same number of KyGuid objects, iterates through each KyGuid in the arrays. For internal use. 
	bool operator<(const GeneratorGuidCompound& other) const; // ignore timestamp !

	/// \return true if all GUIDs in the array maintained by this object match the GUIDs in the specified GeneratorGuidCompound. 
	bool AreGuidsEqual(const GeneratorGuidCompound& other) const;

	/// Adds the specified KyGuid to the array maintained by this object. 
	void AddGuid(const KyGuid& guid);

	/// \return true if the specified KyGuid is in the array maintained by this object. 
	bool DoesContain(const KyGuid& guid) const;

	/// Sets the last modified timestamp for this object to the specified value. For internal use. 
	void SetTimeStamp(KyUInt32 timeStamp) { m_timeStamp = timeStamp; }

	/// Sorts the KyGuid objects in the array. For internal use. 
	void Sort();

private:
	bool operator == (const GeneratorGuidCompound&) const; // No implementation
	bool operator != (const GeneratorGuidCompound&) const; // No implementation

public:
	KyArray<KyGuid> m_guids; ///< The array of KyGuid objects. Do not modify directly. 
	KyUInt32 m_timeStamp; ///< The last modification date of this object. Do not modify directly. 
};


}


#endif

