/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_OBJProducer_H
#define GwNavGen_OBJProducer_H


#include "gwnavgeneration/input/generatorinputproducer.h"


namespace Kaim
{

class ObjFileReader;
class ClientInputConsumer;
class NavTagByString;


/// The OBJProducer class is a concrete implementation of GeneratorInputProducer that can read the triangles contained in a .obj file, and pass it to the ClientInputConsumer.
/// This class is used internally by the standalone NavData generation tools supplied with Gameware Navigation. If you save your
/// terrain geometry in .obj files, you can use this class yourself to create NavData from the triangles in your files. 
class OBJProducer : public GeneratorInputProducer
{
public:
	void SetupCoordSystem(const Kaim::CoordSystemConfig& objCoordSystemConfig) { m_objCoordSystem.Setup(objCoordSystemConfig); }

	virtual ~OBJProducer() {}

	virtual KyResult Produce(const GeneratorSector& sector, ClientInputConsumer& inputConsumer);

protected:
	KyResult ProduceFromObj(const String& inputFileName, ClientInputConsumer& inputConsumer);

public:
	CoordSystem m_objCoordSystem; ///< defines the CoordSystem of the obj files
};


}


#endif

