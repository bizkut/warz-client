/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


//  Primary contact: GUAL - secondary contact: JUBA
#ifndef Navigation_DatabaseVisualGeometryBuilder_H
#define Navigation_DatabaseVisualGeometryBuilder_H


#include "gwnavruntime/visualsystem/ivisualgeometrybuilder.h"
#include "gwnavruntime/kernel/SF_Debug.h"
#include "gwnavruntime/navmesh/navmeshtypes.h"


namespace Kaim
{

class Database;

class DatabaseVisualGeometryBuilder : public IVisualGeometryBuilder
{
public:
	DatabaseVisualGeometryBuilder(Database* database= KY_NULL);

	virtual void DoBuild();

private:
	void BuildActiveDataGeometry();
	void BuildAbstractGraphGeometry();

public:
	Database* m_database;
	CellBox m_cellBox;

public: // Internal. For use in the DatabaseVisRepGrid
	bool m_doBuildNavMeshVisRep;
	bool m_doBuildNavGraphVisRep;
	bool m_doBuildAbstractGraphVisRep;
};

}


#endif //Navigation_DatabaseVisualGeometryBuilder_H
