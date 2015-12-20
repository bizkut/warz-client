/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_GeneratorNavDataElementMap_H
#define GwNavGen_GeneratorNavDataElementMap_H


#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/base/memory.h"


namespace Kaim
{

class GeneratorNavDataElement;
class GeneratorNavDataElementMap_Implementation;
class GeneratorGuidCompound;

// GeneratorNavDataElementMap allows
// - to get a GeneratorNavDataElement from a guidCompound in O(log(N))
// - to get all the GeneratorNavDataElements in an array
// GeneratorNavDataElementMap is used as a member in GeneratorSectorBuilder and in GeneratorSystem
// using a std::map hidden in GeneratorNavDataElementMap_Implementation in the cpp
class GeneratorNavDataElementMap
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)
	KY_CLASS_WITHOUT_COPY(GeneratorNavDataElementMap)
public:
	GeneratorNavDataElementMap();
	~GeneratorNavDataElementMap();

	void Clear();

	KyResult Add(GeneratorNavDataElement& navDataElement);

	GeneratorNavDataElement* Get(const GeneratorGuidCompound& guidCompound) const;

	const KyArrayPOD<GeneratorNavDataElement*>& GetArray() const;

private:
	GeneratorNavDataElementMap_Implementation* m_impl;
};


}


#endif

