/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_PdgInputProducer_H
#define GwNavGen_PdgInputProducer_H


#include "gwnavruntime/base/memory.h"
#include "gwnavgeneration/input/clientinputconsumer.h"
#include "gwnavgeneration/generator/generatorsector.h"

// a forward declare of ClientInputConsumer would be enough but #include is handy
// to avoid the derivation to #include manually <gwnavgeneration/input/clientinputconsumer.h> 
// and <gwnavgeneration/generator/generatorsector.h>

namespace Kaim
{


class GeneratorSystem;


/// The GeneratorInputProducer is an abstract base class for an object invoked by the NavData generation system to retrieve the geometry
/// for which it should generate NavData.
/// This class cannot be used as-is; you must create your own class that derives from GeneratorInputProducer, and implement the Produce()
/// method.
/// You pass an instance of this class to the Generator for each sector or multi-sector that you want to create. Therefore, each 
/// object that derives from this class is typically responsible for handling a single sector chunk of the geometry in your terrain.
class GeneratorInputProducer : public RefCountBaseNTS<GeneratorInputProducer, MemStat_NavDataGen>
{
public:
	/// Constructor for objects of this class. It should not be necessary to use this constructor directly in your own code. However,
	/// if you write a class that derives directly from this class, you may need to call this constructor from within the constructor
	/// of your derived class. 
	GeneratorInputProducer() {}

	/// Called by the NavData generation system to retrieve the geometry all the input associated with this sector.
	/// Consume sector.m_inputTagVolumes and sector.m_inputSeedPoints before calling Produce().
	KyResult ProduceSectorInputs(const GeneratorSector& sector, ClientInputConsumer& sectorInputConsumer)
	{
		for (UPInt i = 0; i < sector.m_inputTagVolumes.GetSize(); ++i)
			sectorInputConsumer.ConsumeTagVolume(sector.m_inputTagVolumes[i]);

		for (UPInt i = 0; i < sector.m_inputSeedPoints.GetSize(); ++i)
			sectorInputConsumer.ConsumeSeedPoint(sector.m_inputSeedPoints[i]);

		// HeightFields & IndexedMeshes
		for (UPInt i = 0; i < sector.m_inputFileNames.GetSize(); ++i)
		{
			const String& inputFileName = sector.m_inputFileNames[i];
			const String fileNameExtension = inputFileName.GetExtension();

			if (fileNameExtension.CompareNoCase(".HeightField") == 0)
			{
				if (sectorInputConsumer.ConsumeHeightFieldFile(inputFileName) == KY_ERROR)
					return KY_ERROR;
			}
			else if (fileNameExtension.CompareNoCase(".IndexedMesh") == 0)
			{
				if (sectorInputConsumer.ConsumeIndexedMeshFile(inputFileName) == KY_ERROR)
					return KY_ERROR;
			}
		}

		if (Produce(sector, sectorInputConsumer) == KY_ERROR)
			return KY_ERROR;

		return KY_SUCCESS;
	}

private:
	/// Called by the NavData generation system to retrieve the geometry associated to the sector.
	/// Your implementation of this method must push the triangles and NavTags contained in your geometry to
	/// the \c inputConsumer using the methods provided by the ClientInputConsumer class. 
	/// Typically, this means iterating through the triangles in the geometry managed by this object, and making
	/// a call to one of the \c Consume... methods of the ClientInputConsumer for each triangle.
	/// You can also add TagVolumes by calling ClientInputConsumer::ConsumeTagVolume, which
	/// automatically tags all triangles in the specified volume with a specified NavTag.
	/// You can also add SeedPoints by calling ClientInputConsumer::ConsumeSeedPoint, which
	/// automatically tags all triangles in the specified volume with a specified NavTag.
	virtual KyResult Produce(const GeneratorSector& sector, ClientInputConsumer& sectorInputConsumer) = 0;
};
}


#endif

