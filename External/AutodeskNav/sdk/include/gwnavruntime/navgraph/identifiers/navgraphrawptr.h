/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/



// Primary contact: JODA - secondary contact: JUBA
#ifndef Navigation_NavGraphRawPtr_H
#define Navigation_NavGraphRawPtr_H

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/navgraph/navgraph.h"


namespace Kaim
{

class NavGraphBlob;


///////////////////////////////////////////////////////////////////////////////////////////
// NavGraphPtr
///////////////////////////////////////////////////////////////////////////////////////////

/// This class uniquely identifies a NavGraph that has been loaded.
/// Instances of this class can be safely stored and used in subsequent frames.
/// When you are finished with an instance of this class, call Invalidate() to free its allocated
/// resources. It is important to call Invalidate() before destroying the World.
class NavGraphRawPtr
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavData)

public:
	/// Constructs a new NavGraphRawPtr.
	/// \param navGraph			A reference to the NavGraph we want the NavGraphRawPtr refers to. 
	NavGraphRawPtr(NavGraph* navGraph = KY_NULL);

	/// Returns true if this object refers to a valid NavGraph. 
	bool IsValid() const;

	/// Invalidates this object. 
	void Invalidate();

	/// Returns true if this object identifies the same NavGraph as navGraphPtr, or if both are invalid. 
	bool operator ==(const NavGraphRawPtr& rhs) const;
	bool operator !=(const NavGraphRawPtr& rhs) const;

	bool operator < (const NavGraphRawPtr& rhs) const;
	bool operator <=(const NavGraphRawPtr& rhs) const;


	/// Returns a reference to the NavGraph that handles the NavGraph.
	/// \pre    This object must be valid. Its validity is not checked. 
	NavGraph* GetNavGraph() const;
	const NavGraphBlob* GetNavGraphBlob() const;

private:
	NavGraph* m_navGraph;
};

KY_INLINE NavGraphRawPtr::NavGraphRawPtr(NavGraph* navGraph) : m_navGraph(navGraph) {}

KY_INLINE bool NavGraphRawPtr::IsValid() const { return m_navGraph != KY_NULL; }
KY_INLINE void NavGraphRawPtr::Invalidate() { m_navGraph = KY_NULL; }

KY_INLINE bool NavGraphRawPtr::operator ==(const NavGraphRawPtr& rhs) const { return m_navGraph == rhs.m_navGraph; }
KY_INLINE bool NavGraphRawPtr::operator !=(const NavGraphRawPtr& rhs) const { return !(*this == rhs); }
KY_INLINE bool NavGraphRawPtr::operator < (const NavGraphRawPtr& rhs) const { return m_navGraph < rhs.m_navGraph; }
KY_INLINE bool NavGraphRawPtr::operator <=(const NavGraphRawPtr& rhs) const { return !(rhs < *this); }

KY_INLINE NavGraph* NavGraphRawPtr::GetNavGraph() const { return m_navGraph; }
KY_INLINE const NavGraphBlob* NavGraphRawPtr::GetNavGraphBlob() const { return m_navGraph->GetNavGraphBlob(); }

} // namespace Kaim

#endif // #ifndef Navigation_NavGraphRawPtr_H
