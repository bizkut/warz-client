//=========================================================================
//	Module: CameraSpotsManager.cpp
//	Copyright (C) 2011.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "../GameLevel.h"
#include "XMLHelpers.h"
#include "../UI/HUD_Base.h"

#include "CameraSpotsManager.h"

extern r3dCamera gCam;
extern BaseHUD *Hud;


//-------------------------------------------------------------------------
//	Standalone helper functions
//-------------------------------------------------------------------------
namespace
{
	/**	Construct XML path of camera spots data for current level. */
	r3dString ConstructXMLPath( bool forSaving )
	{
		return r3dString( forSaving ? r3dGameLevel::GetSaveDir() : r3dGameLevel::GetHomeDir() ) + "/camera_spots.xml";
	}
	
} // unnamed namespace

//-------------------------------------------------------------------------
//	CameraSpotsManager impl
//-------------------------------------------------------------------------
CameraSpotsManager::CameraSpotsManager()
{
	LoadFromXML();
}

//////////////////////////////////////////////////////////////////////////

CameraSpotsManager::CameraSpotData & CameraSpotsManager::GetSpotData(uint32_t idx)
{
	if (idx >= spots.Count())
	{
		static CameraSpotData dummy;
		return dummy;
	}

	return spots[idx];
}

//////////////////////////////////////////////////////////////////////////

void CameraSpotsManager::AddSpot(const CameraSpotsManager::CameraSpotData &spotData)
{
	spots.PushBack(spotData);
}

//////////////////////////////////////////////////////////////////////////

void CameraSpotsManager::RemoveSpot(uint32_t idx)
{
	if (idx >= spots.Count())
		return;

	spots.Erase(idx);
}

//////////////////////////////////////////////////////////////////////////

CameraSpotsManager & CameraSpotsManager::Instance()
{
	static CameraSpotsManager c;
	return c;
}

//////////////////////////////////////////////////////////////////////////

void CameraSpotsManager::LoadFromXML()
{
	r3dString path = ConstructXMLPath( false );
	r3dFile* f = r3d_open(path.c_str(), "r");
	if (!f) return;

	r3dTL::TArray<char> fileBuffer(f->size + 1);

	fread(&fileBuffer[0], f->size, 1, f);
	fileBuffer[f->size] = 0;

	pugi::xml_document xmlDoc;
	pugi::xml_parse_result parseResult = xmlDoc.load_buffer_inplace(&fileBuffer[0], f->size);
	fclose(f);
	if (!parseResult) return;

	pugi::xml_node xmlSpotData = xmlDoc.child("camera_spot");

	while (!xmlSpotData.empty())
	{
		CameraSpotData csd;
		csd.name = xmlSpotData.attribute("name").value();
		GetXMLVal("pos", xmlSpotData, &csd.pos);
		GetXMLVal("up", xmlSpotData, &csd.up);
		GetXMLVal("target", xmlSpotData, &csd.target);
		AddSpot(csd);

		xmlSpotData =  xmlSpotData.next_sibling();
	}
}

//////////////////////////////////////////////////////////////////////////

void CameraSpotsManager::SaveToXML()
{
	pugi::xml_document xmlDoc;

	for (uint32_t i = 0; i < spots.Count(); ++i)
	{
		CameraSpotData &csd = spots[i];

		pugi::xml_node xmlSpot = xmlDoc.append_child();
		xmlSpot.set_name("camera_spot");
		xmlSpot.append_attribute("name") = csd.name.c_str();
		
		SetXMLVal("pos", xmlSpot, &csd.pos);
		SetXMLVal("up", xmlSpot, &csd.up);
		SetXMLVal("target", xmlSpot, &csd.target);
	}

	xmlDoc.save_file(ConstructXMLPath( true ).c_str());
}

//////////////////////////////////////////////////////////////////////////

void CameraSpotsManager::CaptureCurrentCamera(const char *name)
{
	if (!Hud)
		return;

	CameraSpotsManager::CameraSpotData csd;
	csd.name = name;
	csd.pos = Hud->FPS_Position;
	csd.target = Hud->GetCameraDir();
	AddSpot(csd);
}

//////////////////////////////////////////////////////////////////////////

void CameraSpotsManager::RestoreCamera(const char *name)
{
	struct CamSearcher
	{
		const char *name;
		explicit CamSearcher(const char *n)
		: name(n)
		{}

		bool operator ()(const CameraSpotData &csd)
		{
			if (!name)
				return false;
			return !!(csd.name == name);
		}
	};

	const CameraSpotData *start = &spots[0];
	const CameraSpotData *end = &spots[spots.Count() - 1] + 1;

	const CameraSpotData *data = std::find_if(start, end, CamSearcher(name));
	if (end != data)
	{
		RestoreCamera(*data);
	}
}

//////////////////////////////////////////////////////////////////////////

void CameraSpotsManager::RestoreCamera(uint32_t idx)
{
	if (idx >= spots.Count())
		return;

	CameraSpotData &csd = spots[idx];
	RestoreCamera(csd);
}

//////////////////////////////////////////////////////////////////////////

void CameraSpotsManager::RestoreCamera(const CameraSpotData &csd)
{
	if (!Hud)
		return;
	Hud->FPS_Position = csd.pos;
	Hud->SetCameraDir(csd.target);
}