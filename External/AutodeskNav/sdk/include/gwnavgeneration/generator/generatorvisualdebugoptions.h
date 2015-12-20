/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


// primary contact: GUAL - secondary contact: LASI
#ifndef GwNavGen_GeneratorVisualDebugOptions_H
#define GwNavGen_GeneratorVisualDebugOptions_H

#include "gwnavruntime/base/memory.h"
#include "gwnavruntime/kernel/SF_Std.h"

namespace Kaim
{

class GeneratorVisualDebugOptions
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
public:
	GeneratorVisualDebugOptions()
	{
		SetDefaultValues();
	}

	void SetDefaultValues()
	{
		m_filename[0] = '\0';
		m_isActive = false;
	}

	void Activate(const char* filename = "GeneratorSystem.VisualDebug")
	{
		SFsprintf(m_filename, 256, "%s", filename);
		m_isActive = true;
	}

	void Deactivate() { m_isActive = false; }

	const char* GetFileName() const { return m_filename; }
	bool IsActive() const { return m_isActive; }

private:
	char m_filename[256]; // MAX_PATH
	bool m_isActive; // Can be really deactivated in some condition (multicore)
};


} // namespace Kaim

#endif // GwNavGen_GeneratorVisualDebugOptions_H

