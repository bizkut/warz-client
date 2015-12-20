/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// Primary contact: JUBA - secondary contact: LASI (NavCellCleaner_BlobBuilder)
#ifndef Navigation_NavCellBlobBuilder_H
#define Navigation_NavCellBlobBuilder_H

#include "gwnavruntime/blob/baseblobbuilder.h"

namespace Kaim
{

class DynamicNavCell;
class NavCellBlob;
class NavTag;
template<class T>
class KyArrayTLS_POD;

class NavCellBlobBuilder : public BaseBlobBuilder<NavCellBlob>
{
public:
	NavCellBlobBuilder(const DynamicNavCell& dynamicNavCell) :
		m_dynamicNavCell(&dynamicNavCell){}

private:
	virtual void DoBuild();

private:
	const DynamicNavCell* m_dynamicNavCell;
};


} // namespace Kaim


#endif // Navigation_NavCellBlobBuilder_H
