/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: JAPA - secondary contact: GUAL
#ifndef Navigation_CollisionData_H
#define Navigation_CollisionData_H

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/base/endianness.h"
#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/kernel/SF_String.h"
#include "gwnavruntime/blob/blobaggregate.h"
#include "gwnavruntime/collision/heightfield.h"
#include "gwnavruntime/world/worldelement.h"
#include "gwnavruntime/collision/collisiondatasectordescriptor.h"

namespace Kaim
{

class FileOpenerBase;
class File;
class ICollisionInterface;

/// The CollisionData class represents Collision Data that will be added to a CollisionWorld. Collision Data is
/// comprised of IndexedMeshes (classical Triangle Meshes representing 3D structures) and HeightFields (2D uniform grid of altitudes).
/// The CollisionData class offers a mechanism for loading data from .ColData files created by the Gameware Navigation Generation framework.
/// It should not be used directly, but rather added to a CollisionWorld.

class CollisionData : public WorldElement
{
	KY_CLASS_WITHOUT_COPY(CollisionData)
public:
	static  WorldElementType GetStaticType()       { return TypeColData; }
	virtual WorldElementType GetType()       const { return TypeColData; }
	virtual const char*      GetTypeName()   const { return "ColData";   }

	CollisionData() {}
	~CollisionData();

	void Clear();

	/// Load data from .ColData files created by the Gameware Navigation Generation framework.
	/// Must be called before adding the NavData to its associated CollisionWorld.
	KyResult Load(const char* fileName, FileOpenerBase* fileOpener = KY_NULL);
	KyResult LoadFromMemory(void* memory);

	KyResult Save(const char* fileName, FileOpenerBase* fileOpener = KY_NULL, Endianness::Type endianness = Endianness::BigEndian);

	void AddDescriptorToBlobAggregate();

	KyGuid GetGUID() { return m_descriptor.m_guid; }

private:
	KyResult OnLoaded();

public:
	Ptr<BlobAggregate>      m_blobAggregate;
	Ptr<HeightField>        m_heightfield;
	ColDataSectorDescriptor m_descriptor;
};

}

#endif // Navigation_CollisionData_H
