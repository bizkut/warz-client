//=========================================================================
//	Module: CameraSpotsManager.h
//	Copyright (C) 2011.
//=========================================================================

#pragma once

//////////////////////////////////////////////////////////////////////////

class CameraSpotsManager
{
public:
	struct CameraSpotData
	{
		r3dString name;
		r3dVector pos;
		r3dVector up;
		r3dVector target;
	};

	uint32_t GetSpotsCount() const { return spots.Count(); }
	CameraSpotData & GetSpotData(uint32_t idx);
	void AddSpot(const CameraSpotData &spotData);
	void RemoveSpot(uint32_t idx);
	void SaveToXML();
	void CaptureCurrentCamera(const char *name);
	void RestoreCamera(uint32_t idx);
	void RestoreCamera(const char *name);

	static CameraSpotsManager & Instance();

private:
	typedef r3dTL::TArray<CameraSpotData> SpotsData;
	SpotsData spots;

	CameraSpotsManager();
	void LoadFromXML();
	void RestoreCamera(const CameraSpotData &csd);
};
