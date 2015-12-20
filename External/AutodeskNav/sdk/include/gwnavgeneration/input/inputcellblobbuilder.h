/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_PdgInputCellBlobBuilder_H
#define GwNavGen_PdgInputCellBlobBuilder_H


#include "gwnavruntime/blob/baseblobbuilder.h"
#include "gwnavgeneration/input/inputcellblob.h"
#include "gwnavruntime/containers/kyarray.h"


namespace Kaim
{

class GeneratorSystem;
class DynamicInputCell;


class InputCellBlobBuilder : public BaseBlobBuilder<InputCellBlob>
{
public:
	InputCellBlobBuilder(GeneratorSystem* sys, const DynamicInputCell& dynInputCell) { m_sys = sys; m_dynInputCell = &dynInputCell; }

private:
	virtual void DoBuild();
	GeneratorSystem* m_sys;
	const DynamicInputCell* m_dynInputCell;
};


class PdgInputCellBlobMerger : public BaseBlobBuilder<InputCellBlob>
{
public:
	PdgInputCellBlobMerger(const KyArrayPOD<const InputCellBlob*>& inputCellBlobs) { m_inputCellBlobs = &inputCellBlobs; }

private:
	virtual void DoBuild();

	const KyArrayPOD<const InputCellBlob*>* m_inputCellBlobs;
};


class InputCellBlobCopier : public BaseBlobBuilder<InputCellBlob>
{
public:
	InputCellBlobCopier(const InputCellBlob* otherInputCellBlob) { m_inputCellBlob = otherInputCellBlob; }

private:
	virtual void DoBuild()
	{
		KyArrayPOD<const InputCellBlob*> dummyArray;
		dummyArray.PushBack(m_inputCellBlob);
		BLOB_BUILD(*m_blob, PdgInputCellBlobMerger(dummyArray));
	}
	const InputCellBlob* m_inputCellBlob;
};

}


#endif
