/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_GenFlags_H
#define GwNavGen_GenFlags_H

namespace Kaim
{

namespace GenFlags
{
	/// Describes where the Generator will get its input for each sector
	enum SectorInputSource
	{
		INPUTSOURCE_PRODUCER_WHEN_CHANGED = 0, ///< (default) To get the input of the sector, GeneratorInputProducer is called if SECTOR_CHANGED is set, ClientInput is loaded otherwise
		INPUTSOURCE_PRODUCER              = 1, ///< GeneratorInputProducer is called to get the input of the sector
		INPUTSOURCE_CLIENTINPUT           = 2  ///< ClientInput is loaded to get the input of the sector
	};

	/// Describes what the generation does about a sector.
	/// SECTOR_NOCHANGE and SECTOR_REMOVED are relevant only in iterative generations, i.e. when a generation uses a previous result to re-generate
	/// only minimal NavData depending on the changes.
	enum SectorChange
	{
		/// Load the Sector inputs, generate the Sector NavData or impact the NavDataPatch.
		SECTOR_CHANGED   = 0,

		/// May load the Sector inputs (if required by other CHANGED Sectors), does NOT generate any NavData or NavDataPatch related to this sector.
		SECTOR_NOCHANGE = 1,

		/// Read previous imprint of the Sector, the difference is included in the generated NavDataPatch.
		SECTOR_REMOVED   = 2
	};

	enum GenerationMode
	{
		GENERATION_MODE_INVALID = 0,
		GENERATE_REGULAR_SECTORS,
		GENERATE_PATCHES
	};

	enum RegenerateOnChange
	{
		REGEN_ON_CHANGE_ONLY     = 0, ///
		REGEN_ON_NEIGHBOR_CHANGE = 1  ///
	};

	/// Describes whether or not CollisionData should be saved on a per-sector basis
	enum SectorColDataBuildMode
	{
		SECTOR_COLDATA_BUILD_ENABLED,   ///< The Collision Data will be generated for this sector
		SECTOR_COLDATA_BUILD_DISABLED,  ///< (default) The Collision Data will NOT be generated for this sector
	};
}

}

#endif