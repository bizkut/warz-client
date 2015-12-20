#include "r3dPCH.h"
#include "r3d.h"

#include "m_LoadingScreen.h"
#include "GameCode\UserProfile.h"

#include "Multiplayer\MasterServerLogic.h"

#include "LangMngr.h"

#include "r3dDeviceQueue.h"

LoadingScreen::LoadingScreen( const char * movieName ) 
: UIMenu(movieName) 
,m_MapName(NULL)
,m_MapDesc(NULL)
,m_TipOfTheDay(NULL)
{
	m_pBackgroundTex = 0;
	m_RenderingDisabled = false;
}

//------------------------------------------------------------------------

LoadingScreen::~LoadingScreen()
{
	if(m_pBackgroundTex)
		r3dRenderer->DeleteTexture(m_pBackgroundTex);
	m_pBackgroundTex = 0;

	m_GFX_Main.SetUndefined();
	//m_GFX_Main2.SetUndefined();

	SAFE_DELETE_ARRAY(m_MapName);
	SAFE_DELETE_ARRAY(m_MapDesc);
	SAFE_DELETE_ARRAY(m_TipOfTheDay);
}

//------------------------------------------------------------------------

bool LoadingScreen::Initialize()
{
#define MAKE_CALLBACK(FUNC) game_new r3dScaleformMovie::TGFxEICallback<LoadingScreen>(this, &LoadingScreen::FUNC)
	gfxMovie.RegisterEventHandler("eventRegisterUI", MAKE_CALLBACK(eventRegisterUI));

	return true;
}

void LoadingScreen::eventRegisterUI(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	m_GFX_Main = args[0];
	//m_GFX_Main2 = args[1];

	if(m_MapName)
		updateUIDataText(m_MapName, m_MapDesc, m_TipOfTheDay);
}

void ClearFullScreen_Menu();

int LoadingScreen::Update()
{
	R3D_ENSURE_MAIN_THREAD();

	r3dMouse::Show();
	r3dStartFrame();

	if( r3dRenderer->DeviceAvailable )
	{
		r3dRenderer->StartRender(1);
		r3dRenderer->StartFrame();

		r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);

		ClearFullScreen_Menu();

		// for now just draw a static picture in background, later on will be a video
		r3d_assert(m_pBackgroundTex);

		float x, y, w, h;
		r3dRenderer->GetBackBufferViewport(&x, &y, &w, &h);
		D3DVIEWPORT9 oldVp, newVp;

		r3dRenderer->DoGetViewport(&oldVp);
		newVp = oldVp;
		newVp.X = 0;
		newVp.Y = 0;
		newVp.Width = r3dRenderer->d3dpp.BackBufferWidth;
		newVp.Height = r3dRenderer->d3dpp.BackBufferHeight;
		r3dRenderer->SetViewport( (float)newVp.X, (float)newVp.Y, (float)newVp.Width, (float)newVp.Height );
		DWORD oldScissor = 0;
		r3dRenderer->pd3ddev->GetRenderState(D3DRS_SCISSORTESTENABLE, &oldScissor);
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
		r3dDrawBox2D(x, y, w, h, r3dColor24::white, m_pBackgroundTex);
		r3dRenderer->SetViewport( (float)oldVp.X, (float)oldVp.Y, (float)oldVp.Width, (float)oldVp.Height );
		r3dRenderer->pd3ddev->SetRenderState(D3DRS_SCISSORTESTENABLE, oldScissor);

		if(!m_RenderingDisabled)
		{
			gfxMovie.UpdateAndDraw();
		}

		r3dRenderer->Flush();  
		r3dRenderer->EndFrame();
	}

	r3dRenderer->EndRender( true );
	r3dEndFrame();

	return 0;
}

//------------------------------------------------------------------------
void LoadingScreen::SetLoadingTexture(const char* ImagePath)
{
	R3D_ENSURE_MAIN_THREAD();

	if(m_pBackgroundTex)
		r3dRenderer->DeleteTexture(m_pBackgroundTex);
	m_pBackgroundTex = r3dRenderer->LoadTexture(ImagePath);
	r3d_assert(m_pBackgroundTex);
}

void LoadingScreen::SetData( const char* ImagePath, const char* Name, const char* Message, const char* tip_of_the_day )
{
	R3D_ENSURE_MAIN_THREAD();

	SAFE_DELETE_ARRAY(m_MapName);
	SAFE_DELETE_ARRAY(m_MapDesc);
	SAFE_DELETE_ARRAY(m_TipOfTheDay);

	if(!m_RenderingDisabled)
		SetLoadingTexture(ImagePath);

	r3d_assert(_CrtCheckMemory());
	if(m_GFX_Main.IsUndefined())
	{
		if(Name)
		{
			m_MapName = game_new char[strlen(Name)+1];
			r3dscpy(m_MapName, Name);
		}
		if(Message)
		{
			m_MapDesc = game_new char[strlen(Message)+1];
			r3dscpy(m_MapDesc, Message);
		}
		if(tip_of_the_day)
		{
			m_TipOfTheDay = game_new char[strlen(tip_of_the_day)+1];
			r3dscpy(m_TipOfTheDay, tip_of_the_day);
		}
	}
	else
		updateUIDataText(Name, Message, tip_of_the_day);
	r3d_assert(_CrtCheckMemory());
}

void LoadingScreen::updateUIDataText(const char* name, const char* desc, const char* tip)
{
	/*if(g_num_matches_played->GetInt() == 1 && g_num_game_executed2->GetInt()==1) // first time launch of the game, show keyboard schematic
	{
		Scaleform::GFx::Value::DisplayInfo dinfo;
		dinfo.SetVisible(true);
		m_GFX_Main2.SetDisplayInfo(dinfo);
		dinfo.SetVisible(false);
		m_GFX_Main.SetDisplayInfo(dinfo);
	}
	else*/
	{
		//Scaleform::GFx::Value::DisplayInfo dinfo;
		//dinfo.SetVisible(true);
		//m_GFX_Main.SetDisplayInfo(dinfo);
		//dinfo.SetVisible(false);
		//m_GFX_Main2.SetDisplayInfo(dinfo);
	}

	Scaleform::GFx::Value tmp, tmp2, tmp3;
	/*m_GFX_Main2.GetMember("Name", &tmp);
	tmp.GetMember("Name", &tmp2);
	tmp2.GetMember("Text", &tmp3);
	tmp3.SetText(name);*/
	m_GFX_Main.GetMember("Name", &tmp);
	tmp.GetMember("Name", &tmp2);
	tmp2.GetMember("Text", &tmp3);
	tmp3.SetText(name);

	/*m_GFX_Main2.GetMember("Desc", &tmp);
	tmp.GetMember("Text", &tmp2);
	tmp2.GetMember("Text", &tmp3);
	tmp3.SetText(desc);*/
	m_GFX_Main.GetMember("Desc", &tmp);
	tmp.GetMember("Text", &tmp2);
	tmp2.GetMember("Text", &tmp3);
	tmp3.SetText(desc);

	m_GFX_Main.GetMember("Tip", &tmp);
	Scaleform::GFx::Value::DisplayInfo dinfo;
	dinfo.SetVisible(tip!=NULL);
	tmp.SetDisplayInfo(dinfo);
	if(tip)
	{
		tmp.GetMember("Tip", &tmp2);
		tmp2.GetMember("Text", &tmp3);
		tmp3.SetText(tip);

		tmp2.GetMember("Caption", &tmp3);
		tmp3.SetText(gLangMngr.getString("TipOfTheDay"));
	}

	/*Scaleform::GFx::Value vFrame[1], vImages;
	m_GFX_Main.GetMember("Images", &vImages);
	vFrame[0].SetInt(int(u_GetRandom(1.0f, 4.99f)));
	vImages.Invoke("gotoAndStop", NULL, vFrame, 1);*/

	m_GFX_Main.GetMember("Watermark_RU", &tmp);
	dinfo.SetVisible(gLangMngr.getCurrentLang()==LANG_RU);
	tmp.SetDisplayInfo(dinfo);
}

//------------------------------------------------------------------------

void
LoadingScreen::SetProgress( float progress )
{
	R3D_ENSURE_MAIN_THREAD();
	if(!m_GFX_Main.IsUndefined())
	{
		Scaleform::GFx::Value tmp, tmp1, tmp2, tmp3, tmp4;
		m_GFX_Main.GetMember("Bar", &tmp);
		tmp.GetMember("RankBar", &tmp1);
		tmp1.GetMember("Bar", &tmp2);
		float baseWidth = 1510;
		tmp2.GetMember("width", &tmp4);
		tmp4.SetNumber(baseWidth*R3D_CLAMP(progress, 0.01f, 1.0f));
		tmp2.SetMember("width", tmp4);

		/*m_GFX_Main2.GetMember("Bar", &tmp);
		tmp.GetMember("RankBar", &tmp1);
		tmp1.GetMember("Bar", &tmp2);
		tmp1.GetMember("Scale", &tmp3);
		tmp3.GetMember("width", &tmp4);
		baseWidth = (float)tmp4.GetNumber();
		tmp2.GetMember("width", &tmp4);
		tmp4.SetNumber(baseWidth*R3D_CLAMP(progress, 0.01f, 1.0f));
		tmp2.SetMember("width", tmp4);*/

//		Bar._width = Math.round(Scale._width*_global.LocalPercent/100)-5;

	}
	//gfxMovie.SetVariable("_global.LoadedPercent", progress*100);
}

//------------------------------------------------------------------------

static volatile float	gProgress;
static LoadingScreen*	gLoadingScreen;

void StartLoadingScreen()
{
	r3d_assert( !gLoadingScreen );
	gLoadingScreen = game_new LoadingScreen( "Data\\Menu\\LoadingScreen.swf" );

	gLoadingScreen->Load();
	gLoadingScreen->Initialize();
}
void DisableLoadingRendering()
{
	gLoadingScreen->SetRenderingDisabled( true );
}

//------------------------------------------------------------------------

void StopLoadingScreen()
{
	r3d_assert( gLoadingScreen );
	delete gLoadingScreen;

	gLoadingScreen = NULL;
}

void SetLoadingTexture(const char* ImagePath)
{
	if(gLoadingScreen)
	{
		gLoadingScreen->SetLoadingTexture(ImagePath);
	}
}

//------------------------------------------------------------------------

void SetLoadingProgress( float progress )
{
	progress = R3D_MAX( R3D_MIN( progress, 1.f ), 0.f );
	InterlockedExchange( (volatile long*)&gProgress, (LONG&)progress );
}

//------------------------------------------------------------------------

void AdvanceLoadingProgress( float add )
{
	float newVal = R3D_MAX( R3D_MIN( gProgress + add, 1.f ), 0.f );
	InterlockedExchange( (volatile long*)&gProgress, (LONG&)newVal );
}

float GetLoadingProgress()
{
	return gProgress;
}

//------------------------------------------------------------------------

void SetLoadingPhase( const char* Phase )
{
	Phase;
}

//------------------------------------------------------------------------

int DoLoadingScreen( volatile LONG* Loading, const char* LevelName, const char* LevelDescription, const char* LevelFolder, float TimeOut, int gameMode )
{
	r3d_assert( gLoadingScreen );

	char sFullPath[512];
	sprintf( sFullPath, "%s\\%s", LevelFolder, "LoadingScreen.dds" );

	// no dollar sign for access
	if(r3d_access(sFullPath, 0) != 0)
	{
		int sel = rand()%3;
		sprintf( sFullPath, "%s\\LoadingScreen%d.dds", LevelFolder, sel );
	}

	char tempStr[32];
	sprintf(tempStr, "TipOfTheDay%d", int(floorf(u_GetRandom(0.0f, 12.99f))));
	gLoadingScreen->SetData( sFullPath, LevelName, LevelDescription, gLangMngr.getString(tempStr));

	bool checkTimeOut = TimeOut != 0.f;

	float endWait = r3dGetTime() + TimeOut;

	while( *Loading )
	{
		if( checkTimeOut && r3dGetTime() > endWait ) 
		{
			return 0;
		}

		r3dProcessWindowMessages();

		if( r3dRenderer->DeviceAvailable )
		{
			float timeStart = r3dGetTime();

			float MaxRenderTime = 0.033f;

			for( ; r3dGetTime() - timeStart < 0.033f ; )
			{
				ProcessDeviceQueue( timeStart, MaxRenderTime ) ;
			}
		}

		gLoadingScreen->Update();
		gLoadingScreen->SetProgress( gProgress );
	}

	return 1;
}

//------------------------------------------------------------------------
bool IsNeedExit();
int DoConnectScreen( volatile LONG* Loading, const char* Message, float TimeOut )
{
	r3d_assert( gLoadingScreen );

	gLoadingScreen->SetData( "Data\\Menu\\ConnectScreen.dds", gLangMngr.getString("Connecting"), Message, NULL );

	bool checkTimeOut = TimeOut != 0.f;

	float endWait = r3dGetTime() + TimeOut;

	while( *Loading )
	{
		r3dProcessWindowMessages();

		if(IsNeedExit())
			return 0;
		if( checkTimeOut && r3dGetTime() > endWait ) 
		{
			return 0;
		}

		gLoadingScreen->Update();
		gLoadingScreen->SetProgress( checkTimeOut ? 1.f - ( endWait - r3dGetTime() ) / TimeOut : gProgress );
		Sleep( 33 );
	}

	return 1;
}

//------------------------------------------------------------------------

template <typename T>
int DoConnectScreen( T* Logic, bool (T::*CheckFunc)(), const char* Message, float TimeOut )
{
	r3d_assert( gLoadingScreen );

	gLoadingScreen->SetData( "Data\\Menu\\ConnectScreen.dds", gLangMngr.getString("Connecting"), Message, NULL );

	bool checkTimeOut = TimeOut != 0.f;

	const float startWait = r3dGetTime();
	const float endWait = startWait + TimeOut;

	for(;;)
	{
		extern void tempDoMsgLoop();
		tempDoMsgLoop();

		if( (Logic->*CheckFunc)() )
			break;

		if(IsNeedExit())
			return 0;
		if( checkTimeOut && r3dGetTime() > endWait ) 
		{
			return 0;
		}

		// draw loaing screen only after some time
		// so minor waits will be performed without graphics change
		if(r3dGetTime() > startWait + 1.0f)
		{
			gLoadingScreen->Update();
			gLoadingScreen->SetProgress( checkTimeOut ? 1.f - ( endWait - r3dGetTime() ) / TimeOut : gProgress );
		}

		Sleep( 33 );
	}

	return 1;
}

template int DoConnectScreen( ClientGameLogic* Logic, bool (ClientGameLogic::*CheckFunc)(), const char* Message, float TimeOut );
template int DoConnectScreen( MasterServerLogic* Logic, bool (MasterServerLogic::*CheckFunc)(), const char* Message, float TimeOut );

//------------------------------------------------------------------------

