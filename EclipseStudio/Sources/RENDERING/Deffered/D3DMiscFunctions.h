#pragma once

enum AntiCheatType
{
	ANTICHEAT_WALLHACK,
	ANTICHEAT_SCREEN_HELPERS2,
	ANTICHEAT_COUNT
};

void issueD3DAntiCheatCodepath( AntiCheatType anticheatType );
void UpdateD3DAntiCheat();
void UpdateD3DAntiCheatPrePresent();
void UpdateD3DAntiCheatPostPresent();
void AnticheatOnDeviceReset();

IDirect3DTexture9*	GetCheatScreenshot();
void				ReleaseCheatScreenshot();