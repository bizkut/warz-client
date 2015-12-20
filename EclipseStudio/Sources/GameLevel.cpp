#include "r3dPCH.h"
#include "r3d.h"

#include "GameLevel.h"

namespace r3dGameLevel
{
	static char	HomeDirectory[256];
	static char	SaveDirectory[256];
	float		StartLevelTime = 0;
	bool		EnableGrass = true;

	r3dAtmosphere*	Environment = NULL;

	const char*	GetHomeDir() { return HomeDirectory; }
	const char* GetSaveDir() { return SaveDirectory; }
	bool GetGassEnable() { return EnableGrass; } // Disable o enable Grass on server
	void		SetHomeDir(const char* MapName);
	void		SetGassEnable(const char* MapName, bool enable); // Disable o enable Grass on server
	void		SetStartGameTime(float Time) { StartLevelTime = Time; }

	int IsSaving;
};


void r3dGameLevel :: SetHomeDir(const char* MapName)
{
	sprintf(HomeDirectory,  "Levels\\%s", MapName);
	strcpy( SaveDirectory, HomeDirectory ) ;
	r3dOutToLog("SetHomeDir: %s\n", HomeDirectory);
}

void r3dGameLevel :: SetGassEnable(const char* MapName, bool enable) // Disable o enable Grass on server
{
	EnableGrass = enable;
	/*if (EnableGrass)
		r3dOutToLog("The Grass of: %s is enabled\n", MapName);
	else
		r3dOutToLog("The Grass of: %s is disabled\n", MapName);*/
}

void r3dGameLevel :: SetSaveDir(const char* dir )
{
	strcpy(SaveDirectory,  dir);
	r3dOutToLog("SetSaveDir: %s\n", SaveDirectory);
}
