/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// Primary contact: JODA - secondary contact: JUBA
#ifndef Navigation_NavGraphPtr_H
#define Navigation_NavGraphPtr_H

#include "gwnavruntime/navgraph/navgraphtypes.h"
#include "gwnavruntime/navgraph/identifiers/navgraphrawptr.h"
#include "gwnavruntime/navgraph/navgraph.h"


namespace Kaim
{

/// This class uniquely identifies a single NavGraph.
/// Instances of this class can be safely stored and used in subsequent frames.
class NavGraphPtr
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)
public:
	/// Constructs a new NavGraphPtr.
	/// \param navGraph    A pointer to the NavGraph wrapped by this object. 
	NavGraphPtr(NavGraph* navGraph = KY_NULL);

	/// Returns true if this object refers to a valid runtime NavGraph, which means a NavFloor that
	/// exists, whose underlying static data (NavGraphBlob) has not been removed from the Database.
	bool IsValid() const;

	void Invalidate(); ///< Invalidates this object.

	/// Returns a pointer to the NavGraph that handles the graph identified by this object.
	/// If this object is invalid, returns KY_NULL.
	NavGraph* GetNavGraph() const;

	/// Returns a pointer to the NavGraph that handles the graph identified by this object.
	/// \pre    This object must be valid. Its validity is not checked.
	NavGraph* GetNavGraph_UnSafe() const;

	/// Returns a pointer to the NavGraphBlob identified by this object.
	/// If this object is invalid, returns KY_NULL.
	const NavGraphBlob* GetNavGraphBlob() const;

	/// Returns a pointer to the NavGraphBlob identified by this object.
	/// \pre    This object must be valid. Its validity is not checked.
	const NavGraphBlob* GetNavGraphBlob_UnSafe() const;

	bool operator ==(const NavGraphPtr& rhs) const; ///< Returns true if this object identifies the same NavGraph as rhs, or if both are invalid. 
	bool operator !=(const NavGraphPtr& rhs) const; ///< Returns true if this object identifies an NavGraph different from the one identified by rhs, or if only one of them is invalid. 
	bool operator < (const NavGraphPtr& rhs) const;
	bool operator <=(const NavGraphPtr& rhs) const;
private :
	mutable Ptr<NavGraph> m_navGraphPtr;
};

KY_INLINE NavGraphPtr::NavGraphPtr(NavGraph* navGraph) : m_navGraphPtr(navGraph) {}
KY_INLINE void NavGraphPtr::Invalidate() { m_navGraphPtr = KY_NULL; }
KY_INLINE bool NavGraphPtr::IsValid() const
{
	if(m_navGraphPtr != KY_NULL)
	{
		if(m_navGraphPtr->IsStillLoaded())
			return m_navGraphPtr->IsActive();

		m_navGraphPtr = KY_NULL;
	}

	return false;
}

KY_INLINE NavGraph* NavGraphPtr::GetNavGraph()        const { return IsValid() ? GetNavGraph_UnSafe() : KY_NULL; }
KY_INLINE NavGraph* NavGraphPtr::GetNavGraph_UnSafe() const { return m_navGraphPtr;                              }

KY_INLINE const NavGraphBlob* NavGraphPtr::GetNavGraphBlob()        const { return IsValid() ? GetNavGraphBlob_UnSafe() : KY_NULL; }
KY_INLINE const NavGraphBlob* NavGraphPtr::GetNavGraphBlob_UnSafe() const { return GetNavGraph_UnSafe()->GetNavGraphBlob();      }

KY_INLINE bool NavGraphPtr::operator ==(const NavGraphPtr& rhs) const { return m_navGraphPtr == rhs.m_navGraphPtr; }
KY_INLINE bool NavGraphPtr::operator !=(const NavGraphPtr& rhs) const { return !(*this == rhs);                        }
KY_INLINE bool NavGraphPtr::operator < (const NavGraphPtr& rhs) const { return m_navGraphPtr < rhs.m_navGraphPtr;  }
KY_INLINE bool NavGraphPtr::operator <=(const NavGraphPtr& rhs) const { return !(rhs < *this);                         }

} // namespace Kaim

#endif // #ifndef Navigation_NavGraphPtr_H
