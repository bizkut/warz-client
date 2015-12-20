/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_GeneratorSectorListBlobBuilder_H
#define GwNavGen_GeneratorSectorListBlobBuilder_H

#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavgeneration/generator/generatorsectorlistblob.h"
#include "gwnavgeneration/generator/generatorguidcompound.h"
#include "gwnavruntime/containers/kyarray.h"

namespace Kaim
{

class GeneratorInputOutput;

/// Used to serialize GeneratorInputOutput m_exclusiveGuids and m_sectors
class GeneratorSectorListBlobBuilder : public BaseBlobBuilder<GeneratorSectorListBlob>
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	GeneratorSectorListBlobBuilder(const GeneratorInputOutput& config) : m_generatorInputOutput(&config) {}

private:
	virtual void DoBuild();
private: 
	const GeneratorInputOutput* m_generatorInputOutput;
};

}


#endif

