/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Authors: GUAL, JUBA
#ifndef Navigation_WorldElement_H
#define Navigation_WorldElement_H


#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/world/worldelementtype.h"
#include "gwnavruntime/math/vec3f.h"

namespace Kaim
{

class World;
class VisualDebugServer;


/// Enumerates the visual debug levels of details.
enum VisualDebugLOD
{
	VisualDebugLOD_None = 0,
	VisualDebugLOD_Default,
	VisualDebugLOD_Maximal,
	VisualDebugLOD_Undefined = KyUInt32MAXVAL
};


enum VisualDebugSendChangeEvent
{
	VisualDebugSendChangeEvent_NoChange = 0,  // Happens most of the time and means you can send your data only when they have changed
	VisualDebugSendChangeEvent_OnLODChanged,  // Happens when the level of detail has changed and means you must send or remove (send a VisualDebugDataRemovalBlob) all data according to the LOD change
	VisualDebugSendChangeEvent_ForceSendAll   // Happens on connection with a visual debug client (eg: NavigationLab) or when WorldElement::ForceSendAllvisualDebug is called. Means you must send all data describing your element for current LOD.
};


/// Base internal class used to represent elements that can be added to a World,
/// such as instances of Database, Bot, BoxObstacle, CylinderObstacle, TagVolume...
class WorldElement : public RefCountBase<WorldElement, Stat_Default_Mem>
{
public:
	// ---------------------------------- Main API Functions ----------------------------------

	WorldElement(World* world = KY_NULL)
		: m_currentVisualDebugLOD(VisualDebugLOD_Undefined)
		, m_previousVisualDebugLOD(VisualDebugLOD_Undefined)
		, m_forceSendAllVisualDebugData(false)
	{ MakeZeroWorldElement(); m_world = world; }

	virtual ~WorldElement() {}

	virtual WorldElementType GetType() const = 0;
	virtual const char*      GetTypeName() const = 0;

	bool IsAddedToWorld()       const { return m_indexInCollection != KyUInt32MAXVAL; }
	World* GetWorld()           const { return m_world; }
	KyUInt32 GetVisualDebugId() const { return m_visualDebugId; }
	void* GetUserData()         const { return m_userData; }

	void SetUserData(void* userData) { m_userData = userData; }


	// ---------------------------------- Visual Debug ----------------------------------

	/// Set the visual debug level of details to use for this WorldElement instance.
	/// Generally, setting this to VisualDebugLOD_Minimal, sends almost nothing but
	/// position and status, whereas setting it to VisualDebugLOD_Maximal sends all
	/// available debug information including spatialization and some internal
	/// algorithm results.
	void SetCurrentVisualDebugLOD(VisualDebugLOD lod);

	VisualDebugLOD GetCurrentVisualDebugLOD() const { return m_currentVisualDebugLOD; }
	VisualDebugLOD GetPreviousVisualDebugLod() const { return m_previousVisualDebugLOD; }
	void SendVisualDebug(VisualDebugServer& server);

	/// Call this when something impacts all your visual debug data. It will set
	/// the event to VisualDebugSendChangeEvent_ForceSendAll. (called internally
	/// when connecting with a VisualDebug client).
	void ForceSendAllVisualDebug();


public: // internal

	// Called at each update on all registered WorldElement. Consider the event
	// to decide what to send.
	virtual void DoSendVisualDebug(VisualDebugServer&, VisualDebugSendChangeEvent) {}
	void SendForMaxLevelOfDetail(VisualDebugServer& server, const Vec3f& shapePosition);

	// visual debug registry
	void RegisterToVisualDebug();
	void UnRegisterFromVisualDebug();

	// for O(1) remove from world collections
	KyUInt32 GetIndexInCollection() const { return m_indexInCollection; }
	void SetIndexInCollection(KyUInt32 indexInCollection) { m_indexInCollection = indexInCollection; }

protected:
	void MakeZeroWorldElement();

protected:
	World* m_world;
	KyUInt32 m_indexInCollection;         // for O(1) remove from world collections
	KyUInt32 m_visualDebugId;                 // TODO: 30 bits is enough merge it into 1 UINT32
	VisualDebugLOD m_currentVisualDebugLOD;   // TODO:  4 bits are enough merge it into 1 UINT32
	VisualDebugLOD m_previousVisualDebugLOD;  // TODO:  4 bits are enough merge it into 1 UINT32
	bool m_forceSendAllVisualDebugData;
	void* m_userData;

friend class VisualDebugRegistry;
};


#ifndef KY_BUILD_SHIPPING
KY_INLINE void WorldElement::SetCurrentVisualDebugLOD(VisualDebugLOD lod) { m_currentVisualDebugLOD = lod; }

#else // VisualDebug is disabled in Shipping mode

KY_INLINE void WorldElement::SetCurrentVisualDebugLOD(VisualDebugLOD) {}

KY_INLINE void WorldElement::RegisterToVisualDebug() {}
KY_INLINE void WorldElement::UnRegisterFromVisualDebug() {}
KY_INLINE void WorldElement::ForceSendAllVisualDebug() {}
KY_INLINE void WorldElement::SendVisualDebug(VisualDebugServer&) {}
KY_INLINE void WorldElement::SendForMaxLevelOfDetail(VisualDebugServer&, const Vec3f&) {}
#endif


}

#endif
