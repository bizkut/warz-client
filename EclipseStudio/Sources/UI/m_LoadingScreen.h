#ifndef LOADINGSCREEN_H
#define LOADINGSCREEN_H

#include "UIMenu.h"

#include "multiplayer/ClientGameLogic.h"

class LoadingScreen : public UIMenu
{
private:
	r3dTexture*	m_pBackgroundTex;
	bool m_RenderingDisabled;

	Scaleform::GFx::Value m_GFX_Main;
	//Scaleform::GFx::Value m_GFX_Main2;

	char* m_MapName;
	char* m_MapDesc;
	char* m_TipOfTheDay;

	void	updateUIDataText(const char* name, const char* desc, const char* tip);
public:

	LoadingScreen( const char * movieName );
	virtual ~LoadingScreen();

	void	eventRegisterUI(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);

	virtual bool Initialize() OVERRIDE;
	virtual int Update() OVERRIDE;

	void SetLoadingTexture(const char* ImagePath);
	void SetData( const char* ImagePath, const char* Name, const char* Message, const char* tip_of_the_day );
	void SetProgress( float progress );
	void SetRenderingDisabled( bool disabled ) { m_RenderingDisabled = disabled; }

};

void StartLoadingScreen( );
void DisableLoadingRendering( );
void StopLoadingScreen( );
void SetLoadingTexture(const char* ImagePath);
void SetLoadingProgress( float progress );
void AdvanceLoadingProgress( float add );
float GetLoadingProgress();
void SetLoadingPhase( const char* Phase );
int DoLoadingScreen( volatile LONG* loading, const char* LevelName, const char* LevelDescription, const char* LevelFolder, float TimeOut, int gameMode );

template <typename T>
int DoConnectScreen( T* Logic, bool (T::*CheckFunc)(), const char* Message, float TimeOut );

int DoConnectScreen( volatile LONG* loading, const char* Message, float TimeOut );

#define PROGRESS_LOAD_LEVEL_START 0.033f
#define PROGRESS_LOAD_LEVEL_END 0.8f
#define PLAYER_CACHE_INIT_END 0.85f

#endif  //LOADINGSCREEN_H
