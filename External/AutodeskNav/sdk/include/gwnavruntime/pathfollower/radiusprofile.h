/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: LAPA - secondary contact: nobody
#ifndef Navigation_RadiusProfile_H
#define Navigation_RadiusProfile_H

#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/kernel/SF_String.h"


namespace Kaim
{

class RadiusProfileBlob;
class RadiusProfileArrayBlob;


/// RadiusProfile is an array of preferred radii. It is typically used to provide
/// sets of radii to be optimally used when computing a CircleArcSpline.
/// E.g. if your animation system is so that turn animations are optimal when turning
/// around circles with 1m and 3m radii, just push these two values in a RadiusProfile
/// provided to CircleArcSpline computer so that it will use these values wherever
/// possible.
class RadiusProfile
{
public:
	RadiusProfile() { SetDefaults(); }

	RadiusProfile(const char* name)
		: m_name(name)
	{}

	RadiusProfile(const RadiusProfile& other)
		: m_name(other.m_name)
		, m_radiusArray(other.m_radiusArray)
	{}

	~RadiusProfile() { Clear(); }

	void SetDefaults();

	// The new profile should have has many radius has current one.
	// This function is only used in NavigationLab for simple visualdebug and command
	void UpdateFromBlob(const RadiusProfileBlob& blob);

	void        Clear()                         { m_radiusArray.Clear(); }
	void        PushBack(KyFloat32 radius)      { m_radiusArray.PushBack(radius); }

	const char* GetName()                 const { return m_name.ToCStr(); }
	KyUInt32    GetCount()                const { return m_radiusArray.GetCount(); }
	KyFloat32   GetRadius(KyUInt32 index) const { return m_radiusArray[index]; }

	void operator=(const RadiusProfile& other);
	bool operator==(const RadiusProfile& other) const;
	bool operator!=(const RadiusProfile& other) const { return !(*this == other); }

public: // internal
	void SortInIncreasingOrder();

public:
	String m_name;
	KyArray<KyFloat32> m_radiusArray;
};


/// CircleArcSpline computer can compute several CircleArcSplines each corresponding
/// to a specific RadiusProfile. In such case, it mutualizes first steps of the
/// computation. This can be used when you want to select the best pace accordingly
/// to the corresponding splines that can be followed in the upcoming Channel portion.
class RadiusProfileArray
{
public:
	RadiusProfileArray() { SetDefaults(); }
	~RadiusProfileArray() { Clear(); }

	void SetDefaults();

	// The new profileArray should have has many radius profile arrays with each has many radius has current one.
	// This function is only used in NavigationLab for simple visualdebug and command
	void UpdateFromBlob(const RadiusProfileArrayBlob& blob);

	void PushBack(const RadiusProfile& radiusProfile);

	KyUInt32             GetCount()                       const { return m_radiusProfileArray.GetCount(); }
	const RadiusProfile& GetRadiusProfile(KyUInt32 index) const { return m_radiusProfileArray[index]; }
	RadiusProfile&       GetRadiusProfile(KyUInt32 index)       { return m_radiusProfileArray[index]; }

	bool operator==(const RadiusProfileArray& other) const;
	bool operator!=(const RadiusProfileArray& other) const { return !(*this == other); }

private:
	void Clear();

	KyArray<RadiusProfile> m_radiusProfileArray;
	bool m_userDefinedProfile;
};

} // namespace Kaim

#endif // Navigation_RadiusProfile_H
