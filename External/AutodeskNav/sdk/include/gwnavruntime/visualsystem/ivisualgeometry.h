/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/




// primary contact: GUAL - secondary contact: MAMU
#ifndef Navigation_IVisualGeometry_H
#define Navigation_IVisualGeometry_H

#include "gwnavruntime/kernel/SF_RefCount.h"
#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/visualsystem/visualgeometrysetupconfig.h"
#include "gwnavruntime/basesystem/coordsystem.h"


namespace Kaim
{

class VisualTriangle;
class VisualLine;
class VisualText;

/// TODO Update
/// IVisualGeometry is an abstract base class for an object that accepts triangles, lines and texts from a class
/// that that derives from IVisualGeometryBuilder, and has be initially designed to manage the rendering of those
/// triangles/lines/texts in the game engine.
/// This class cannot be used as-is. You must write a custom implementation of this class that implements
/// the pure virtual metrhods.
class IVisualGeometry : public RefCountBase<IVisualGeometry, MemStat_VisualSystem>
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_VisualSystem)
public:
	IVisualGeometry() {}
	virtual ~IVisualGeometry() {}

	/// This method is called by the IVisualGeometryBuilder when it begins the process of passing
	/// triangles/lines/texts to this object. You have to implement this method to carry out any initialization
	/// steps necessary  in order to begin the process of managing the triangles/lines/texts that will be passed to
	/// DoPushTriangle/DoPushLine/DoPushText. 
	virtual void DoBegin(const VisualGeometrySetupConfig& setupConfig) = 0;

	virtual void DoPushTriangle(const VisualTriangle& triangle) = 0;
	virtual void DoPushLine(const VisualLine& visualLine) = 0;
	virtual void DoPushText(const VisualText& visualText) = 0;

	/// This method is called by the IVisualGeometryBuilder when the passing of triangles/lines/texts
	/// has finished
	virtual void DoEnd() = 0;

public:
	//m_coordSystem is used to convert Navigation positions into the the visualGeometry coordinate system.
	//So do not forget to set it if you are not in classic Z-up, 1m, ...
	CoordSystem m_coordSystem;
	bool m_coordSystemIsRightHanded; ///< Indicates whether or not the CoordSystem is RightHanded. Updated automatically by the IVisualGeometryBuilder.
};


/// IVisualGeometryFactory is an abstract base class for an object that accepts that can instantiate objects of your
/// own implementation of IVisualGeometry
/// This class cannot be used as-is. You must write a custom implementation of this class that implements
/// the pure virtual methods.
/// This class is used in the context of tile base database rendering. Each time a new tile is created, a new IVisualGeometry
/// will be created with this factory
/// During the clearing of the database, OnDatabaseClear() is called just before all the refcount of each IVisualGeometryFactory is 
/// decremented and the database do not refer in any way the IVisualGeometry created by CreateIVisualGeometryForDatabase.
class IVisualGeometryFactory : public RefCountBase<IVisualGeometry, MemStat_VisualSystem>
{
public:
	virtual ~IVisualGeometryFactory() {}
	virtual Kaim::Ptr<IVisualGeometry> CreateIVisualGeometryForDatabase(KyUInt32 databaseIdx) = 0;
	virtual void OnDatabaseClear(KyUInt32 databaseIdx) = 0;
};

}

#endif
