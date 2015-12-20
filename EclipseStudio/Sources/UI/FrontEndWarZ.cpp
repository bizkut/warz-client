#include "r3dPCH.h"
#include "r3d.h"

#include "GameCommon.h"
#include "r3dDebug.h"

#include "FrontendWarZ.h"
#include "GameCode\UserFriends.h"
#include "GameCode\UserRewards.h"
#include "GameCode\UserSkills.h"
#include "GameCode\UserClans.h"
#include "GameCode\UserSettings.h"

#include "CkHttpRequest.h"
#include "CkHttpResponse.h"
#include "backend/HttpDownload.h"
#include "backend/WOBackendAPI.h"

#include "../rendering/Deffered/CommonPostFX.h"
#include "../rendering/Deffered/PostFXChief.h"

#include "multiplayer/MasterServerLogic.h"
#include "multiplayer/FriendsServerLogic.h"
#include "multiplayer/LoginSessionPoller.h"

#include "../ObjectsCode/weapons/WeaponArmory.h"
#include "../ObjectsCode/weapons/Weapon.h"
#include "../ObjectsCode/weapons/Ammo.h"
#include "../ObjectsCode/weapons/Gear.h"
#include "../ObjectsCode/ai/AI_Player.h"
#include "../ObjectsCode/ai/AI_PlayerAnim.h"
#include "../ObjectsCode/Gameplay/UIWeaponModel.h"
#include "GameLevel.h"
#include "Scaleform/Src/Render/D3D9/D3D9_Texture.h"
#include "../../Eternity/Source/r3dEternityWebBrowser.h"

#include "SectorMaster.h"
#include "TeamSpeakClient.h"
#include "m_LoadingScreen.h"

#include "HWInfo.h"

#include "shellapi.h"
#include "SteamHelper.h"
#include "../Editors/CameraSpotsManager.h"

// for IcmpSendEcho
#undef NTDDI_VERSION
#define NTDDI_VERSION NTDDI_WINXP
#include <iphlpapi.h>
#include <icmpapi.h>
#pragma comment(lib, "iphlpapi.lib")

extern	char		_p2p_masterHost[MAX_PATH];
extern	int		_p2p_masterPort;

char	Login_PassedUser[256] = "";
char	Login_PassedPwd[256] = "";
char	Login_PassedAuth[256] = "";
static int LoginMenuExitFlag = 0;

void writeGameOptionsFile();
extern r3dScreenBuffer*	Scaleform_RenderToTextureRT;

extern void ReloadLocalization();

float getRatio(float num1, float num2)
{
	if(num1 == 0)
		return 0.0f;
	if(num2 == 0)
		return num1;
	
	return num1/num2;
}

const char* getTimePlayedString(int timePlayed) 
{
	int seconds = timePlayed%60;
	int minutes = (timePlayed/60)%60;
	int hours = (timePlayed/3600)%24;
	int days = (timePlayed/86400);

	static char tmpStr[64];
	sprintf(tmpStr, "%d:%02d:%02d", days, hours, minutes);
	return tmpStr;
}

const char* getReputationString(int reputation)
{
	const char* algnmt = "$rep_civilian";
	if(reputation >= 1000)
		algnmt = "$rep_paragon";
	else if(reputation >= 500)
		algnmt = "$rep_vigilante";
	else if(reputation >= 250)
		algnmt = "$rep_guardian";
	else if(reputation >= 80)
		algnmt = "$rep_lawmen";
	else if(reputation >= 20)
		algnmt = "$rep_deputy";
	else if(reputation >= 10)
		algnmt = "$rep_constable";
	else if(reputation <= -1000)
		algnmt = "$rep_assassin";
	else if(reputation <= -600)
		algnmt = "$rep_villain";
	else if(reputation <= -300)
		algnmt = "$rep_hitman";
	else if(reputation <= -100)
		algnmt = "$rep_bandit";
	else if(reputation <= -25)
		algnmt = "$rep_outlaw";
	else if(reputation <= -5)
		algnmt = "$rep_thug";

	return algnmt;
}

const char* getReputationIconName(int reputation)
{
	const char* algnmt = "civilian";
	if(reputation >= 1000)
		algnmt = "paragon";
	else if(reputation >= 500)
		algnmt = "vigilante";
	else if(reputation >= 250)
		algnmt = "guardian";
	else if(reputation >= 80)
		algnmt = "lawman";
	else if(reputation >= 20)
		algnmt = "deputy";
	else if(reputation >= 10)
		algnmt = "constable";
	else if(reputation <= -1000)
		algnmt = "assassin";
	else if(reputation <= -600)
		algnmt = "villain";
	else if(reputation <= -300)
		algnmt = "hitman";
	else if(reputation <= -100)
		algnmt = "bandit";
	else if(reputation <= -20)
		algnmt = "outlaw";
	else if(reputation <= -5)
		algnmt = "thug";

	return algnmt;
}

const char* getMapName(int mapID)
{
	if(mapID == GBGameInfo::MAPID_WZ_Colorado) return gLangMngr.getString("MapColorado");
	else if(mapID == GBGameInfo::MAPID_WZ_Cliffside) return gLangMngr.getString("MapCliffside");
	else if(mapID == GBGameInfo::MAPID_WZ_California) return gLangMngr.getString("MapCalifornia");
	else if(mapID == GBGameInfo::MAPID_WZ_Caliwood) return gLangMngr.getString("MapCaliwood");
	else if(mapID == GBGameInfo::MAPID_WZ_ColoradoV1) return gLangMngr.getString("MapColoradoV1");
	else if(mapID == GBGameInfo::MAPID_WZ_SanDiego) return gLangMngr.getString("MapSanDiego");
	else if(mapID == GBGameInfo::MAPID_WZ_Devmap) return gLangMngr.getString("MapDevmap");
	else if(mapID == GBGameInfo::MAPID_WZ_GameHard1) return gLangMngr.getString("MapGameHard1");
	else if(mapID == GBGameInfo::MAPID_ServerTest) return "DEVMAP";

	return "";
}

int		superPings_[2048]; // big enough to hold all possiblesupervisors id

int GetGamePing(DWORD superId)
{
	// high word of gameId is supervisor Id
	r3d_assert(superId < R3D_ARRAYSIZE(superPings_));
	return superPings_[superId];
}

FrontendWarZ::FrontendWarZ(const char * movieName)
: UIMenu(movieName)
, r3dIResource(r3dIntegrityGuardian())
{
	extern bool g_bDisableP2PSendToHost;
	g_bDisableP2PSendToHost = true;

	RTScaleformTexture = NULL;
	needReInitScaleformTexture = false;

	prevGameResult = GRESULT_Unknown;

	CurrentBrowseChannel = 0;

	leaderboardSize = 1000;
	leaderboard_requestStartPos = 0;
	leaderboard_BoardSelected = 0;

	asyncThread_ = NULL;
	asyncErr_[0] = 0;

	CancelQuickJoinRequest = false;
  	exitRequested_      = false;
  	needExitByGameJoin_ = false;
	needReturnFromQuickJoin = false;
	m_ReloadProfile = false;

	lastServerReqTime_ = -1;
	masterConnectTime_ = -1;

	m_CreateGameMode = 0;

	memset(&superPings_, 0, sizeof(superPings_));
		
	m_Player = 0;
	m_needPlayerRenderingRequest = 0;
	m_CreateHeroID = 0;
	m_CreateBodyIdx = 0;
	m_CreateHeadIdx = 0;
	m_CreateLegsIdx = 0;
	
	m_joinGameServerId = 0;
	m_joinGamePwd[0]   = 0;
	
	m_buyGpPriceCents = -1;
	
	m_rentServerPrice = 0;

	loginThread = NULL;
	loginAnswerCode = ANS_Unactive;

	m_browseGamesMode = 0;
}

FrontendWarZ::~FrontendWarZ()
{
	r3d_assert(asyncThread_ == NULL);
	r3d_assert(loginThread == NULL);

	if(m_Player)
	{
		GameWorld().DeleteObject(m_Player);

		extern void DestroyGame(); // destroy game only if player was loaded. to prevent double call to destroy game
		DestroyGame();
	}

	extern bool g_bDisableP2PSendToHost;
	g_bDisableP2PSendToHost = false;

	WorldLightSystem.Destroy();
}

unsigned int WINAPI FrontendWarZ::LoginProcessThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;

	r3d_assert(This->loginAnswerCode == ANS_Unactive);
	This->loginAnswerCode = ANS_Processing;
	gUserProfile.CustomerID = 0;

	CWOBackendReq req("api_Login.aspx");
	req.AddParam("username", Login_PassedUser);
	req.AddParam("password", Login_PassedPwd);

	if(!req.Issue())
	{
		r3dOutToLog("Login FAILED, code: %d\n", req.resultCode_);
		This->loginAnswerCode = req.resultCode_ == 8 ? ANS_Timeout : ANS_Error;
		return 0;
	}

	int n = sscanf(req.bodyStr_, "%d %d %d", 
		&gUserProfile.CustomerID, 
		&gUserProfile.SessionID,
		&gUserProfile.AccountStatus);
	if(n != 3)
	{
		r3dOutToLog("Login: bad answer\n");
		This->loginAnswerCode = ANS_Error;
		return 0;
	}
	//r3dOutToLog("CustomerID: %d\n",gUserProfile.CustomerID);

	if(gUserProfile.CustomerID == 0)
		This->loginAnswerCode = ANS_BadPassword;
	else if(gUserProfile.AccountStatus >= 200)
		This->loginAnswerCode = ANS_Frozen;
	else
		This->loginAnswerCode = ANS_Logged;

	return 0;
}

void FrontendWarZ_LoginProcessThread(void *in_data)
{
	FrontendWarZ::LoginProcessThread(in_data);
}

unsigned int WINAPI FrontendWarZ::LoginAuthThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;

	r3d_assert(This->loginAnswerCode == ANS_Unactive);
	This->loginAnswerCode = ANS_Processing;
	r3d_assert(gUserProfile.CustomerID);
	r3d_assert(gUserProfile.SessionID);

	CWOBackendReq req(&gUserProfile, "api_LoginSessionPoller.aspx");
	if(req.Issue() == true)
	{
		This->loginAnswerCode = ANS_Logged;
		return true;
	}

	gUserProfile.CustomerID    = 0;
	gUserProfile.SessionID     = 0;
	gUserProfile.AccountStatus = 0;

	r3dOutToLog("LoginAuth: %d\n", req.resultCode_);
	This->loginAnswerCode = ANS_BadPassword;
	return 0;
}

static bool getAuthFromRegistry(const char* token, int tokensize)
{
	// query for game registry node
	HKEY hKey;
	int hr;
	hr = RegOpenKeyEx(HKEY_CURRENT_USER, 
		"Software\\Arktos Entertainment Group\\WarZ", 
		0, 
		KEY_ALL_ACCESS, 
		&hKey);
	if(hr != ERROR_SUCCESS)
		return false;

	DWORD size = tokensize;
	hr = RegQueryValueEx(hKey, "LoginToken", NULL, NULL, (BYTE*)token, &size);
	if(hr != ERROR_SUCCESS)
		return false;
	
	// and clear it
	hr = RegDeleteValue(hKey, "LoginToken");
	RegCloseKey(hKey);
	return true;
}

bool FrontendWarZ::DecodeAuthParams()
{
	r3d_assert(Login_PassedAuth[0]);
	
	if(Login_PassedAuth[0] == '=' && Login_PassedAuth[1] == '=')
	{
		if(!getAuthFromRegistry(Login_PassedAuth, sizeof(Login_PassedAuth)))
		{
			r3dError("unable to get auth data");
			return false;
		}
	}

	CkString s1;
	s1 = Login_PassedAuth;
	s1.base64Decode("utf-8");

	char* authToken = (char*)s1.getAnsi();
	for(size_t i=0; i<strlen(authToken); i++)
		authToken[i] = authToken[i] ^ 0x64;

	DWORD CustomerID = 0;
	DWORD SessionID = 0;
	DWORD AccountStatus = 0;
	int n = sscanf(authToken, "%d:%d:%d", &CustomerID, &SessionID, &AccountStatus);
	if(n != 3)
	{
		r3dError("unable to get session data");
		return false;
	}

	gUserProfile.CustomerID    = CustomerID;
	gUserProfile.SessionID     = SessionID;
	gUserProfile.AccountStatus = AccountStatus;
	return true;
}

void FrontendWarZ::LoginCheckAnswerCode()
{
	if(loginAnswerCode == ANS_Unactive)
		return;
		
	if(loginAnswerCode == ANS_Processing)
		return;
		
	// wait for thread to finish
	if(::WaitForSingleObject(loginThread, 1000) == WAIT_TIMEOUT)
		r3d_assert(0);
	
	CloseHandle(loginThread);
	loginThread = NULL;
	
	Scaleform::GFx::Value vars[3];
	switch(loginAnswerCode)
	{
	case ANS_Timeout:
		loginMsgBoxOK_Exit = true;
		vars[0].SetString(gLangMngr.getString("LoginMenu_CommError"));
		vars[1].SetBoolean(true);
		vars[2].SetString("$ERROR");
		gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
		break;
	case ANS_Error:
		loginMsgBoxOK_Exit = true;
		vars[0].SetString(gLangMngr.getString("LoginMenu_WrongLoginAnswer"));
		vars[1].SetBoolean(true);
		vars[2].SetString("$ERROR");
		gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
		break;
	case ANS_Logged:
		LoginMenuExitFlag = 1; 
		break;

	case ANS_BadPassword:
		loginMsgBoxOK_Exit = true;
		vars[0].SetString(gLangMngr.getString("LoginMenu_LoginFailed"));
		vars[1].SetBoolean(true);
		vars[2].SetString("$ERROR");
		gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
		break;

	case ANS_Frozen:
		loginMsgBoxOK_Exit = true;
		vars[0].SetString(gLangMngr.getString("LoginMenu_AccountFrozen"));
		vars[1].SetBoolean(true);
		vars[2].SetString("$ERROR");
		gfxMovie.Invoke("_root.api.showInfoMsg", vars, 2);
		break;
	}
}

void FrontendWarZ::initLoginStep(const char* loginErrorMsg)
{
	LoginMenuExitFlag = 0;
	loginProcessStartTime = r3dGetTime();

	// show info message and render it one time
	gfxMovie.Invoke("_root.api.showLoginMsg", gLangMngr.getString("LoggingIn"));

	if(loginErrorMsg)
	{
		loginMsgBoxOK_Exit = true;
		Scaleform::GFx::Value vars[3];
		vars[0].SetString(loginErrorMsg);
		vars[1].SetBoolean(true);
		vars[2].SetString("$ERROR");
		gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
		return;
	}

	if( r3dRenderer->DeviceAvailable )
	{
		// advance movie by 5 frames, so info screen will fade in and show
		Scaleform::GFx::Movie* pMovie = gfxMovie.GetMovie();

		pMovie->Advance((1.0f/pMovie->GetFrameRate()) * 5);

		r3dRenderer->StartFrame();
		r3dRenderer->StartRender(1);

		r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);

		gfxMovie.UpdateAndDraw();

		r3dRenderer->Flush();
		CurRenderPipeline->Finalize() ;
		r3dRenderer->EndFrame();
	}
	r3dRenderer->EndRender( true );

	if(!loginErrorMsg)
	{
		// if we have encoded login session information
		if(Login_PassedAuth[0])
		{
			if(DecodeAuthParams())
			{
				r3d_assert(loginThread == NULL);
				loginThread = (HANDLE)_beginthreadex(NULL, 0, &LoginAuthThread, this, 0, NULL);
				if(loginThread == NULL)
					r3dError("Failed to begin thread");
			}
			return;
		}
#ifndef FINAL_BUILD
		if(Login_PassedUser[0] == 0 || Login_PassedPwd[0] == 0)
		{
			r3dscpy(Login_PassedUser, d_login->GetString());
			r3dscpy(Login_PassedPwd, d_password->GetString());
			if(strlen(Login_PassedUser)<2 || strlen(Login_PassedPwd)<2)
			{
				r3dError("you should set login as d_login <user> d_password <pwd> in local.ini");
				// programmers only can do this:
				//r3dError("you should set login as '-login <user> -pwd <pwd> in command line");
			}
		}
#endif

		loginThread = (HANDLE)_beginthreadex(NULL, 0, &LoginProcessThread, this, 0, NULL);
	}
}

static volatile LONG gProfileIsAquired = 0;
static volatile LONG gProfileOK = 0;
static volatile float gTimeWhenProfileLoaded = 0;
static volatile LONG gProfileLoadStage = 0;

extern CHWInfo g_HardwareInfo;

static void SetLoadStage(const char* stage)
{
	const static char* sname = NULL;
	static float stime = 0;
#ifndef FINAL_BUILD	
	if(sname) 
	{
		r3dOutToLog("SetLoadStage: %4.2f sec in %s\n", r3dGetTime() - stime, sname);
	}
#endif

	sname = stage;
	stime = r3dGetTime();
	gProfileLoadStage++;
}

static void LoadFrontendGameData(FrontendWarZ* UI)
{
	//
	// load shooting gallery
	//
	SetLoadStage("FrontEnd Lighting Level");
	{
		extern void DoLoadGame(const char* LevelFolder, int MaxPlayers, bool unloadPrev, bool isMenuLevel );
		DoLoadGame(r3dGameLevel::GetHomeDir(), 4, true, true );
	}

	//
	// create player and FPS weapon
	//
	SetLoadStage("Player Model");
	{
		obj_Player* plr = (obj_Player *)srv_CreateGameObject("obj_Player", "Player", r3dPoint3D(0,0,0));
		plr->PlayerState = PLAYER_IDLE;
		plr->bDead = 0;
		plr->CurLoadout = gUserProfile.ProfileData.ArmorySlots[0];
		plr->m_disablePhysSkeleton = true;
		plr->m_fPlayerRotationTarget = plr->m_fPlayerRotation = 0;
		// we need it to be created as a networklocal character for physics.
		plr->NetworkLocal = true;
		plr->OnCreate();
		plr->NetworkLocal = false;
		// work around for loading fps model sometimes instead of proper tps model
		plr->UpdateLoadoutSlot(plr->CurLoadout, -1);
		// switch player to UI idle mode
		plr->uberAnim_->IsInUI = true;
		plr->uberAnim_->AnimPlayerState = -1;
		plr->uberAnim_->anim.StopAll();	// prevent animation blending on loadout switch
		plr->SyncAnimation(true);
		UI->SetLoadedThings(plr);
	}
}

static bool ActualGetProfileData(FrontendWarZ* UI)
{
	gProfileLoadStage = 0;

	SetLoadStage("ApiGetShopData");
	if(gUserProfile.ApiGetShopData() != 0)
		return false;
		
	// get game rewards from server.
	SetLoadStage("ApiGameRewards");
	if(g_GameRewards == NULL)
		g_GameRewards = game_new CGameRewards();
	if(!g_GameRewards->loaded_) {
		if(g_GameRewards->ApiGetDataGameRewards() != 0) {
			return false;
		}
	}
		
	// update items info only once and do not check for errors
	static bool gotCurItemsData = false;
	SetLoadStage("ApiGetItemsInfo");
	if(!gotCurItemsData) {
		gotCurItemsData = true;
		gUserProfile.ApiGetItemsInfo();
	}

	static bool gotGP2GDTable = false;
	SetLoadStage("ApiGetConvertGP2GD");
	if(!gotGP2GDTable)
	{
		gotGP2GDTable = true;
		gUserProfile.ApiGetGamePointsConvertsionRates();
	}

	SetLoadStage("GetProfile");
	if(gUserProfile.GetProfile() != 0)
		return false;

	// load player only after profile
	// need to load game data first, because of DestroyGame() in destructor
	LoadFrontendGameData(UI);

	if(gUserProfile.ProfileDataDirty > 0)
	{
		//@TODO: set dirty profile flag, repeat getting profile
		r3dOutToLog("@@@@@@@@@@ProfileDataDirty: %d\n", gUserProfile.ProfileDataDirty);
	}

	// initiate connection to friends server, do not wait for actual connect
	gFriendsServerLogic.ConnectAsync(_p2p_masterHost, FSNET_SERVER_PORT);


	// send HW report if necessary
	/*SetLoadStage("HWReport");
	if(FrontendWarZ::frontendFirstTimeInit)
	{
		if(NeedUploadReport(g_HardwareInfo))
		{
			CWOBackendReq req(&gUserProfile, "api_ReportHWInfo_Customer.aspx");
			char buf[1024];
			sprintf(buf, "%I64d", g_HardwareInfo.uniqueId);
			req.AddParam("r00", buf);
			req.AddParam("r10", g_HardwareInfo.CPUString);
			req.AddParam("r11", g_HardwareInfo.CPUBrandString);
			sprintf(buf, "%d", g_HardwareInfo.CPUFreq);
			req.AddParam("r12", buf);
			sprintf(buf, "%d", g_HardwareInfo.TotalMemory);
			req.AddParam("r13", buf);

			sprintf(buf, "%d", g_HardwareInfo.DisplayW);
			req.AddParam("r20", buf);
			sprintf(buf, "%d", g_HardwareInfo.DisplayH);
			req.AddParam("r21", buf);
			sprintf(buf, "%d", g_HardwareInfo.gfxErrors);
			req.AddParam("r22", buf);
			sprintf(buf, "%d", g_HardwareInfo.gfxVendorId);
			req.AddParam("r23", buf);
			sprintf(buf, "%d", g_HardwareInfo.gfxDeviceId);
			req.AddParam("r24", buf);
			req.AddParam("r25", g_HardwareInfo.gfxDescription);

			req.AddParam("r30", g_HardwareInfo.OSVersion);

			if(!req.Issue())
			{
				r3dOutToLog("Failed to upload HW Info\n");
			}
			else
			{
				// mark that we reported it
				HKEY hKey;
				int hr;
				hr = RegCreateKeyEx(HKEY_CURRENT_USER, 
					"Software\\Arktos Entertainment Group\\TheWarZ", 
					0, 
					NULL,
					REG_OPTION_NON_VOLATILE, 
					KEY_ALL_ACCESS,
					NULL,
					&hKey,
					NULL);
				if(hr == ERROR_SUCCESS)
				{
					__int64 repTime = _time64(NULL);
					DWORD size = sizeof(repTime);

					hr = RegSetValueEx(hKey, "UpdaterTime2", NULL, REG_QWORD, (BYTE*)&repTime, size);
					RegCloseKey(hKey);
				}
			}
		}
	}*/

	SetLoadStage(NULL);
	return true;
}

static unsigned int WINAPI GetProfileDataThread( void * FrontEnd )
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	r3dRandInitInTread rand_in_thread;

	try 
	{
		gProfileOK = 0;
		if(ActualGetProfileData((FrontendWarZ*)FrontEnd))
		{
			gProfileOK = 1;
			gTimeWhenProfileLoaded = r3dGetTime();
		}
	}
	catch(const char* err)
	{
		// catch r3dError
		r3dOutToLog("GetProfileData error: %s\n", err);
	}
		
	InterlockedExchange( &gProfileIsAquired, 1 );

	return 0;
}

//////////////////////////////////////////////////////////////////////////

void FrontendWarZ::D3DCreateResource()
{
	needReInitScaleformTexture = true;
}

//////////////////////////////////////////////////////////////////////////

static float aquireProfileStart = 0;
static HANDLE handleGetProfileData = 0;

bool FrontendWarZ::Initialize()
{
	extern int g_CCBlackWhite;
	extern float g_fCCBlackWhitePwr;

	g_CCBlackWhite = false;
	g_fCCBlackWhitePwr = 0.0f;

	bindRTsToScaleForm();
	frontendStage = 0;
	loginMsgBoxOK_Exit = false;
	ReviveCharSuccessfull = 0;

	clanListRequest_SortID = 1;
	clanListRequest_StartPos = 0;
	clanListRequest_ClanListSize = 1000;

	m_browseGamesCurrentCur = 0;

	m_browseGamesNameFilter[0]=0;

	// check for bad values in contrast, brightness
// 	if(r_contrast->GetFloat() < r_contrast->GetMinVal() || r_contrast->GetFloat() > r_contrast->GetMaxVal())
// 		r_contrast->SetFloat(0.5f);
// 	if(r_brightness->GetFloat() < r_brightness->GetMinVal() || r_brightness->GetFloat() > r_brightness->GetMaxVal())
// 		r_brightness->SetFloat(0.5f);
	
	if(g_mouse_sensitivity->GetFloat() < g_mouse_sensitivity->GetMinVal() || g_mouse_sensitivity->GetFloat() > g_mouse_sensitivity->GetMaxVal())
		g_mouse_sensitivity->SetFloat(0.5f);
	if(s_sound_volume->GetFloat() < s_sound_volume->GetMinVal() || s_sound_volume->GetFloat() > s_sound_volume->GetMaxVal())
		s_sound_volume->SetFloat(1.0f);
	if(s_music_volume->GetFloat() < s_music_volume->GetMinVal() || s_music_volume->GetFloat() > s_music_volume->GetMaxVal())
		s_music_volume->SetFloat(1.0f);
//	if(s_comm_volume->GetFloat() < s_comm_volume->GetMinVal() || s_comm_volume->GetFloat() > s_comm_volume->GetMaxVal())
//		s_comm_volume->SetFloat(1.0f);

	// reacquire the menu.
	gfxMovie.SetKeyboardCapture();

	r_film_tone_a->SetFloat(0.15f);
	r_film_tone_b->SetFloat(0.50f);
	r_film_tone_c->SetFloat(0.10f);
	r_film_tone_d->SetFloat(0.20f);
	r_film_tone_e->SetFloat(0.02f);
	r_film_tone_f->SetFloat(0.30f);
	r_exposure_bias->SetFloat(0.5f);
	r_white_level->SetFloat(11.2f);

	gClientLogic().Reset(); // reset game finished, otherwise player will not update and will not update its skelet and will not render

	gfxMovie.SetCurentRTViewport( Scaleform::GFx::Movie::SM_ExactFit );

#define MAKE_CALLBACK(FUNC) game_new r3dScaleformMovie::TGFxEICallback<FrontendWarZ>(this, &FrontendWarZ::FUNC)
	gfxMovie.RegisterEventHandler("eventPlayGame", MAKE_CALLBACK(eventPlayGame));
	gfxMovie.RegisterEventHandler("eventCancelQuickGameSearch", MAKE_CALLBACK(eventCancelQuickGameSearch));
	gfxMovie.RegisterEventHandler("eventQuitGame", MAKE_CALLBACK(eventQuitGame));
	gfxMovie.RegisterEventHandler("eventCreateCharacter", MAKE_CALLBACK(eventCreateCharacter));
	gfxMovie.RegisterEventHandler("eventRenameCharacter", MAKE_CALLBACK(eventRenameCharacter));
	gfxMovie.RegisterEventHandler("eventDeleteChar", MAKE_CALLBACK(eventDeleteChar));
	gfxMovie.RegisterEventHandler("eventShowSurvivorsMap", MAKE_CALLBACK(eventShowSurvivorsMap));
	gfxMovie.RegisterEventHandler("eventReviveChar", MAKE_CALLBACK(eventReviveChar));
	gfxMovie.RegisterEventHandler("eventReviveCharMoney", MAKE_CALLBACK(eventReviveCharMoney));
	gfxMovie.RegisterEventHandler("eventBuyItem", MAKE_CALLBACK(eventBuyItem));	
	gfxMovie.RegisterEventHandler("eventBackpackFromInventory", MAKE_CALLBACK(eventBackpackFromInventory));	
	gfxMovie.RegisterEventHandler("eventBackpackToInventory", MAKE_CALLBACK(eventBackpackToInventory));	
	gfxMovie.RegisterEventHandler("eventBackpackGridSwap", MAKE_CALLBACK(eventBackpackGridSwap));	
	gfxMovie.RegisterEventHandler("eventSetSelectedChar", MAKE_CALLBACK(eventSetSelectedChar));	
	gfxMovie.RegisterEventHandler("eventOpenBackpackSelector", MAKE_CALLBACK(eventOpenBackpackSelector));	
	gfxMovie.RegisterEventHandler("eventChangeBackpack", MAKE_CALLBACK(eventChangeBackpack));	
	gfxMovie.RegisterEventHandler("eventLearnSkill", MAKE_CALLBACK(eventLearnSkill));	
	gfxMovie.RegisterEventHandler("eventChangeOutfit", MAKE_CALLBACK(eventChangeOutfit));	
	gfxMovie.RegisterEventHandler("eventSetCurrentBrowseChannel", MAKE_CALLBACK(eventSetCurrentBrowseChannel));	
	gfxMovie.RegisterEventHandler("eventTrialRequestUpgrade", MAKE_CALLBACK(eventTrialRequestUpgrade));	
	gfxMovie.RegisterEventHandler("eventTrialUpgradeAccount", MAKE_CALLBACK(eventTrialUpgradeAccount));	
	gfxMovie.RegisterEventHandler("eventBuyPremiumAccount", MAKE_CALLBACK(eventBuyPremiumAccount));	
	gfxMovie.RegisterEventHandler("eventMarketplaceActive", MAKE_CALLBACK(eventMarketplaceActive));	
	gfxMovie.RegisterEventHandler("eventOpenURL", MAKE_CALLBACK(eventOpenURL));	

	gfxMovie.RegisterEventHandler("eventOptionsReset", MAKE_CALLBACK(eventOptionsReset));
	gfxMovie.RegisterEventHandler("eventOptionsApply", MAKE_CALLBACK(eventOptionsApply));
	gfxMovie.RegisterEventHandler("eventOptionsControlsReset", MAKE_CALLBACK(eventOptionsControlsReset));
	gfxMovie.RegisterEventHandler("eventOptionsControlsApply", MAKE_CALLBACK(eventOptionsControlsApply));
	gfxMovie.RegisterEventHandler("eventOptionsVoipApply", MAKE_CALLBACK(eventOptionsVoipApply));
	gfxMovie.RegisterEventHandler("eventOptionsVoipReset", MAKE_CALLBACK(eventOptionsVoipReset));
	gfxMovie.RegisterEventHandler("eventOptionsLanguageSelection", MAKE_CALLBACK(eventOptionsLanguageSelection));
	gfxMovie.RegisterEventHandler("eventOptionsControlsRequestKeyRemap", MAKE_CALLBACK(eventOptionsControlsRequestKeyRemap));

	gfxMovie.RegisterEventHandler("eventCreateChangeCharacter", MAKE_CALLBACK(eventCreateChangeCharacter));	
	gfxMovie.RegisterEventHandler("eventCreateCancel", MAKE_CALLBACK(eventCreateCancel));	

	gfxMovie.RegisterEventHandler("eventRequestPlayerRender", MAKE_CALLBACK(eventRequestPlayerRender));	
	gfxMovie.RegisterEventHandler("eventMsgBoxCallback", MAKE_CALLBACK(eventMsgBoxCallback));	

	gfxMovie.RegisterEventHandler("eventBrowseGamesRequestFilterStatus", MAKE_CALLBACK(eventBrowseGamesRequestFilterStatus));	
	gfxMovie.RegisterEventHandler("eventBrowseGamesSetFilter", MAKE_CALLBACK(eventBrowseGamesSetFilter));	
	gfxMovie.RegisterEventHandler("eventBrowseGamesJoin", MAKE_CALLBACK(eventBrowseGamesJoin));	
	gfxMovie.RegisterEventHandler("eventBrowseGamesOnAddToFavorites", MAKE_CALLBACK(eventBrowseGamesOnAddToFavorites));	
	gfxMovie.RegisterEventHandler("eventBrowseGamesRequestList", MAKE_CALLBACK(eventBrowseGamesRequestList));	

	gfxMovie.RegisterEventHandler("eventRequestMyClanInfo", MAKE_CALLBACK(eventRequestMyClanInfo));	
	gfxMovie.RegisterEventHandler("eventRequestClanList", MAKE_CALLBACK(eventRequestClanList));	
	gfxMovie.RegisterEventHandler("eventCreateClan", MAKE_CALLBACK(eventCreateClan));	
	gfxMovie.RegisterEventHandler("eventClanAdminDonateGC", MAKE_CALLBACK(eventClanAdminDonateGC));	
	gfxMovie.RegisterEventHandler("eventClanAdminAction", MAKE_CALLBACK(eventClanAdminAction));	
	gfxMovie.RegisterEventHandler("eventClanLeaveClan", MAKE_CALLBACK(eventClanLeaveClan));	
	gfxMovie.RegisterEventHandler("eventClanDonateGCToClan", MAKE_CALLBACK(eventClanDonateGCToClan));	
	gfxMovie.RegisterEventHandler("eventRequestClanApplications", MAKE_CALLBACK(eventRequestClanApplications));	
	gfxMovie.RegisterEventHandler("eventClanApplicationAction", MAKE_CALLBACK(eventClanApplicationAction));	
	gfxMovie.RegisterEventHandler("eventClanInviteToClan", MAKE_CALLBACK(eventClanInviteToClan));	
	gfxMovie.RegisterEventHandler("eventClanRespondToInvite", MAKE_CALLBACK(eventClanRespondToInvite));	
	gfxMovie.RegisterEventHandler("eventClanBuySlots", MAKE_CALLBACK(eventClanBuySlots));	
	gfxMovie.RegisterEventHandler("eventClanApplyToJoin", MAKE_CALLBACK(eventClanApplyToJoin));	

	gfxMovie.RegisterEventHandler("eventStorePurchaseGPCallback", MAKE_CALLBACK(eventStorePurchaseGPCallback));	
	gfxMovie.RegisterEventHandler("eventStorePurchaseGP", MAKE_CALLBACK(eventStorePurchaseGP));	
	gfxMovie.RegisterEventHandler("eventStorePurchaseGPRequest", MAKE_CALLBACK(eventStorePurchaseGPRequest));	
	
	gfxMovie.RegisterEventHandler("eventStorePurchaseGDCallback", MAKE_CALLBACK(eventStorePurchaseGDCallback));	
	gfxMovie.RegisterEventHandler("eventStorePurchaseGD", MAKE_CALLBACK(eventStorePurchaseGD));	

	gfxMovie.RegisterEventHandler("eventDonateGCtoServerCallback", MAKE_CALLBACK(eventDonateGCtoServerCallback));	
	gfxMovie.RegisterEventHandler("eventDonateGCtoServer", MAKE_CALLBACK(eventDonateGCtoServer));	
	gfxMovie.RegisterEventHandler("eventRequestShowDonateGCtoServer", MAKE_CALLBACK(eventRequestShowDonateGCtoServer));	

	gfxMovie.RegisterEventHandler("eventRequestLeaderboardData", MAKE_CALLBACK(eventRequestLeaderboardData));	

	gfxMovie.RegisterEventHandler("eventRequestGCTransactionData", MAKE_CALLBACK(eventRequestGCTransactionData));	

	gfxMovie.RegisterEventHandler("eventRequestMyServerList", MAKE_CALLBACK(eventRequestMyServerList));	
	gfxMovie.RegisterEventHandler("eventRequestMyServerInfo", MAKE_CALLBACK(eventRequestMyServerInfo));	
	gfxMovie.RegisterEventHandler("eventMyServerKickPlayer", MAKE_CALLBACK(eventMyServerKickPlayer));	
	gfxMovie.RegisterEventHandler("eventRentServerUpdatePrice", MAKE_CALLBACK(eventRentServerUpdatePrice));	
	gfxMovie.RegisterEventHandler("eventRentServer", MAKE_CALLBACK(eventRentServer));	
	gfxMovie.RegisterEventHandler("eventMyServerJoinServer", MAKE_CALLBACK(eventMyServerJoinServer));	
	gfxMovie.RegisterEventHandler("eventMyServerUpdateSettings", MAKE_CALLBACK(eventMyServerUpdateSettings));	
	gfxMovie.RegisterEventHandler("eventRenewServerUpdatePrice", MAKE_CALLBACK(eventRenewServerUpdatePrice));	
	gfxMovie.RegisterEventHandler("eventRenewServer", MAKE_CALLBACK(eventRenewServer));	

	return true;
}

void FrontendWarZ::postLoginStepInit(EGameResult gameResult)
{
	frontendStage = 1;
	prevGameResult = gameResult;

	gProfileIsAquired = 0;
	aquireProfileStart = r3dGetTime();
	handleGetProfileData = (HANDLE)_beginthreadex(NULL, 0, &GetProfileDataThread, this, 0, 0);
	if(handleGetProfileData == 0)
		r3dError("Failed to begin thread");

	// show info message and render it one time
	gfxMovie.Invoke("_root.api.showLoginMsg", gLangMngr.getString("RetrievingProfileData"));

	if( r3dRenderer->DeviceAvailable )
	{
		// advance movie by 5 frames, so info screen will fade in and show
		Scaleform::GFx::Movie* pMovie = gfxMovie.GetMovie();

		pMovie->Advance((1.0f/pMovie->GetFrameRate()) * 5);

		r3dRenderer->StartFrame();
		r3dRenderer->StartRender(1);

		r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);

		gfxMovie.UpdateAndDraw();

		r3dRenderer->Flush();
		CurRenderPipeline->Finalize() ;
		r3dRenderer->EndFrame();
	}
	r3dRenderer->EndRender( true );

	// init things to load game level
	r3dGameLevel::SetHomeDir("WZ_FrontEndLighting");
	r3dGameLevel::SetGassEnable("WZ_FrontEndLighting",true);
	extern void InitGame_Start();
	InitGame_Start();
}

void FrontendWarZ::bindRTsToScaleForm()
{
	RTScaleformTexture = gfxMovie.BoundRTToImage("merc_rendertarget", Scaleform_RenderToTextureRT->AsTex2D(), (int)Scaleform_RenderToTextureRT->Width, (int)Scaleform_RenderToTextureRT->Height);
}


bool FrontendWarZ::Unload()
{
#if ENABLE_WEB_BROWSER
	d_show_browser->SetBool(false);
	g_pBrowserManager->SetSize(4, 4);
#endif

	return UIMenu::Unload();
}

extern void InputUpdate();
int FrontendWarZ::Update()
{
	struct EnableDisableDistanceCull
	{
		EnableDisableDistanceCull()
		{
			oldValue = r_allow_distance_cull->GetInt();
			r_allow_distance_cull->SetInt( 0 );
		}

		~EnableDisableDistanceCull()
		{
			r_allow_distance_cull->SetInt( oldValue );
		}

		int oldValue;

	} enableDisableDistanceCull; (void)enableDisableDistanceCull;

	if(needReInitScaleformTexture)
	{
		if (RTScaleformTexture && Scaleform_RenderToTextureRT)
			RTScaleformTexture->Initialize(Scaleform_RenderToTextureRT->AsTex2D());
		needReInitScaleformTexture = false;
	}


	if(gSteam.inited_)
		SteamAPI_RunCallbacks();

	InputUpdate();

	{
		r3dPoint3D soundPos(0,0,0), soundDir(0,0,1), soundUp(0,1,0);
		SoundSys.Update(soundPos, soundDir, soundUp);
	}

	{
		extern int tempCountInsertKeyPressedNum;
		if(Keyboard->WasPressed(kbsIns))
			tempCountInsertKeyPressedNum++;
		extern int tempCountDeleteKeyPressedNum;
		if(Keyboard->WasPressed(kbsDel))
			tempCountDeleteKeyPressedNum++;
		extern int tempCountMinusKeyPressedNum;
		if(Keyboard->WasPressed(kbsGrayMinus))
			tempCountMinusKeyPressedNum++;
	}

	if(frontendStage == 0) // login stage
	{
		// run temp drawing loop
		extern void tempDoMsgLoop();
		tempDoMsgLoop();

		float elapsedTime = r3dGetTime() - loginProcessStartTime;
		float progress = R3D_CLAMP(elapsedTime/2.0f, 0.0f, 1.0f); 
		if(loginMsgBoxOK_Exit)
			progress = 0;

		gfxMovie.Invoke("_root.api.updateLoginMsg", progress);

		r3dStartFrame();
		if( r3dRenderer->DeviceAvailable )
		{
			r3dRenderer->StartFrame();
			r3dRenderer->StartRender(1);

			r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);

			gfxMovie.UpdateAndDraw();

			r3dRenderer->Flush();
			CurRenderPipeline->Finalize() ;
			r3dRenderer->EndFrame();
		}

		r3dRenderer->EndRender( true );

		// process d3d device queue, keeping 20fps for rendering
		if( r3dRenderer->DeviceAvailable )
		{
			float endTime = r3dGetTime() + (1.0f / 20);
			while(r3dGetTime() < endTime)
			{
				extern bool ProcessDeviceQueue( float chunkTimeStart, float maxDuration ) ;
				ProcessDeviceQueue(r3dGetTime(), 0.05f);
			}
		}

		r3dEndFrame();

		LoginCheckAnswerCode();
		if(loginThread == NULL)
		{
			bool IsNeedExit();
			if(IsNeedExit())
				return FrontEndShared::RET_Exit;

			if(LoginMenuExitFlag == 1) 
				return FrontEndShared::RET_LoggedIn;
			else if(LoginMenuExitFlag == -1) // error logging in
				return FrontEndShared::RET_Exit;
		}
		
		return 0;
	}

	// we're still retreiving profile
	if(handleGetProfileData != 0 && gProfileIsAquired == 0)
	{
		// run temp drawing loop
		extern void tempDoMsgLoop();
		tempDoMsgLoop();
		
		// replace message with loading stage info
		static int oldStage = -1;
		if(oldStage != gProfileLoadStage)
		{
			oldStage = gProfileLoadStage;

			wchar_t dots[32] = L"";
			for(int i=0; i<gProfileLoadStage; i++) dots[i] = L'.';
			dots[gProfileLoadStage] = 0;
			
			wchar_t info[1024];
			StringCbPrintfW(info, sizeof(info), L"%s\n%s", gLangMngr.getString("RetrievingProfileData"), dots);
			
			//updateInfoMsgText(info);
		}
		{
			float progress = gProfileLoadStage/8.0f;
			gfxMovie.Invoke("_root.api.updateLoginMsg", progress);
		}

		// NOTE: WARNING: DO NOT TOUCH GameWorld() or anything here - background loading thread in progress!
		r3dStartFrame();
		if( r3dRenderer->DeviceAvailable )
		{
			r3dRenderer->StartFrame();
			r3dRenderer->StartRender(1);

			r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);

			gfxMovie.UpdateAndDraw();

			r3dRenderer->Flush();
			CurRenderPipeline->Finalize() ;
			r3dRenderer->EndFrame();
		}

		r3dRenderer->EndRender( true );

		// process d3d device queue, keeping 20fps for rendering
		if( r3dRenderer->DeviceAvailable )
		{
			float endTime = r3dGetTime() + (1.0f / 20);
			while(r3dGetTime() < endTime)
			{
				extern bool ProcessDeviceQueue( float chunkTimeStart, float maxDuration ) ;
				ProcessDeviceQueue(r3dGetTime(), 0.05f);
			}
		}
		
		r3dEndFrame();

		// update browser, so that by the time we get profile our welcome back screen will be ready to show page
#if ENABLE_WEB_BROWSER
		g_pBrowserManager->Update();
#endif

		return 0;
	}

	if(handleGetProfileData != 0)
	{
		// profile is acquired
		r3d_assert(gProfileIsAquired);
		
		if(!gProfileOK)
		{
			r3dOutToLog("Couldn't get profile data! stage: %d\n", gProfileLoadStage);
			return FrontEndShared::RET_Diconnected;
		}

		CloseHandle(handleGetProfileData);
		handleGetProfileData = 0;

		r3dOutToLog( "Acquired base profile data for %f\n", r3dGetTime() - aquireProfileStart );
		if(gUserProfile.AccountStatus >= 200)
		{
			return FrontEndShared::RET_Banned;
		}
		
		r3dResetFrameTime();

		extern void InitGame_Finish();
		InitGame_Finish();

		//
		if (gUserProfile.ProfileDataDirty == 0)
			initFrontend();
		else
		{
			m_ReloadProfile = true;
			m_ReloadTimer = r3dGetTime();

			Scaleform::GFx::Value var[2];

			var[0].SetString("Waiting for profile to finish updating...");
			var[1].SetBoolean(false);
			gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);		
		}
	}

	if (m_ReloadProfile)
	{
		int time = int (r3dGetTime() - m_ReloadTimer);


		if (time > 10)
		{
			if(gUserProfile.GetProfile() != 0)
				return false;

			if (gUserProfile.ProfileDataDirty == 0)
			{
				m_ReloadProfile = false;
				gfxMovie.Invoke("_root.api.hideInfoMsg", "");		
				initFrontend();
			}
			else
			{
				m_ReloadTimer = r3dGetTime();
			}
		}

		return 0; // frontend isn't ready yet, just keep looping until profile will be ready
	}

	// at the moment we must have finished initializing things in background
	r3d_assert(handleGetProfileData == 0);

	extern bool g_warn_about_low_memory;
	if(g_warn_about_low_memory)
	{
		g_warn_about_low_memory = false;

		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("LowSystemMemoryWarning"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);		
	}
	extern bool g_warn_about_full_inventory;
	static bool warned_about_full_inventory = false;
	if(g_warn_about_full_inventory && !warned_about_full_inventory)
	{
		warned_about_full_inventory = true;

		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("FullInventoryWarning"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);		
	}

	/*static bool blablatest = false;
	if(!blablatest)
	{
		blablatest = true;
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("$FF_BanMessage"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);		
	}*/

	if(m_waitingForKeyRemap != -1)
	{
		// query input manager for any input
		bool conflictRemapping = false;
		if(InputMappingMngr->attemptRemapKey((r3dInputMappingMngr::KeybordShortcuts)m_waitingForKeyRemap, conflictRemapping))
		{
			Scaleform::GFx::Value var[2];
			var[0].SetNumber(m_waitingForKeyRemap);
			var[1].SetString(InputMappingMngr->getKeyName((r3dInputMappingMngr::KeybordShortcuts)m_waitingForKeyRemap));
			gfxMovie.Invoke("_root.api.updateKeyboardMapping", var, 2);
			m_waitingForKeyRemap = -1;

			void writeInputMap();
			writeInputMap();

			if(conflictRemapping)
			{
				Scaleform::GFx::Value var[2];
				var[0].SetString(gLangMngr.getString("ConflictRemappingKeys"));
				var[1].SetBoolean(true);
				gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);		
			}
		}
	}

	if(gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].Alive == 0 && gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].Hardcore == 0) // dead
	{
		// for now, use hard coded revive time
		const int timeToReviveInSec = gUserProfile.ProfileData.PremiumAcc > 0 ? 10*60 : 20*60; // 10 and 20 min

		Scaleform::GFx::Value var[3];

		int timeLeftToRevive = R3D_MAX(gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].SecToRevive - int(r3dGetTime() - gTimeWhenProfileLoaded), 0);
		var[0].SetInt(timeLeftToRevive);
		int perc = 100-int((float(timeLeftToRevive)/float(timeToReviveInSec))*100.0f);
		var[1].SetInt(perc);
#ifdef FINAL_BUILD
		var[2].SetBoolean(timeLeftToRevive<=(timeToReviveInSec-0)); // show revive button after 0 minutes after death to prevent abuse
#else
		var[2].SetBoolean(gUserProfile.ProfileData.isDevAccount?true:(timeLeftToRevive<=(timeToReviveInSec-0)));
#endif
		gfxMovie.Invoke("_root.api.updateDeadTimer", var, 3);
	}
	m_Player->Darkness = 0;
	if(gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].Alive == 0 && gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].Hardcore == 1)
		m_Player->Darkness = 1;

#if ENABLE_WEB_BROWSER
	g_pBrowserManager->Update();
#endif

	settingsChangeFlags_ = 0;

	r3dMouse::Show();

	extern void tempDoMsgLoop();
	tempDoMsgLoop();

	m_Player->UpdateTransform();
	r3dPoint3D size = m_Player->GetBBoxLocal().Size;

	float distance = GetOptimalDist(size, 22.5f);

	r3dPoint3D camPos(0.0f, size.y * 0.7f, distance*1.0f);
	r3dPoint3D playerPosHome(0, 0.38f, 0);
	r3dPoint3D playerPosCreate(0, 0.38f, 0);

	float backupFOV = gCam.FOV;
	gCam = camPos;
	gCam.vPointTo = (r3dPoint3D(0, 1.0, 0) - gCam).NormalizeTo();
	gCam.FOV = 45;

	gCam.SetPlanes(0.01f, 200.0f);
	if(m_needPlayerRenderingRequest==1) // home
		m_Player->SetPosition(playerPosHome);	
	else if(m_needPlayerRenderingRequest==2) // create
		m_Player->SetPosition(playerPosCreate);
	else if(m_needPlayerRenderingRequest==3) // play game screen
		m_Player->SetPosition(playerPosCreate);

	m_Player->m_fPlayerRotationTarget = m_Player->m_fPlayerRotation = 0;

	GameWorld().StartFrame();
	r3dRenderer->SetCamera( gCam, true );

#if R3D_ALLOW_SECTOR_LOADING
	if( r_sector_loading->GetInt() )
	{
		g_pSectorMaster->Update( gCam );
	}
#endif

	GameWorld().Update();

	ProcessAsyncOperation();

	gfxMovie.SetCurentRTViewport( Scaleform::GFx::Movie::SM_ExactFit );

	r3dStartFrame();

	if( r3dRenderer->DeviceAvailable )
	{
		r3dRenderer->StartFrame();

		r3dRenderer->StartRender(1);

		//r3d_assert(m_pBackgroundPremiumTex);
		r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA | R3D_BLEND_NZ);
		r3dColor backgroundColor = r3dColor::white;

		if(m_needPlayerRenderingRequest)
			drawPlayer() ;

		gfxMovie.UpdateAndDraw();

		r3dRenderer->Flush();

		CurRenderPipeline->Finalize() ;

		r3dRenderer->EndFrame();
	}

	if( r3dRenderer->DeviceAvailable )
	{
		r3dUpdateScreenShot();
		if(Keyboard->WasPressed(kbsPrtScr))
			r3dToggleScreenShot();
	}

	r3dRenderer->EndRender( true );
	
	GameWorld().EndFrame();
	r3dEndFrame();

	if( needUpdateSettings_ )
	{
		UpdateSettings();
		needUpdateSettings_ = false;
	}

	if(gMasterServerLogic.IsConnected() && asyncThread_ == NULL)
	{
		if(r3dGetTime() > masterConnectTime_ + _p2p_idleTime)
		{
			masterConnectTime_ = -1;
			gMasterServerLogic.Disconnect();
		}
		
		if(gMasterServerLogic.shuttingDown_)
		{
			gMasterServerLogic.Disconnect();
			
			Scaleform::GFx::Value var[2];
			var[0].SetString(gLangMngr.getString("MSShutdown1"));
			var[1].SetBoolean(true);
			gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);		
		}
	}

	if(asyncThread_ == NULL)
	{
		bool IsNeedExit();
		if(IsNeedExit())
			return FrontEndShared::RET_Exit;
		
		if(exitRequested_)
			return FrontEndShared::RET_Exit;

		if(!gLoginSessionPoller.IsConnected()) {
			//@TODO: set var, display message and exit
			r3dError("double login");
		}

		if(needExitByGameJoin_)
		{
			if(!gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].Alive)
			{
				needExitByGameJoin_ = false;

				Scaleform::GFx::Value var[2];
				var[0].SetString(gLangMngr.getString("$FR_PLAY_GAME_SURVIVOR_DEAD"));
				var[1].SetBoolean(true);
				gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);		
				return 0;
			}
			return FrontEndShared::RET_JoinGame;
		}
	}

	return 0;
}

void FrontendWarZ::drawPlayer()
{
	struct BeginEndEvent
	{
		BeginEndEvent()
		{
			D3DPERF_BeginEvent( 0, L"FrontendUI::drawPlayer" ) ;
		}
		
		~BeginEndEvent()
		{
			D3DPERF_EndEvent() ;
		}
	} beginEndEvent ;

	CurRenderPipeline->PreRender();
	CurRenderPipeline->Render();

	CurRenderPipeline->AppendPostFXes();

	{
#if 0
		PFX_Fill::Settings efsts ;

		efsts.Value = float4( r_gameui_exposure->GetFloat(), 0.f, 0.f, 0.f ) ;

		gPFX_Fill.PushSettings( efsts ) ;
		g_pPostFXChief->AddFX( gPFX_Fill, PostFXChief::RTT_SCENE_EXPOSURE0, PostFXChief::RTT_AVG_SCENE_LUMA );
		gPFX_Fill.PushSettings( efsts ) ;
		g_pPostFXChief->AddFX( gPFX_Fill, PostFXChief::RTT_SCENE_EXPOSURE1, PostFXChief::RTT_AVG_SCENE_LUMA );

		g_pPostFXChief->AddFX( gPFX_ConvertToLDR );
		g_pPostFXChief->AddSwapBuffers();
#endif

		PFX_Fill::Settings fsts;

		fsts.ColorWriteMask = D3DCOLORWRITEENABLE_ALPHA;			

		gPFX_Fill.PushSettings( fsts );

		g_pPostFXChief->AddFX( gPFX_Fill, PostFXChief::RTT_PINGPONG_LAST, PostFXChief::RTT_DIFFUSE_32BIT );

		PFX_StencilToMask::Settings ssts;

		ssts.Value = float4( 0, 0, 0, 1 );

		gPFX_StencilToMask.PushSettings( ssts );

		g_pPostFXChief->AddFX( gPFX_StencilToMask, PostFXChief::RTT_PINGPONG_LAST );

		{
			r3dScreenBuffer* buf = g_pPostFXChief->GetBuffer( PostFXChief::RTT_PINGPONG_LAST ) ;
			r3dScreenBuffer* buf_scaleform = g_pPostFXChief->GetBuffer( PostFXChief::RTT_UI_CHARACTER_32BIT ) ;

			PFX_Copy::Settings sts ;

			sts.TexScaleX = 1.0f;
			sts.TexScaleY = 1.0f;
			sts.TexOffsetX = 0.0f;
			sts.TexOffsetY = 0.0f;

			gPFX_Copy.PushSettings( sts ) ;

			g_pPostFXChief->AddFX( gPFX_Copy, PostFXChief::RTT_UI_CHARACTER_32BIT ) ;
		}

		g_pPostFXChief->Execute( false, true );
	}

	r3dRenderer->SetVertexShader();
	r3dRenderer->SetPixelShader();
}

void FrontendWarZ::StartAsyncOperation(fn_thread threadFn, fn_finish finishFn)
{
	r3d_assert(asyncThread_ == NULL);

	asyncFinish_ = finishFn;
	asyncErr_[0] = 0;
	asyncThread_ = (HANDLE)_beginthreadex(NULL, 0, threadFn, this, 0, NULL);
	if(asyncThread_ == NULL)
		r3dError("Failed to begin thread");
}

void FrontendWarZ::SetAsyncError(int apiCode, const char* msg)
{
	if(gMasterServerLogic.shuttingDown_)
	{
		sprintf(asyncErr_, "%s", gLangMngr.getString("MSShutdown1"));
		return;
	}

	if(apiCode == 0) {
		sprintf(asyncErr_, "%s", msg);
	} else {
		sprintf(asyncErr_, "%s, code:%d", msg, apiCode);
	}
}

void FrontendWarZ::ProcessAsyncOperation()
{
	if(asyncThread_ == NULL)
		return;

	DWORD w0 = WaitForSingleObject(asyncThread_, 0);
	if(w0 == WAIT_TIMEOUT) 
		return;

	CloseHandle(asyncThread_);
	asyncThread_ = NULL;
	
	if(gMasterServerLogic.badClientVersion_)
	{
		Scaleform::GFx::Value args[2];
		args[0].SetString(gLangMngr.getString("ClientMustBeUpdated"));
		args[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", args, 2);		
		//@TODO: on infoMsg closing, exit app.
		return;
	}

	if(asyncErr_[0]) 
	{
		Scaleform::GFx::Value args[2];
		args[0].SetString(asyncErr_);
		args[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", args, 2);		
		return;
	}
	
	if(asyncFinish_)
		(this->*asyncFinish_)();
}

void FrontendWarZ::addClientSurvivor(const wiCharDataFull& slot, int slotIndex)
{
	Scaleform::GFx::Value var[27];
	char tmpGamertag[128];
	if(slot.ClanID != 0)
		sprintf(tmpGamertag, "[%s] %s", slot.ClanTag, slot.Gamertag);
	else
		r3dscpy(tmpGamertag, slot.Gamertag);
	var[0].SetString(tmpGamertag);
	var[1].SetNumber(slot.Health);
	var[2].SetNumber(slot.Stats.XP);
	var[3].SetNumber(slot.Stats.TimePlayed);
	var[4].SetNumber(slot.Hardcore);
	var[5].SetNumber(slot.HeroItemID);
	var[6].SetNumber(slot.HeadIdx);
	var[7].SetNumber(slot.BodyIdx);
	var[8].SetNumber(slot.LegsIdx);
	var[9].SetNumber(slot.Alive);
	var[10].SetNumber(slot.Hunger);
	var[11].SetNumber(slot.Thirst);
	var[12].SetNumber(slot.Toxic);
	var[13].SetNumber(slot.BackpackID);
	var[14].SetNumber(slot.BackpackSize);

	float totalWeight = slot.getTotalWeight();
	if(slot.Skills[CUserSkills::SKILL_Physical3])
		totalWeight *= 0.95f;
	if(slot.Skills[CUserSkills::SKILL_Physical7])
		totalWeight *= 0.9f;
	var[15].SetNumber(totalWeight);		// weight
	var[16].SetNumber(slot.Stats.KilledZombies);		// zombies Killed
	var[17].SetNumber(slot.Stats.KilledBandits);		// bandits killed
	var[18].SetNumber(slot.Stats.KilledSurvivors);		// civilians killed
	char repS[64];
	sprintf(repS, "%s : %d", getReputationString(slot.Stats.Reputation), slot.Stats.Reputation);
	var[19].SetString(repS);	// alignment

	const char* lastMap = getMapName(slot.GameMapId);
	var[20].SetString(lastMap);

	var[21].SetBoolean(slot.GameFlags & wiCharDataFull::GAMEFLAG_NearPostBox);
	var[22].SetInt(slot.Stats.XP - slot.Stats.SpendXP);
	/*char kdrS[32];
	sprintf(kdrS, "%.2f", getRatio(float(slot.Stats.KilledSurvivors + slot.Stats.KilledBandits), float(slot.Stats.Deaths)));
	var[23].SetString(kdrS); // kdr
	char hsrS[32];
	sprintf(hsrS, "%.2f", getRatio(float(slot.Stats.ShotsHeadshots), float(slot.Stats.ShotsFired)));
	var[24].SetString(hsrS); // hsr
	var[25].SetInt(int(getRatio((float)slot.Stats.ShotsHits, (float)slot.Stats.ShotsFired)*100.0f)); // accuracy
	var[26].SetInt(slot.Stats.Deaths); // died*/

	gfxMovie.Invoke("_root.api.addClientSurvivor", var, 23);

	addBackpackItems(slot, slotIndex);

	for(int i=0; i<34; ++i)
	{
		if(slot.Skills[i]>0)
		{
			var[1].SetInt(i);
			gfxMovie.Invoke("_root.api.setSkillLearnedSurvivor", var, 2);
		}
	}
}

void FrontendWarZ::addBackpackItems(const wiCharDataFull& slot, int slotIndex)
{
	Scaleform::GFx::Value var[8];
	for (int a = 0; a < slot.BackpackSize; a++)
	{
		if (slot.Items[a].InventoryID != 0)
		{
			var[0].SetInt(slotIndex);
			var[1].SetInt(a);
			var[2].SetUInt(uint32_t(slot.Items[a].InventoryID));
			var[3].SetUInt(slot.Items[a].itemID);
			var[4].SetInt(slot.Items[a].quantity);
			var[5].SetInt(slot.Items[a].Var1);
			var[6].SetInt(slot.Items[a].Var2);
			var[7].SetString(getAdditionalDescForItem(slot.Items[a].itemID, slot.Items[a].Var1, slot.Items[a].Var2, slot.Items[a].Var3));
			gfxMovie.Invoke("_root.api.addBackpackItem", var, 8);
		}
	}
}

void FrontendWarZ::initVoipOptions()
{
	gfxMovie.Invoke("_root.api.Main.OptionsVoip.clearDevicesArray", NULL, 0);

	if(gTeamSpeakClient.m_playbackDevices.size() == 0 && gTeamSpeakClient.m_captureDevices.size() == 0)
	{
		g_voip_enable->SetBool(false);
		writeGameOptionsFile();
		return;
	}
	
	// reset index in case when we're out of current devices count
	if(g_voip_InputDeviceInd->GetInt() >= (int)gTeamSpeakClient.m_captureDevices.size())
		g_voip_InputDeviceInd->SetInt(-1);
	if(g_voip_OutputDeviceInd->GetInt() >= (int)gTeamSpeakClient.m_playbackDevices.size())
		g_voip_OutputDeviceInd->SetInt(-1);
	
	for(size_t i=0; i<gTeamSpeakClient.m_playbackDevices.size(); i++)
	{
		const CTeamSpeakClient::dev_s& dev = gTeamSpeakClient.m_playbackDevices[i];
		Scaleform::GFx::Value var[1];
		var[0].SetString(dev.name.c_str());
		gfxMovie.Invoke("_root.api.Main.OptionsVoip.addOutputDevice", var, 1);
		
		if(g_voip_OutputDeviceInd->GetInt() == -1 && dev.isDefault)
			g_voip_OutputDeviceInd->SetInt(i);
	}

	for(size_t i=0; i<gTeamSpeakClient.m_captureDevices.size(); i++)
	{
		const CTeamSpeakClient::dev_s& dev = gTeamSpeakClient.m_captureDevices[i];
		Scaleform::GFx::Value var[1];
		var[0].SetString(dev.name.c_str());
		gfxMovie.Invoke("_root.api.Main.OptionsVoip.addInputDevice", var, 1);
		
		if(g_voip_InputDeviceInd->GetInt() == -1 && dev.isDefault)
			g_voip_InputDeviceInd->SetInt(i);
	}

	writeGameOptionsFile();
}

void FrontendWarZ::initFrontend()
{
	if(dictionaryHash_.Size() == 0)
	{
		r3dFile* f = r3d_open( "Data/LangPack/dictionary.dat", "rb" );
		char tmpStr[256];
		while(fgets(tmpStr, 256, f) != NULL)
		{
			size_t len = strlen(tmpStr);
			for(size_t i=0; i<len; ++i)
			{
				if(tmpStr[i]==13 || tmpStr[i]==10)
					tmpStr[i]=0;
			}
			dictionaryHash_.Add(tmpStr, tmpStr);	
		}
		fclose(f);
	}

	initItems();

	// tutorial screens
	{
		if(g_shown_marketplace_tutorial->GetBool() == false)
		{
			gfxMovie.SetVariable("_root.api.Main.Marketplace.Marketplace.Tutorial.visible", true);
		}
	}

	// add skill info
	{
		// addSkillInfo(id:uint, name:String, desc:String, icon:String, iconBW:String, cost:uint)
		Scaleform::GFx::Value var[6];
		char tmpStr[64];
		for(int i=0; i<34; ++i)
		{
			var[0].SetUInt(i);
			sprintf(tmpStr, "SkillName%d", i);
			var[1].SetString(gLangMngr.getString(tmpStr));
			sprintf(tmpStr, "SkillDesc%d", i);
			var[2].SetString(gLangMngr.getString(tmpStr));
			char iconPath[512];
			sprintf(iconPath, "$Data/Menu/skillIcons/Skill%d.dds", i);
			var[3].SetString(iconPath);
			char iconPathBW[512];
			sprintf(iconPathBW, "$Data/Menu/skillIcons/Skill%dBW.dds", i);
			var[4].SetString(iconPathBW);
			var[5].SetUInt(gUserProfile.ShopSkillCosts[i][0]);
			gfxMovie.Invoke("_root.api.addSkillInfo", var, 6);
		}
	}
	
	initVoipOptions();

	Scaleform::GFx::Value var[20];
	// init browse channel buttons
	{
		var[0].SetBoolean(true); // trial channel
		var[1].SetBoolean(!gUserProfile.IsTrialAccount()); // official channel
		var[2].SetBoolean(!gUserProfile.IsTrialAccount()); // private servers
		var[3].SetBoolean(gUserProfile.ShopPremiumAcc>0 || gUserProfile.ProfileData.PremiumAcc>0); // premium servers (enable only if premium is available for sale or player alreayd has premium account
		var[4].SetBoolean(!gUserProfile.IsTrialAccount()); // strongholds
		var[5].SetBoolean(!gUserProfile.IsTrialAccount()); // public test environment
		var[6].SetBoolean(!gUserProfile.IsTrialAccount()); // veteran servers
		gfxMovie.Invoke("_root.api.Main.BrowseGamesChannelsAnim.initButtons", var, 7);
	}

	gfxMovie.SetVariable("_root.api.isTrialAccount", gUserProfile.IsTrialAccount());
	if(gUserProfile.IsTrialAccount())
	{
		gfxMovie.SetVariable("_root.api.Main.SurvivorsAnim.Survivors.BtnUpgtrial.visible", true);
	}
	if(gUserProfile.ProfileData.PremiumAcc>0) 
	{
		gfxMovie.SetVariable("_root.api.Main.SurvivorsAnim.Survivors.PremiumAcc.visible", true);
	}

	gfxMovie.Invoke("_root.api.Main.CreateSurv.EnableHardcore", "");

	// send survivor info
	for(int i=0; i< gUserProfile.ProfileData.NumSlots; ++i)
	{
		addClientSurvivor(gUserProfile.ProfileData.ArmorySlots[i], i);
	}

	updateSurvivorTotalWeight(gUserProfile.SelectedCharID);

	var[0].SetInt(gUserProfile.ProfileData.GamePoints);
	gfxMovie.Invoke("_root.api.setGC", var, 1);

	var[0].SetInt(gUserProfile.ProfileData.GameDollars);
	gfxMovie.Invoke("_root.api.setDollars", var, 1);

	var[0].SetInt(0);
	gfxMovie.Invoke("_root.api.setCells", var, 1);

	gfxMovie.SetVariable("_root.api.EarlyRevival_Price", gUserProfile.ShopCharRevive);
	gfxMovie.SetVariable("_root.api.PremiumAccount_Price", gUserProfile.ShopPremiumAcc);
	gfxMovie.SetVariable("_root.api.ChangeName_Price", gUserProfile.ShopCharRename);

	for(int i=0; i<r3dInputMappingMngr::KS_NUM; ++i)
	{
		Scaleform::GFx::Value args[2];
		args[0].SetString(gLangMngr.getString(InputMappingMngr->getMapName((r3dInputMappingMngr::KeybordShortcuts)i)));
		args[1].SetString(InputMappingMngr->getKeyName((r3dInputMappingMngr::KeybordShortcuts)i));
		gfxMovie.Invoke("_root.api.addKeyboardMapping", args, 2);
	}

	SyncGraphicsUI();

	gfxMovie.SetVariable("_root.api.SelectedChar", gUserProfile.SelectedCharID);
	m_Player->uberAnim_->anim.StopAll();	
	m_Player->UpdateLoadoutSlot(gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID], -1);

	gfxMovie.Invoke("_root.api.showSurvivorsScreen", "");

	// check if user has russian language set and if he has proper russian client
/*	if(strcmp(g_user_language->GetString(), "russian")==0 && !gUserProfile.IsRussianAccount())
	{
		g_user_language->SetString("english"); // disable russian for non russian client users
		writeGameOptionsFile();
		ReloadLocalization();
	}*/
	/*if(!gUserProfile.IsRussianAccount())
		gfxMovie.SetVariable("_root.api.Main.OptionsLang.OptionsLangClip.Lang6.visible", false);*/

	gfxMovie.Invoke("_root.api.setLanguage", g_user_language->GetString());

	// init clan icons
	// important: DO NOT CHANGE THE ORDER OF ICONS!!! EVER!
	{
		gfxMovie.Invoke("_root.api.addClanIcon", "$Data/Menu/clanIcons/clan_survivor.dds");
		gfxMovie.Invoke("_root.api.addClanIcon", "$Data/Menu/clanIcons/clan_bandit.dds");
		gfxMovie.Invoke("_root.api.addClanIcon", "$Data/Menu/clanIcons/clan_lawman.dds");
		// add new icons at the end!
	}
	{
		//public function addClanSlotBuyInfo(buyIdx:uint, price:uint, numSlots:uint)
		Scaleform::GFx::Value var[3];
		for(int i=0; i<6; ++i)
		{
			var[0].SetUInt(i);
			var[1].SetUInt(gUserProfile.ShopClanAddMembers_GP[i]);
			var[2].SetUInt(gUserProfile.ShopClanAddMembers_Num[i]);
			gfxMovie.Invoke("_root.api.addClanSlotBuyInfo", var, 3);
		}
	}

	// add server rental info
	{
		Scaleform::GFx::Value var[3];
		var[0].SetInt(GBGameInfo::MAPID_WZ_Colorado);
		var[1].SetString(gLangMngr.getString("MapColorado"));
		gfxMovie.Invoke("_root.api.addRentServer_MapInfo", var, 2);

		var[0].SetInt(GBGameInfo::MAPID_WZ_Cliffside); // need id
		var[1].SetString(gLangMngr.getString("MapCliffside"));
		gfxMovie.Invoke("_root.api.addRentServer_StrongholdInfo", var, 2);

		var[0].SetInt(GBNET_REGION_US_West);
		var[1].SetString(gLangMngr.getString("$FR_REGION_NA"));
		gfxMovie.Invoke("_root.api.addRentServer_RegionInfo", var, 2);

		var[0].SetInt(GBNET_REGION_Europe);
		var[1].SetString(gLangMngr.getString("$FR_REGION_EU"));
		gfxMovie.Invoke("_root.api.addRentServer_RegionInfo", var, 2);

		var[0].SetInt(GBNET_REGION_SouthAmerica);
		var[1].SetString(gLangMngr.getString("$FR_REGION_SA"));
		gfxMovie.Invoke("_root.api.addRentServer_RegionInfo", var, 2);

		// MAKE SURE TO SYNC slot numbers with backend API api_ServersMgr::GameworldSlots
		// MAKE SURE TO SYNC slot numbers with eventRenewServerUpdatePrice
		var[0].SetInt(0);
		var[1].SetString("10");
		var[2].SetBoolean(true);
		gfxMovie.Invoke("_root.api.addRentServer_SlotsInfo", var, 3);
		var[0].SetInt(1);
		var[1].SetString("30");
		var[2].SetBoolean(true);
		gfxMovie.Invoke("_root.api.addRentServer_SlotsInfo", var, 3);
		var[0].SetInt(2);
		var[1].SetString("50");
		var[2].SetBoolean(true);
		gfxMovie.Invoke("_root.api.addRentServer_SlotsInfo", var, 3);
		var[0].SetInt(3);
		var[1].SetString("70");
		var[2].SetBoolean(true);
		gfxMovie.Invoke("_root.api.addRentServer_SlotsInfo", var, 3);
		var[0].SetInt(4);
		var[1].SetString("100");
		var[2].SetBoolean(true);
		gfxMovie.Invoke("_root.api.addRentServer_SlotsInfo", var, 3);

		// MAKE SURE TO SYNC actual slot numbers with backend API api_ServersMgr::StrongholdSlots
		// MAKE SURE TO SYNC slot numbers with eventRenewServerUpdatePrice
		var[0].SetInt(0);
		var[1].SetString("10");
		var[2].SetBoolean(true);
		gfxMovie.Invoke("_root.api.addRentServer_SlotsInfoStronghold", var, 3);
		var[0].SetInt(1);
		var[1].SetString("20");
		var[2].SetBoolean(true);
		gfxMovie.Invoke("_root.api.addRentServer_SlotsInfoStronghold", var, 3);
		var[0].SetInt(2);
		var[1].SetString("30");
		var[2].SetBoolean(true);
		gfxMovie.Invoke("_root.api.addRentServer_SlotsInfoStronghold", var, 3);
		var[0].SetInt(3);
		var[1].SetString("40");
		var[2].SetBoolean(true);
		gfxMovie.Invoke("_root.api.addRentServer_SlotsInfoStronghold", var, 3);
		var[0].SetInt(4);
		var[1].SetString("50");
		var[2].SetBoolean(true);
		gfxMovie.Invoke("_root.api.addRentServer_SlotsInfoStronghold", var, 3);

		var[0].SetInt(0); // rent ID
		var[1].SetUInt(3); // number of months\weeks
		var[2].SetString(gLangMngr.getString("Days")); // string
		gfxMovie.Invoke("_root.api.addRentServer_RentInfo", var, 3);
		var[0].SetInt(1); // rent ID
		var[1].SetUInt(7); // number of months\weeks
		var[2].SetString(gLangMngr.getString("Days")); // string
		gfxMovie.Invoke("_root.api.addRentServer_RentInfo", var, 3);
		var[0].SetInt(2);
		var[1].SetUInt(15);
		var[2].SetString(gLangMngr.getString("Days"));
		gfxMovie.Invoke("_root.api.addRentServer_RentInfo", var, 3);
		var[0].SetInt(3);
		var[1].SetUInt(30);
		var[2].SetString(gLangMngr.getString("Days"));
		gfxMovie.Invoke("_root.api.addRentServer_RentInfo", var, 3);
		var[0].SetInt(4);
		var[1].SetUInt(60);
		var[2].SetString(gLangMngr.getString("Days"));
		gfxMovie.Invoke("_root.api.addRentServer_RentInfo", var, 3);

		var[0].SetInt(0);
		var[1].SetString(gLangMngr.getString("$FR_optNo"));
		gfxMovie.Invoke("_root.api.addRentServer_PVEInfo", var, 2);
		// disabled until proper design
		//var[0].SetInt(1);
		//var[1].SetString(gLangMngr.getString("$FR_optYes"));
		//gfxMovie.Invoke("_root.api.addRentServer_PVEInfo", var, 2);
	}

	gfxMovie.Invoke("_root.api.hideLoginMsg", "");

	m_waitingForKeyRemap = -1;
	m_needPlayerRenderingRequest = 1; // by default when FrontendInit we are in home screen, so show player

	{
		extern char g_Custom_KickMsg[256];
		Scaleform::GFx::Value vars[3];
		vars[1].SetBoolean(true);
		vars[2].SetString("$ERROR");
		switch(prevGameResult)
		{
		case GRESULT_Failed_To_Join_Game:
			vars[0].SetString(gLangMngr.getString("FailedToJoinGame"));
			gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
			break;
		case GRESULT_Timeout:
			vars[0].SetString(gLangMngr.getString("TimeoutJoiningGame"));
			gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
			break;
		case GRESULT_StillInGame:
			vars[0].SetString(gLangMngr.getString("FailedToJoinGameStillInGame"));
			gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
			break;
		case GRESULT_Disconnect:
			if(g_Custom_KickMsg[0])
			{
				vars[0].SetString(g_Custom_KickMsg);
				gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
				memset(g_Custom_KickMsg, 0, sizeof(g_Custom_KickMsg));
			}
			else
			{
				vars[0].SetString(gLangMngr.getString("DisconnectMsg"));
				gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
			}
			break;
		}
	}
	prevGameResult = GRESULT_Unknown;
}

void FrontendWarZ::eventPlayGame(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	if(gUserProfile.ProfileData.NumSlots == 0)
		return;
		
	StartAsyncOperation(&FrontendWarZ::as_PlayGameThread);
}

void FrontendWarZ::eventCancelQuickGameSearch(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	CancelQuickJoinRequest = true;
}

void FrontendWarZ::eventQuitGame(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	exitRequested_ = true;
}

void FrontendWarZ::eventCreateCharacter(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3dscpy(m_CreateGamerTag, args[0].GetString()); // gamertag
	m_CreateHeroID = args[1].GetInt(); // hero
	m_CreateGameMode = args[2].GetInt(); // mode
	m_CreateHeadIdx = args[3].GetInt(); // bodyID
	m_CreateBodyIdx = args[4].GetInt(); // headID
	m_CreateLegsIdx = args[5].GetInt(); // legsID

	if (gUserProfile.ProfileData.PremiumAcc <= 0 && m_CreateGameMode == 1)
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("$FR_PremiumAccountRequired"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	if(gUserProfile.IsTrialAccount())
	{
		// only one survivor allowed for trial accounts
		if(gUserProfile.ProfileData.NumSlots > 0)
		{
			Scaleform::GFx::Value var[2];
			var[0].SetString(gLangMngr.getString("TrialAccountLimit_SurvivorSlots"));
			var[1].SetString(gLangMngr.getString("$TrialAccountLimit"));
			gfxMovie.Invoke("_root.api.showTrialAccountLimit", var, 2);
			return;
		}
	}

	if(strpbrk(m_CreateGamerTag, "!@#$%^&*()-=+_<>,./?'\":;|{}[]")!=NULL) // do not allow this symbols
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("CharacterNameCannotContaintSpecialSymbols"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}
	// check profanity filter
	{
		bool hasProfanity = false;
		dictionaryHash_.IterateStart();
		while ( dictionaryHash_.IterateNext() )
		{
			const char* profanityEntry = dictionaryHash_.IterateGetKey().c_str();
			if(stristr(m_CreateGamerTag, profanityEntry))
			{
				hasProfanity = true;
				break;
			}
		}
		if(hasProfanity)
		{
			Scaleform::GFx::Value var[2];
			var[0].SetString(gLangMngr.getString("CharacterNameCannotContainProfanity"));
			var[1].SetBoolean(true);
			gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
			return;
		}
	}

	if(gUserProfile.ProfileData.NumSlots >= 5) 
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("CannotCreateMoreThan5Char"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	StartAsyncOperation(&FrontendWarZ::as_CreateCharThread, &FrontendWarZ::OnCreateCharSuccess);
}

void FrontendWarZ::eventRenameCharacter(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3dscpy(m_CreateGamerTag, args[0].GetString()); // gamertag

	if(strpbrk(m_CreateGamerTag, "!@#$%^&*()-=+_<>,./?'\":;|{}[]")!=NULL) // do not allow this symbols
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("CharacterNameCannotContaintSpecialSymbols"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}
	// check profanity filter
	{
		bool hasProfanity = false;
		dictionaryHash_.IterateStart();
		while ( dictionaryHash_.IterateNext() )
		{
			const char* profanityEntry = dictionaryHash_.IterateGetKey().c_str();
			if(stristr(m_CreateGamerTag, profanityEntry))
			{
				hasProfanity = true;
				break;
			}
		}
		if(hasProfanity)
		{
			Scaleform::GFx::Value var[2];
			var[0].SetString(gLangMngr.getString("CharacterNameCannotContainProfanity"));
			var[1].SetBoolean(true);
			gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
			return;
		}
	}

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	r3dscpy(CharRename_PreviousName, gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].Gamertag);

	StartAsyncOperation(&FrontendWarZ::as_CharRenameThread, &FrontendWarZ::OnCharRenameSuccess);
}

void FrontendWarZ::eventDeleteChar(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	StartAsyncOperation(&FrontendWarZ::as_DeleteCharThread, &FrontendWarZ::OnDeleteCharSuccess);
}

extern const char* getLevelDirBasedOnLevelID(int mapID);
extern bool ParseXMLFile(const char *xmlFileName, pugi::xml_document &outXml, char *& outBuf);
extern r3dPoint2D getMinimapPosWithExternalSize(const r3dPoint3D& pos, const r3dPoint3D& worldOrigin, const r3dPoint3D& worldSize);
void FrontendWarZ::eventShowSurvivorsMap(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==0);

	wiCharDataFull& w = gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID];
	if(w.Alive != 1)
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("SurvivorIsNotSpawnedYet"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	char HomeDir[512];
	sprintf(HomeDir, "Levels\\%s\\minimap.dds", getLevelDirBasedOnLevelID(w.GameMapId));

	if(r3dFileExists(HomeDir))
	{
		char HomeDir2[512];
		sprintf(HomeDir2, "$%s", HomeDir);
		gfxMovie.Invoke("_root.api.Main.SurvivorsAnim.loadSurvivorMap", HomeDir2);
	}
	else
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("MinimapNotAvailableForThisLevel"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	// we need to load level xml to read minimap size...
	r3dPoint3D minimapOrigin(0,0,0), minimapSize(0,0,0);
	{
		char fname[MAX_PATH];
		sprintf(fname, "Levels\\%s\\LevelData.xml", getLevelDirBasedOnLevelID(w.GameMapId));
		r3d_assert(r3dFileExists(fname));

		pugi::xml_document xmlLevelFile;
		char *levelData = 0;
		ParseXMLFile(fname, xmlLevelFile, levelData);

		pugi::xml_node xmlLevel = xmlLevelFile.child("level");

		minimapOrigin.x = xmlLevel.attribute("minimapOrigin.x").as_float();
		minimapOrigin.z = xmlLevel.attribute("minimapOrigin.z").as_float();
		minimapSize.x = xmlLevel.attribute("minimapSize.x").as_float();  
		minimapSize.z = xmlLevel.attribute("minimapSize.z").as_float();

		delete [] levelData;
	}

	for(int i=0; i<wiUserProfile::MAX_LOADOUT_SLOTS; ++i)
	{
		wiCharDataFull& w2 = gUserProfile.ProfileData.ArmorySlots[i];
		if(w2.Alive == 1 && w2.GameMapId == w.GameMapId)
		{
			r3dPoint2D mapPos = getMinimapPosWithExternalSize(w2.GamePos, minimapOrigin, minimapSize);
			Scaleform::GFx::Value var[3];
			var[0].SetNumber(mapPos.x);
			var[1].SetNumber(mapPos.y);
			var[2].SetString(w2.Gamertag);
			gfxMovie.Invoke("_root.api.Main.SurvivorsAnim.addSurvivorPinToMap", var, 3);
		}
	}

	gfxMovie.SetVariable("_root.api.Main.SurvivorsAnim.Survivors.PopupLastMap.Title.text", getMapName(w.GameMapId));
	gfxMovie.SetVariable("_root.api.Main.SurvivorsAnim.Survivors.PopupLastMap.visible", true);
}

void FrontendWarZ::eventReviveChar(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	ReviveCharSuccessfull = 0;

	StartAsyncOperation(&FrontendWarZ::as_ReviveCharThread, &FrontendWarZ::OnReviveCharSuccess);
}

void FrontendWarZ::eventReviveCharMoney(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	ReviveCharSuccessfull = 0;

	StartAsyncOperation(&FrontendWarZ::as_ReviveCharThread2, &FrontendWarZ::OnReviveCharSuccess);
}

void FrontendWarZ::eventBuyItem(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	mStore_BuyItemID = args[0].GetUInt(); // legsID
	mStore_BuyPrice = args[1].GetInt();
	mStore_BuyPriceGD = args[2].GetInt();

	if(mStore_BuyPriceGD)
	{
		if(gUserProfile.ProfileData.GameDollars < mStore_BuyPriceGD)
		{
			Scaleform::GFx::Value var[2];
			var[0].SetString(gLangMngr.getString("NotEnougMoneyToBuyItem"));
			var[1].SetBoolean(true);
			gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
			return;
		}
	}
	else if (gUserProfile.ProfileData.GamePoints < mStore_BuyPrice)
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("NotEnougMoneyToBuyItem"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}


	if(gUserProfile.haveFreeInventorySlot(false) == false)
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("InGameUI_ErrorMsg_NoInventorySpace"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	if (mStore_BuyItemID == 103018 && gUserProfile.ProfileData.PremiumAcc == 0) // premium required
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("$FR_PremiumAccountRequired"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	StartAsyncOperation(&FrontendWarZ::as_BuyItemThread, &FrontendWarZ::OnBuyItemSuccess);
}

void FrontendWarZ::DelayServerRequest()
{
	// allow only one server request per second
	if(r3dGetTime() < lastServerReqTime_ + 1.0f) {
		::Sleep(1000);
	}
	lastServerReqTime_ = r3dGetTime();
}

bool FrontendWarZ::ConnectToMasterServer()
{
	masterConnectTime_ = r3dGetTime();
	if(gMasterServerLogic.badClientVersion_)
		return false;
	if(gMasterServerLogic.IsConnected())
		return true;
	
	gMasterServerLogic.Disconnect();
	if(!gMasterServerLogic.StartConnect(_p2p_masterHost, _p2p_masterPort))
	{
		SetAsyncError(0, gLangMngr.getString("NoConnectionToMasterServer"));
		return false;
	}

	const float endTime = r3dGetTime() + 30.0f;
	while(r3dGetTime() < endTime)
	{
		::Sleep(10);
		//if(gMasterServerLogic.IsConnected())
		//	return true;

		if(gMasterServerLogic.versionChecked_ && gMasterServerLogic.badClientVersion_)
			return false;
			
		// if we received server id, connection is ok.
		if(gMasterServerLogic.masterServerId_)
		{
			r3d_assert(gMasterServerLogic.versionChecked_);
			return true;
		}
		
		// early timeout by enet connect fail
		if(!gMasterServerLogic.net_->IsStillConnecting())
			break;
	}
	
	needReturnFromQuickJoin = true;
	SetAsyncError(0, gLangMngr.getString("TimeoutToMasterServer"));
	return false;
}

bool FrontendWarZ::WaitForGameJoinAnswer()
{
	const float endTime = r3dGetTime() + 60.0f;
	while(r3dGetTime() < endTime)
	{
		::Sleep(10);

		if(CancelQuickJoinRequest)
		{
			CancelQuickJoinRequest = false;
			break;
		}

		if(!gMasterServerLogic.IsConnected())
		{
			break;
		}

		if(gMasterServerLogic.gameJoinAnswered_)
			return true;
	}
	
	return false;
}

bool FrontendWarZ::ParseGameJoinAnswer()
{
	r3d_assert(gMasterServerLogic.gameJoinAnswered_);
	
	switch(gMasterServerLogic.gameJoinAnswer_.result)
	{
	case GBPKT_M2C_JoinGameAns_s::rOk:
		needExitByGameJoin_ = true;
		return true;
	case GBPKT_M2C_JoinGameAns_s::rMasterStarting:
		SetAsyncError(0, gLangMngr.getString("JoinMasterStarting"));
		return false;
	case GBPKT_M2C_JoinGameAns_s::rNoGames:
		SetAsyncError(0, gLangMngr.getString("JoinGameNoGames"));
		return false;
	case GBPKT_M2C_JoinGameAns_s::rGameFull:
		SetAsyncError(0, gLangMngr.getString("GameIsFull"));
		return false;
	case GBPKT_M2C_JoinGameAns_s::rGameFinished:
		SetAsyncError(0, gLangMngr.getString("GameIsAlmostFinished"));
		return false;
	case GBPKT_M2C_JoinGameAns_s::rGameNotFound:
		SetAsyncError(0, gLangMngr.getString("GameNotFound"));
		return false;
	case GBPKT_M2C_JoinGameAns_s::rWrongPassword:
		SetAsyncError(0, gLangMngr.getString("WrongPassword"));
		return false;
	case GBPKT_M2C_JoinGameAns_s::rLevelTooLow:
		SetAsyncError(0, gLangMngr.getString("GameTooLow"));
		return false;
	case GBPKT_M2C_JoinGameAns_s::rLevelTooHigh:
		SetAsyncError(0, gLangMngr.getString("GameTooHigh"));
		return false;
	case GBPKT_M2C_JoinGameAns_s::rJoinDelayActive:
		SetAsyncError(0, gLangMngr.getString("JoinDelayActive"));
		return false;
	}

	char buf[128];
	sprintf(buf, gLangMngr.getString("UnableToJoinGameCode"), gMasterServerLogic.gameJoinAnswer_.result);
	SetAsyncError(0, buf);
	return  false;
}

unsigned int WINAPI FrontendWarZ::as_PlayGameThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This_ = (FrontendWarZ*)in_data;

	This_->DelayServerRequest();
	if(!This_->ConnectToMasterServer())
		return 0;
		
	NetPacketsGameBrowser::GBPKT_C2M_QuickGameReq_s n;
	n.CustomerID = gUserProfile.CustomerID;
#ifndef FINAL_BUILD
	n.gameMap    = d_use_test_map->GetInt();
#else
	n.gameMap	 = 0xFF;
#endif
	n.region         = GBNET_REGION_US_West;
	n.browseChannel  = (BYTE)This_->CurrentBrowseChannel;
	n.playerGameTime = gUserProfile.ProfileData.TimePlayed/60/60; // convert to hours
		
	gMasterServerLogic.SendJoinQuickGame(n);
	
	if(This_->WaitForGameJoinAnswer() == false)
	{
		This_->SetAsyncError(0, gLangMngr.getString("TimeoutJoiningGame"));
		This_->needReturnFromQuickJoin = true;
		return 0;
	}

	if(This_->ParseGameJoinAnswer())
		return 1;
		
	This_->needReturnFromQuickJoin = true;
	return 0;
}

unsigned int WINAPI FrontendWarZ::as_JoinGameThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This_ = (FrontendWarZ*)in_data;

	This_->DelayServerRequest();
	if(!This_->ConnectToMasterServer())
		return 0;
		
	const float gameStartWaitTime = r3dGetTime() + 120.0f;	// allow 2 mins to start the game
	while(r3dGetTime() < gameStartWaitTime)
	{
		gMasterServerLogic.SendJoinGame(This_->m_joinGameServerId, This_->m_joinGamePwd);
		if(This_->WaitForGameJoinAnswer() == false)
			break;
		
		if(gMasterServerLogic.gameJoinAnswer_.result == GBPKT_M2C_JoinGameAns_s::rGameStarting)
		{
			// game is starting, send new request after some delay
			::Sleep(15000);
			continue;
		}

		return This_->ParseGameJoinAnswer() == true ? 1 : 0;
	}
		
	This_->SetAsyncError(0, gLangMngr.getString("TimeoutJoiningGame"));
	return 0;
}

unsigned int WINAPI FrontendWarZ::as_CreateCharThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;
	
	This->DelayServerRequest();
	
	int apiCode = gUserProfile.ApiCharCreate(This->m_CreateGamerTag, This->m_CreateGameMode, This->m_CreateHeroID, This->m_CreateHeadIdx, This->m_CreateBodyIdx, This->m_CreateLegsIdx);
	if(apiCode != 0)
	{
		if(apiCode == 9)
		{
			This->SetAsyncError(0, gLangMngr.getString("ThisNameIsAlreadyInUse"));
		}
		else if(apiCode == 7)
		{
			This->SetAsyncError(0, gLangMngr.getString("NameContainInvalidSymbols"));
		}
		else
		{
			This->SetAsyncError(apiCode, gLangMngr.getString("CreateCharacterFail"));
		}
		return 0;
	}

	return 1;
}

void FrontendWarZ::OnCreateCharSuccess()
{
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");

	Scaleform::GFx::Value var[20];

	int	i = gUserProfile.ProfileData.NumSlots - 1;
	{
		addClientSurvivor(gUserProfile.ProfileData.ArmorySlots[i], i);
	}

	var[0].SetInt(i);
	gfxMovie.Invoke("_root.api.createCharSuccessful", var, 1);

	gUserProfile.SelectedCharID = i;
	m_Player->uberAnim_->anim.StopAll();	// prevent animation blending on loadout switch
	m_Player->UpdateLoadoutSlot(gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID], -1);
	return;
}

unsigned int WINAPI FrontendWarZ::as_DeleteCharThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;
	
	This->DelayServerRequest();
	
	int apiCode = gUserProfile.ApiCharDelete();
	if(apiCode != 0)
	{
		if(apiCode == 7)
			This->SetAsyncError(0, gLangMngr.getString("CannotDeleteCharThatIsClanLeader"));
		else
			This->SetAsyncError(apiCode, gLangMngr.getString("FailedToDeleteChar"));
		return 0;
	}

	return 1;
}

void FrontendWarZ::OnDeleteCharSuccess()
{
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");

	gfxMovie.Invoke("_root.api.deleteCharSuccessful", "");

	gUserProfile.SelectedCharID = 0;
	m_Player->uberAnim_->anim.StopAll();	// prevent animation blending on loadout switch
	m_Player->UpdateLoadoutSlot(gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID], -1);
}

unsigned int WINAPI FrontendWarZ::as_ReviveCharThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;

	This->DelayServerRequest();

	int needMoney = 0;
	int secToRevive = 0;
	int apiCode = gUserProfile.ApiCharReviveCheck(&needMoney, &secToRevive);
	if(apiCode != 0)
	{
		This->SetAsyncError(apiCode, gLangMngr.getString("FailedToReviveChar"));
		return 0;
	}

	if(needMoney > 0)
	{
		This->ReviveCharSuccessfull = 2;
	}
	else	
	{
		apiCode = gUserProfile.ApiCharRevive(needMoney);
		if(apiCode != 0)
		{
			This->SetAsyncError(apiCode, gLangMngr.getString("FailedToReviveChar"));
			return 0;
		}
		else
			This->ReviveCharSuccessfull = 1;
	}

	return 1;
}

unsigned int WINAPI FrontendWarZ::as_ReviveCharThread2(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;

	This->DelayServerRequest();

	int needMoney = 0;
	int secToRevive = 0;
	int apiCode = gUserProfile.ApiCharReviveCheck(&needMoney, &secToRevive);
	if(apiCode != 0)
	{
		This->SetAsyncError(apiCode, gLangMngr.getString("FailedToReviveChar"));
		return 0;
	}

	if(gUserProfile.ProfileData.GamePoints < gUserProfile.ShopCharRevive)
	{
		This->SetAsyncError(0, gLangMngr.getString("NotEnoughGC"));
		return 0;
	}

	// this function called when user aknowledged that he is okay with paying money for revive
	apiCode = gUserProfile.ApiCharRevive(needMoney);
	if(apiCode != 0)
	{
		This->SetAsyncError(apiCode, gLangMngr.getString("FailedToReviveChar"));
		return 0;
	}
	else
		This->ReviveCharSuccessfull = 1;

	return 1;
}

void FrontendWarZ::OnReviveCharSuccess()
{
	if(ReviveCharSuccessfull == 1)
	{
		gfxMovie.Invoke("_root.api.hideInfoMsg", "");

		// sync what server does. after revive you are allowed to access global inventory
		gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].GameFlags |= wiCharDataFull::GAMEFLAG_NearPostBox;

		gfxMovie.Invoke("_root.api.reviveCharSuccessful", "");
		Scaleform::GFx::Value var[1];
		var[0].SetInt(gUserProfile.ProfileData.GamePoints);
		gfxMovie.Invoke("_root.api.setGC", var, 1);
	}
	else if(ReviveCharSuccessfull == 2)
	{
		gfxMovie.Invoke("_root.api.hideInfoMsg", "");
		gfxMovie.Invoke("_root.api.Main.PopUpEarlyRevival.showPopUp", "");
	}
	ReviveCharSuccessfull = 0;
	return;
}

void FrontendWarZ::initItems()
{
	addItemCategoriesToUI(gfxMovie);

	// add items
	{
		addItemsAndCategoryToUI(gfxMovie);

	}

	updateInventoryAndSkillItems ();
	addStore();
}

void FrontendWarZ::updateInventoryAndSkillItems()
{
	reloadInventoryInfo(gfxMovie);
}

static void addAllItemsToStore()
{
	// reset store and add all items from DB
	g_NumStoreItems = 0;

	#define SET_STOREITEM \
		memset(&st, 0, sizeof(st)); \
		st.itemID = item->m_itemID;\
		st.pricePerm = 60000;\
		st.gd_pricePerm = 0;

	std::vector<const ModelItemConfig*> allItems;
	std::vector<const WeaponConfig*> allWpns;
	std::vector<const GearConfig*> allGear;
	
	g_pWeaponArmory->startItemSearch();
	while(g_pWeaponArmory->searchNextItem())
	{
		uint32_t itemID = g_pWeaponArmory->getCurrentSearchItemID();
		const WeaponConfig* weaponConfig = g_pWeaponArmory->getWeaponConfig(itemID);
		if(weaponConfig)
		{
			allWpns.push_back(weaponConfig);
		}

		const ModelItemConfig* itemConfig = g_pWeaponArmory->getAccountItemConfig(itemID);
		if(itemConfig)
		{
			allItems.push_back(itemConfig);
		}

		const GearConfig* gearConfig = g_pWeaponArmory->getGearConfig(itemID);
		if(gearConfig)
		{
			allGear.push_back(gearConfig);
		}
	}

	const size_t itemSize   = allItems.size();
	const size_t weaponSize = allWpns.size();
	const size_t gearSize   = allGear.size();

	for(size_t i=0; i<itemSize; ++i)
	{
		const ModelItemConfig* item = allItems[i];
		wiStoreItem& st = g_StoreItems[g_NumStoreItems++];
		SET_STOREITEM;
	}

	for(size_t i=0; i<weaponSize; ++i)
	{
		const WeaponConfig* item = allWpns[i];
		wiStoreItem& st = g_StoreItems[g_NumStoreItems++];
		SET_STOREITEM;
	}

	for(size_t i=0; i<gearSize; ++i)
	{
		const GearConfig* item = allGear[i];
		wiStoreItem& st = g_StoreItems[g_NumStoreItems++];
		SET_STOREITEM;
	}
	
	#undef SET_STOREITEM
}

void FrontendWarZ::addStore()
{
#if 0
	// add all items to store for test purpose
	addAllItemsToStore();
#endif	

	addStoreToUI(gfxMovie);
}

unsigned int WINAPI FrontendWarZ::as_BuyItemThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;
	
	This->DelayServerRequest();
	
	int buyIdx = This->StoreDetectBuyIdx();
	if(buyIdx == 0)
	{
		This->SetAsyncError(-1, gLangMngr.getString("BuyItemFailNoIndex"));
		return 0;
	}

	int apiCode = gUserProfile.ApiBuyItem(This->mStore_BuyItemID, buyIdx, &This->m_inventoryID);
	if(apiCode != 0)
	{
		This->SetAsyncError(apiCode, gLangMngr.getString("BuyItemFail"));
		return 0;
	}

	return 1;
}

void FrontendWarZ::OnBuyItemSuccess()
{
	bool isNewItem = true;
	isNewItem = !UpdateInventoryWithBoughtItem();

	Scaleform::GFx::Value var[1];
	var[0].SetInt(gUserProfile.ProfileData.GamePoints);
	gfxMovie.Invoke("_root.api.setGC", var, 1);

	var[0].SetInt(gUserProfile.ProfileData.GameDollars);
	gfxMovie.Invoke("_root.api.setDollars", var, 1);

	gfxMovie.Invoke("_root.api.buyItemSuccessful", "");
	
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");
	return;
}

bool FrontendWarZ::UpdateInventoryWithBoughtItem()
{
	int numItem = gUserProfile.ProfileData.NumItems;

	// check if we bought consumable
	int quantityToAdd = storecat_GetItemBuyStackSize(mStore_BuyItemID);
	int totalQuantity = 1;

	// see if we already have this item in inventory
	bool found = false;
	uint32_t inventoryID = 0;
	int	var1 = -1;
	int	var2 = 0;
	int	var3 = wiInventoryItem::MAX_DURABILITY;

	for(int i=0; i<numItem; ++i)
	{
		if(gUserProfile.ProfileData.Inventory[i].InventoryID == m_inventoryID)
		{
			inventoryID = uint32_t(gUserProfile.ProfileData.Inventory[i].InventoryID);
			var1 = gUserProfile.ProfileData.Inventory[i].Var1;
			var2 = gUserProfile.ProfileData.Inventory[i].Var2;
			var3 = gUserProfile.ProfileData.Inventory[i].Var3;

			gUserProfile.ProfileData.Inventory[i].quantity += quantityToAdd;
			totalQuantity = gUserProfile.ProfileData.Inventory[i].quantity;

			found = true;
			break; 
		}
	}

	if(!found)
	{
		wiInventoryItem* itm = gUserProfile.getFreeInventorySlot();
		if(!itm)
		{
			Scaleform::GFx::Value var[2];
			var[0].SetString(gLangMngr.getString("InGameUI_ErrorMsg_NoInventorySpace"));
			var[1].SetBoolean(true);
			gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
			return true;
		}

		itm->InventoryID = m_inventoryID;
		itm->itemID     = mStore_BuyItemID;
		itm->quantity   = quantityToAdd;
		itm->Var1   = var1;
		itm->Var2   = var2;
		itm->Var3   = var3;
		
		inventoryID = uint32_t(itm->InventoryID);

		totalQuantity = quantityToAdd;
	}

	Scaleform::GFx::Value var[7];
	var[0].SetUInt(inventoryID);
	var[1].SetUInt(mStore_BuyItemID);
	var[2].SetNumber(totalQuantity);
	var[3].SetNumber(var1);
	var[4].SetNumber(var2);
	var[5].SetBoolean(false);
	var[6].SetString(getAdditionalDescForItem(mStore_BuyItemID, var1, var2, var3));
	gfxMovie.Invoke("_root.api.addInventoryItem", var, 7);

	return found;
}

int FrontendWarZ::StoreDetectBuyIdx()
{
	return getStoreBuyIdxUI(mStore_BuyPriceGD, mStore_BuyPrice);
}

void FrontendWarZ::eventBackpackFromInventory(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	const wiCharDataFull& slot = gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID];
	if(!(slot.GameFlags & wiCharDataFull::GAMEFLAG_NearPostBox))
		return;

	m_inventoryID = args[0].GetUInt();
	m_gridTo = args[1].GetInt();
	m_Amount = args[2].GetInt();

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	uint32_t itemID = 0;
	for(uint32_t i=0; i<gUserProfile.ProfileData.NumItems; ++i)
	{
		if(gUserProfile.ProfileData.Inventory[i].InventoryID == m_inventoryID)
		{
			itemID = gUserProfile.ProfileData.Inventory[i].itemID;
			break;
		}
	}

	if (itemID == 103025) 
	{
		const wiCharDataFull& slot = gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID];
		for(int i=0; i<slot.BackpackSize; ++i)
		{
			if (slot.Items[i].itemID == itemID)
			{
				Scaleform::GFx::Value var[3];
				var[0].SetString(gLangMngr.getString("$NoMoreADroneOnBackpack"));
				var[1].SetBoolean(true);
				var[2].SetString("");
				gfxMovie.Invoke("_root.api.showInfoMsg", var, 3);
				return;
			}
		}
		m_Amount = 1;
	}

	if (itemID == 103018 && gUserProfile.ProfileData.PremiumAcc == 0)
	{
			Scaleform::GFx::Value var[3];
			var[0].SetString(gLangMngr.getString("$FR_PremiumAccountRequired"));
			var[1].SetBoolean(true);
			var[2].SetString("");
			gfxMovie.Invoke("_root.api.showInfoMsg", var, 3);
			return;
	}

	// check to see if there is anything in backpack, and if there is, then we need to firstly move that item to inventory
	if(m_gridTo != -1 && slot.Items[m_gridTo].itemID != 0 && slot.Items[m_gridTo].itemID!=itemID)
	{
		m_gridFrom = m_gridTo;
		m_Amount2 = slot.Items[m_gridTo].quantity;

		// check weight
		float totalWeight = slot.getTotalWeight();
		const BaseItemConfig* bic = g_pWeaponArmory->getConfig(slot.Items[m_gridTo].itemID);
		if(bic)
			totalWeight -= bic->m_Weight*slot.Items[m_gridTo].quantity;
	
		bic = g_pWeaponArmory->getConfig(itemID);
		if(bic)
			totalWeight += bic->m_Weight*m_Amount;

		if(slot.Skills[CUserSkills::SKILL_Physical3])
			totalWeight *= 0.95f;
		if(slot.Skills[CUserSkills::SKILL_Physical7])
			totalWeight *= 0.9f;

		const BackpackConfig* bc = g_pWeaponArmory->getBackpackConfig(slot.BackpackID);
		r3d_assert(bc);
		if(totalWeight > bc->m_maxWeight)
		{
			Scaleform::GFx::Value var[3];
			var[0].SetString(gLangMngr.getString("FR_PAUSE_TOO_MUCH_WEIGHT"));
			var[1].SetBoolean(true);
			var[2].SetString("");
			gfxMovie.Invoke("_root.api.showInfoMsg", var, 3);
			return;
		}

		StartAsyncOperation(&FrontendWarZ::as_BackpackFromInventorySwapThread, &FrontendWarZ::OnBackpackFromInventorySuccess);
	}
	else
	{
		// check weight
		float totalWeight = slot.getTotalWeight();

		const BaseItemConfig* bic = g_pWeaponArmory->getConfig(itemID);
		if(bic)
			totalWeight += bic->m_Weight*m_Amount;

		if(slot.Skills[CUserSkills::SKILL_Physical3])
			totalWeight *= 0.95f;
		if(slot.Skills[CUserSkills::SKILL_Physical7])
			totalWeight *= 0.9f;

		const BackpackConfig* bc = g_pWeaponArmory->getBackpackConfig(slot.BackpackID);
		r3d_assert(bc);
		if(totalWeight > bc->m_maxWeight)
		{
			Scaleform::GFx::Value var[2];
			var[0].SetString(gLangMngr.getString("FR_PAUSE_TOO_MUCH_WEIGHT"));
			var[1].SetBoolean(true);
			gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
			return;
		}

		StartAsyncOperation(&FrontendWarZ::as_BackpackFromInventoryThread, &FrontendWarZ::OnBackpackFromInventorySuccess);
	}
}

unsigned int WINAPI FrontendWarZ::as_BackpackFromInventorySwapThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;

	This->DelayServerRequest();

	// move item in backpack to inventory
	int apiCode = gUserProfile.ApiBackpackToInventory(This->m_gridFrom, This->m_Amount2);
	if(apiCode != 0)
	{
		if(apiCode == 7)
			This->SetAsyncError(0, gLangMngr.getString("GameSessionHasNotClosedYet"));
		else if(apiCode == 9)
			This->SetAsyncError(0, gLangMngr.getString("InGameUI_ErrorMsg_NoInventorySpace"));
		else
			This->SetAsyncError(apiCode, gLangMngr.getString("BackpackFromInventoryFail"));
		return 0;
	}

	apiCode = gUserProfile.ApiBackpackFromInventory(This->m_inventoryID, This->m_gridTo, This->m_Amount);
	if(apiCode != 0)
	{
		if(apiCode==7)
			This->SetAsyncError(0, gLangMngr.getString("GameSessionHasNotClosedYet"));
		else
			This->SetAsyncError(apiCode, gLangMngr.getString("BackpackFromInventoryFail"));
		return 0;
	}

	return 1;
}


unsigned int WINAPI FrontendWarZ::as_BackpackFromInventoryThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;
	
	This->DelayServerRequest();
	
	int apiCode = gUserProfile.ApiBackpackFromInventory(This->m_inventoryID, This->m_gridTo, This->m_Amount);
	if(apiCode != 0)
	{
		if(apiCode==7)
			This->SetAsyncError(0, gLangMngr.getString("GameSessionHasNotClosedYet"));
		else
			This->SetAsyncError(apiCode, gLangMngr.getString("BackpackFromInventoryFail"));
		return 0;
	}

	return 1;
}

void FrontendWarZ::OnBackpackFromInventorySuccess()
{
	Scaleform::GFx::Value var[8];
	gfxMovie.Invoke("_root.api.clearBackpack", "");
	int	slot = gUserProfile.SelectedCharID;

	addBackpackItems(gUserProfile.ProfileData.ArmorySlots[slot], slot);

	updateInventoryAndSkillItems ();

	updateSurvivorTotalWeight(slot);

	gfxMovie.Invoke("_root.api.hideInfoMsg", "");
	gfxMovie.Invoke("_root.api.backpackFromInventorySuccess", "");
	return;
}

void FrontendWarZ::updateSurvivorTotalWeight(int survivor)
{
	wiCharDataFull& slot = gUserProfile.ProfileData.ArmorySlots[survivor];
	float totalWeight = slot.getTotalWeight();

	if(slot.Skills[CUserSkills::SKILL_Physical3])
		totalWeight *= 0.95f;
	if(slot.Skills[CUserSkills::SKILL_Physical7])
		totalWeight *= 0.9f;

	Scaleform::GFx::Value var[2];
	char tmpGamertag[128];
	if(slot.ClanID != 0)
		sprintf(tmpGamertag, "[%s] %s", slot.ClanTag, slot.Gamertag);
	else
		r3dscpy(tmpGamertag, slot.Gamertag);

	var[0].SetString(tmpGamertag);
	var[1].SetNumber(totalWeight);
	gfxMovie.Invoke("_root.api.updateClientSurvivorWeight", var, 2);
}

void FrontendWarZ::eventBackpackToInventory(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	const wiCharDataFull& slot = gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID];
	if(!(slot.GameFlags & wiCharDataFull::GAMEFLAG_NearPostBox))
		return;

	m_gridFrom = args[0].GetInt();
	m_Amount = args[1].GetInt();

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	StartAsyncOperation(&FrontendWarZ::as_BackpackToInventoryThread, &FrontendWarZ::OnBackpackToInventorySuccess);
}

unsigned int WINAPI FrontendWarZ::as_BackpackToInventoryThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;
	
	This->DelayServerRequest();
	
	int apiCode = gUserProfile.ApiBackpackToInventory(This->m_gridFrom, This->m_Amount);
	if(apiCode != 0)
	{
		if(apiCode == 7)
			This->SetAsyncError(0, gLangMngr.getString("GameSessionHasNotClosedYet"));
		else if(apiCode == 9)
			This->SetAsyncError(0, gLangMngr.getString("InGameUI_ErrorMsg_NoInventorySpace"));
		else
			This->SetAsyncError(apiCode, gLangMngr.getString("BackpackToInventoryFail"));
		return 0;
	}

	return 1;
}

void FrontendWarZ::OnBackpackToInventorySuccess()
{
	Scaleform::GFx::Value var[8];
	gfxMovie.Invoke("_root.api.clearBackpack", "");
	int	slot = gUserProfile.SelectedCharID;

	addBackpackItems(gUserProfile.ProfileData.ArmorySlots[slot], slot);

	updateInventoryAndSkillItems ();

	updateSurvivorTotalWeight(slot);

	gfxMovie.Invoke("_root.api.hideInfoMsg", "");
	gfxMovie.Invoke("_root.api.backpackToInventorySuccess", "");


	return;
}

void FrontendWarZ::eventBackpackGridSwap(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	m_gridFrom = args[0].GetInt();
	m_gridTo = args[1].GetInt();

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	StartAsyncOperation(&FrontendWarZ::as_BackpackGridSwapThread, &FrontendWarZ::OnBackpackGridSwapSuccess);
}

void FrontendWarZ::eventSetSelectedChar(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	gUserProfile.SelectedCharID = args[0].GetInt();
	m_Player->uberAnim_->anim.StopAll();	// prevent animation blending on loadout switch
	m_Player->UpdateLoadoutSlot(gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID], -1);
}

void FrontendWarZ::eventOpenBackpackSelector(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 0);
	Scaleform::GFx::Value var[2];

	// clear
	gfxMovie.Invoke("_root.api.clearBackpacks", "");

	std::vector<uint32_t> uniqueBackpacks; // to filter identical backpack
	
	int backpackSlotIDInc = 0;
	// add backpack content info
	{
		wiCharDataFull& slot = gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID];

		for (int a = 0; a < slot.BackpackSize; a++)
		{
			if (slot.Items[a].itemID != 0)
			{
				if(std::find<std::vector<uint32_t>::iterator, uint32_t>(uniqueBackpacks.begin(), uniqueBackpacks.end(), slot.Items[a].itemID) != uniqueBackpacks.end())
					continue;
				
				const BackpackConfig* bpc = g_pWeaponArmory->getBackpackConfig(slot.Items[a].itemID);
				if(bpc)
				{
					// add backpack info
					var[0].SetInt(backpackSlotIDInc++);
					var[1].SetUInt(slot.Items[a].itemID);
					gfxMovie.Invoke("_root.api.addBackpack", var, 2);

					uniqueBackpacks.push_back(slot.Items[a].itemID);
				}
			}
		}
	}
	// add inventory info
	for(uint32_t i=0; i<gUserProfile.ProfileData.NumItems; ++i)
	{
		if(std::find<std::vector<uint32_t>::iterator, uint32_t>(uniqueBackpacks.begin(), uniqueBackpacks.end(), gUserProfile.ProfileData.Inventory[i].itemID) != uniqueBackpacks.end())
			continue;

		const BackpackConfig* bpc = g_pWeaponArmory->getBackpackConfig(gUserProfile.ProfileData.Inventory[i].itemID);
		if(bpc)
		{
			// add backpack info
			var[0].SetInt(backpackSlotIDInc++);
			var[1].SetUInt(gUserProfile.ProfileData.Inventory[i].itemID);
			gfxMovie.Invoke("_root.api.addBackpack", var, 2);

			uniqueBackpacks.push_back(gUserProfile.ProfileData.Inventory[i].itemID);
		}
	}

	gfxMovie.Invoke("_root.api.showChangeBackpack", "");
}

void FrontendWarZ::eventChangeBackpack(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 2);
	uint32_t itemID = args[1].GetUInt();

	__int64 inventoryID = 0;
	wiCharDataFull& slot = gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID];
	if(slot.BackpackID == itemID) // same backpack, skip
	{
		Scaleform::GFx::Value vars[2];
		vars[0].SetString(gLangMngr.getString("SwitchBackpackSameBackpacks"));
		vars[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", vars, 2);
		return;
	}

	// find inventory id with that itemID
	for (int a = 0; a < slot.BackpackSize; a++)
	{
		if (slot.Items[a].itemID == itemID)
		{
			inventoryID = slot.Items[a].InventoryID;
			break;
		}
	}
	if(inventoryID == 0)
	{
		for(uint32_t i=0; i<gUserProfile.ProfileData.NumItems; ++i)
		{
			if(gUserProfile.ProfileData.Inventory[i].itemID == itemID)
			{
				inventoryID = gUserProfile.ProfileData.Inventory[i].InventoryID;
				break;
			}
		}
	}

	if(inventoryID == 0)
	{
		Scaleform::GFx::Value vars[3];
		vars[0].SetString(gLangMngr.getString("FailedToFindBackpack"));
		vars[1].SetBoolean(true);
		vars[2].SetString("$ERROR");
		gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
		return;
	}

	// check if user has enough space in GI to put extra items if any
	{
		const BackpackConfig* bpc_current = g_pWeaponArmory->getBackpackConfig(slot.BackpackID);
		const BackpackConfig* bpc_new = g_pWeaponArmory->getBackpackConfig(itemID);
		r3d_assert(bpc_current);
		r3d_assert(bpc_new);
		if(bpc_new->m_maxSlots < bpc_current->m_maxSlots)
		{
			int extraItems = 0;
			for(int i=bpc_new->m_maxSlots; i<slot.BackpackSize; ++i)
			{
				if(slot.Items[i].itemID > 0)
					extraItems++;
			}

			if(extraItems)
			{
				if((gUserProfile.ProfileData.NumItems + extraItems) > wiUserProfile::INVENTORY_SIZE_LIMIT)
				{
					Scaleform::GFx::Value vars[3];
					vars[0].SetString(gLangMngr.getString("InGameUI_ErrorMsg_NoInventorySpace"));
					vars[1].SetBoolean(true);
					vars[2].SetString("$ERROR");
					gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
					return;
				}
			}
		}
	}

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);		

	mChangeBackpack_inventoryID = inventoryID;
	StartAsyncOperation(&FrontendWarZ::as_BackpackChangeThread, &FrontendWarZ::OnBackpackChangeSuccess);
}

unsigned int WINAPI FrontendWarZ::as_BackpackChangeThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;

	This->DelayServerRequest();

	int apiCode = gUserProfile.ApiChangeBackpack(This->mChangeBackpack_inventoryID);
	if(apiCode != 0)
	{
		This->SetAsyncError(apiCode, "Backpack change failed");
		return 0;
	}

	return 1;
}

void FrontendWarZ::updateClientSurvivor(const wiCharDataFull& slot)
{
	Scaleform::GFx::Value var[11];
	char tmpGamertag[128];
	if(slot.ClanID != 0)
		sprintf(tmpGamertag, "[%s] %s", slot.ClanTag, slot.Gamertag);
	else
		r3dscpy(tmpGamertag, slot.Gamertag);
	var[0].SetString(tmpGamertag);
	var[1].SetNumber(slot.Health);
	var[2].SetNumber(slot.Stats.XP);
	var[3].SetNumber(slot.Stats.TimePlayed);
	var[4].SetNumber(slot.Alive);
	var[5].SetNumber(slot.Hunger);
	var[6].SetNumber(slot.Thirst);
	var[7].SetNumber(slot.Toxic);
	var[8].SetNumber(slot.BackpackID);
	var[9].SetNumber(slot.BackpackSize);
	var[10].SetInt(slot.Stats.XP - slot.Stats.SpendXP);
	gfxMovie.Invoke("_root.api.updateClientSurvivor", var, 11);
}

void FrontendWarZ::OnBackpackChangeSuccess()
{
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");

	wiCharDataFull& slot = gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID];
	
	updateClientSurvivor(slot);

	addBackpackItems(slot, gUserProfile.SelectedCharID);
	updateInventoryAndSkillItems();

	updateSurvivorTotalWeight(gUserProfile.SelectedCharID);

	gfxMovie.Invoke("_root.api.changeBackpackSuccess", "");
}

void FrontendWarZ::eventCreateChangeCharacter(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	PlayerCreateLoadout.HeroItemID = args[0].GetInt();
	PlayerCreateLoadout.HeadIdx = args[1].GetInt();
	PlayerCreateLoadout.BodyIdx = args[2].GetInt();
	PlayerCreateLoadout.LegsIdx = args[3].GetInt();

	m_Player->uberAnim_->anim.StopAll();	// prevent animation blending on loadout switch
	m_Player->UpdateLoadoutSlot(PlayerCreateLoadout, -1);
}

void FrontendWarZ::eventCreateCancel(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	m_Player->uberAnim_->anim.StopAll();	// prevent animation blending on loadout switch
	m_Player->UpdateLoadoutSlot(gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID], -1);
}

void FrontendWarZ::eventRequestPlayerRender(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==1);
	m_needPlayerRenderingRequest = args[0].GetInt();
}

unsigned int WINAPI FrontendWarZ::as_BackpackGridSwapThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;
	
	This->DelayServerRequest();
	
	int apiCode = gUserProfile.ApiBackpackGridSwap(This->m_gridFrom, This->m_gridTo);
	if(apiCode != 0)
	{
		if(apiCode==7)
			This->SetAsyncError(0, gLangMngr.getString("GameSessionHasNotClosedYet"));
		else
			This->SetAsyncError(apiCode, gLangMngr.getString("BackpackToBackpackFail"));
		return 0;
	}

	return 1;
}

void FrontendWarZ::OnBackpackGridSwapSuccess()
{
	Scaleform::GFx::Value var[8];

	gfxMovie.Invoke("_root.api.clearBackpack", "");
	int	slot = gUserProfile.SelectedCharID;

	addBackpackItems(gUserProfile.ProfileData.ArmorySlots[slot], slot);

	updateSurvivorTotalWeight(slot);

	gfxMovie.Invoke("_root.api.hideInfoMsg", "");
	gfxMovie.Invoke("_root.api.backpackGridSwapSuccess", "");
	return;
}

void FrontendWarZ::eventOptionsLanguageSelection(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(args);
	r3d_assert(argCount == 1);

	const char* newLang = args[0].GetString();

	if(strcmp(newLang, g_user_language->GetString())==0)
		return; // same language

/*
#ifdef FINAL_BUILD
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(L"LOCALIZATIONS ARE COMING SOON");
		var[1].SetBoolean(true);
		pMovie->Invoke("_root.api.showInfoMsg", var, 2);		
		return;
	}
#endif
*/

	g_user_language->SetString(newLang);

	ReloadLocalization();

	gfxMovie.Invoke("_root.api.setLanguage", g_user_language->GetString());

	// write to ini file
	writeGameOptionsFile();
}

void FrontendWarZ::AddSettingsChangeFlag( DWORD flag )
{
	settingsChangeFlags_ |= flag;
}

static int compRes( const void* r1, const void* r2 )
{
	const r3dDisplayResolution* rr1 = (const r3dDisplayResolution*)r1 ;
	const r3dDisplayResolution* rr2 = (const r3dDisplayResolution*)r2 ;

	return rr1->Width - rr2->Width; // sort resolutions by width
}

void FrontendWarZ::SyncGraphicsUI()
{
	const DisplayResolutions& reses = r3dRenderer->GetDisplayResolutions();

	DisplayResolutions supportedReses ;

	for( uint32_t i = 0, e = reses.Count(); i < e; i ++ )
	{
		const r3dDisplayResolution& r = reses[ i ];
		float aspect = (float)r.Width / r.Height ;
		supportedReses.PushBack( r );
	}

	if(supportedReses.Count() == 0)
		r3dError("Couldn't find any supported video resolutions. Bad video driver?!\n");

	qsort( &supportedReses[ 0 ], supportedReses.Count(), sizeof supportedReses[ 0 ], compRes );

	gfxMovie.Invoke("_root.api.clearScreenResolutions", "");
	for( uint32_t i = 0, e = supportedReses.Count() ; i < e; i ++ )
	{
		char resString[ 128 ] = { 0 };
		const r3dDisplayResolution& r = supportedReses[ i ] ;
		_snprintf( resString, sizeof resString - 1, "%d x %d", r.Width, r.Height );
		gfxMovie.Invoke("_root.api.addScreenResolution", resString);
	}

	int width	= r_width->GetInt();
	int height	= r_height->GetInt();

	int desktopWidth, desktopHeight ;
	r3dGetDesktopDimmensions( &desktopWidth, &desktopHeight );

	if( !r_ini_read->GetBool() )
	{
		if( desktopWidth < width || desktopHeight < height )
		{
			width = desktopWidth ;
			height = desktopHeight ;
		}
	}

	bool finalResSet = false ;
	int selectedRes = 0;
	for( uint32_t i = 0, e = supportedReses.Count() ; i < e; i ++ )
	{
		const r3dDisplayResolution& r = supportedReses[ i ] ;
		if( width == r.Width && height == r.Height )
		{
			selectedRes = i;
			finalResSet = true;
			break;
		}
	}

	if( !finalResSet )
	{
		int bestSum = 0 ;

		for( uint32_t i = 0, e = supportedReses.Count() ; i < e; i ++ )
		{
			const r3dDisplayResolution& r = supportedReses[ i ] ;

			if( width >= r.Width && 
				height >= r.Height )
			{
				if( r.Width + r.Height > bestSum )
				{
					selectedRes = i;
					bestSum = r.Width + r.Height ;
					finalResSet = true ;
				}
			}
		}
	}

	if( !finalResSet )
	{
		int bestSum = 0x7fffffff ;

		// required mode too small, find smallest mode..
		for( uint32_t i = 0, e = supportedReses.Count() ; i < e; i ++ )
		{
			const r3dDisplayResolution& r = supportedReses[ i ] ;

			if( r.Width + r.Height < bestSum )
			{
				finalResSet = true ;

				selectedRes = i;
				bestSum = r.Width + r.Height ;
			}
		}
	}

	Scaleform::GFx::Value var[30];
	var[0].SetNumber(selectedRes);
	var[1].SetNumber( r_overall_quality->GetInt());
	var[2].SetNumber( ((r_gamma_pow->GetFloat()-2.2f)+1.0f)/2.0f);
	var[3].SetNumber( 0.0f );
	var[4].SetNumber( s_sound_volume->GetFloat());
	var[5].SetNumber( s_music_volume->GetFloat());
	var[6].SetNumber( 0.0f );
	var[7].SetNumber( g_tps_camera_mode->GetInt());
	var[8].SetNumber( g_enable_voice_commands->GetBool());
	var[9].SetNumber( r_antialiasing_quality->GetInt());
	var[10].SetNumber( r_ssao_quality->GetInt());
	var[11].SetNumber( r_terrain_quality->GetInt());
	var[12].SetNumber( r_environment_quality->GetInt() ); 
	var[13].SetNumber( 0 );
	var[14].SetNumber( r_shadows_quality->GetInt()+1);
	var[15].SetNumber( r_lighting_quality->GetInt());
	var[16].SetNumber( 0 );
	var[17].SetNumber( r_mesh_quality->GetInt());
	var[18].SetNumber( r_anisotropy_quality->GetInt());
	var[19].SetNumber( r_postprocess_quality->GetInt());
	var[20].SetNumber( r_texture_quality->GetInt());
	var[21].SetNumber( g_vertical_look->GetBool());
	var[22].SetNumber( 0 ); // not used
	var[23].SetNumber( g_mouse_wheel->GetBool());
	var[24].SetNumber( g_mouse_sensitivity->GetFloat());
	var[25].SetNumber( g_mouse_acceleration->GetBool());
	var[26].SetNumber( g_toggle_aim->GetBool());
	var[27].SetNumber( g_toggle_crouch->GetBool());
	var[28].SetNumber( r_fullscreen->GetInt());
	var[29].SetNumber( r_vsync_enabled->GetInt()+1);

	gfxMovie.Invoke("_root.api.setOptions", var, 30);

	{
		var[0].SetBoolean(g_voip_enable->GetBool());
		var[1].SetBoolean(g_voip_showChatBubble->GetBool());
		var[2].SetNumber(g_voip_InputDeviceInd->GetInt());
		var[3].SetNumber(g_voip_OutputDeviceInd->GetInt());
		var[4].SetNumber(g_voip_volume->GetFloat());
		gfxMovie.Invoke("_root.api.setVoipOptions", var, 5);
	}
 
	gfxMovie.Invoke("_root.api.reloadOptions", "");
}

void FrontendWarZ::eventOptionsReset(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(args);
	r3d_assert(argCount == 0);

	// get options
	g_tps_camera_mode->SetInt(0);
	g_enable_voice_commands			->SetBool(true);

	int old_fullscreen = r_fullscreen->GetInt();
	r_fullscreen->SetBool(true);

	int old_vsync = r_vsync_enabled->GetInt();
	r_vsync_enabled			->SetInt(0);

	if(old_fullscreen!=r_fullscreen->GetInt() || old_vsync!=r_vsync_enabled->GetInt())
	{
		// show message telling player that to change windows\fullscreen he have to restart game
		// todo: make fullscreen/window mode switch on the fly?
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("RestartGameForChangesToTakeEffect"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);		
	}

	switch( r3dGetDeviceStrength() )
	{
		case S_WEAK:
			r_overall_quality->SetInt(1);
			break;
		case S_MEDIUM:
			r_overall_quality->SetInt(2);
			break;
		case S_STRONG:
			r_overall_quality->SetInt(3);
			break;
		case S_ULTRA:
			r_overall_quality->SetInt(4);
			break;
		default:
			r_overall_quality->SetInt(1);
			break;
	}

	DWORD settingsChangedFlags = 0;
	GraphicSettings settings;

	switch( r_overall_quality->GetInt() )
	{
		case 1:
			FillDefaultSettings( settings, S_WEAK );
			settingsChangedFlags = SetDefaultSettings( S_WEAK );
			break;
		case 2:
			FillDefaultSettings( settings, S_MEDIUM );
			settingsChangedFlags = SetDefaultSettings( S_MEDIUM );
			break;
		case 3:
			FillDefaultSettings( settings, S_STRONG );
			settingsChangedFlags = SetDefaultSettings( S_STRONG );
			break;
		case 4:
			FillDefaultSettings( settings, S_ULTRA );
			settingsChangedFlags = SetDefaultSettings( S_ULTRA );
			break;
		case 5:
			{
				settings.mesh_quality			= (int)args[17].GetNumber();
				settings.texture_quality		= (int)args[20].GetNumber();
				settings.terrain_quality		= (int)args[11].GetNumber();
				settings.shadows_quality		= (int)args[14].GetNumber()-1;
				settings.lighting_quality		= (int)args[15].GetNumber();
				settings.environment_quality	= (int)args[12].GetNumber();
				settings.anisotropy_quality		= (int)args[18].GetNumber();
				settings.postprocess_quality	= (int)args[19].GetNumber();
				SaveCustomSettings( settings );
			}
			break;
		default:
			r3d_assert( false );
	}

	// AA\SSAO is separate and can be changed at any overall quality level
	settings.antialiasing_quality	= 0;
	settings.ssao_quality = 0;
	settingsChangedFlags |= GraphSettingsToVars( settings );
	AddSettingsChangeFlag( settingsChangedFlags );

	// clamp brightness and contrast, otherwise if user set it to 0 the screen will be white
	//r_brightness			->SetFloat(0.5f);
	//r_contrast				->SetFloat(0.5f);
	r_gamma_pow->SetFloat(2.2f);

	s_sound_volume			->SetFloat(1.0f);
	s_music_volume			->SetFloat(1.0f);
	//s_comm_volume			->SetFloat(1.0f);

	SetNeedUpdateSettings();

	// write to ini file
	writeGameOptionsFile();
	SyncGraphicsUI();
}

void FrontendWarZ::eventOptionsApply(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(args);
	r3d_assert(argCount == 23);

	// get options
	g_tps_camera_mode->SetInt((int)args[7].GetNumber());
	g_enable_voice_commands			->SetBool( !!(int)args[8].GetNumber() );

	const char* res = args[0].GetString();
	int width = 1280, height = 720;

	for( ; *res < '0' || * res > '9' ; ) res ++;
	int n = sscanf(res, "%d", &width );
	r3d_assert( n == 1 );
	for( ; *res >= '0' && * res <= '9' ; ) res ++;
	for( ; *res < '0' || * res > '9' ; ) res ++;
	n = sscanf(res, "%d", &height );
	r3d_assert( n == 1 );

	r_width->SetInt( width );
	r_height->SetInt( height );

	int old_fullscreen = r_fullscreen->GetInt();
	r_fullscreen->SetInt( (int)args[21].GetNumber() );

	int old_vsync = r_vsync_enabled->GetInt();
	r_vsync_enabled			->SetInt((int)args[22].GetNumber()-1);

	if(old_fullscreen!=r_fullscreen->GetInt() || old_vsync!=r_vsync_enabled->GetInt())
	{
		// show message telling player that to change windows\fullscreen he have to restart game
		// todo: make fullscreen/window mode switch on the fly?
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("RestartGameForChangesToTakeEffect"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);		
	}

	r_overall_quality		->SetInt( (int)args[1].GetNumber());

	DWORD settingsChangedFlags = 0;
	GraphicSettings settings;

	switch( r_overall_quality->GetInt() )
	{
		case 1:
			FillDefaultSettings( settings, S_WEAK );
			settingsChangedFlags = SetDefaultSettings( S_WEAK );
			break;
		case 2:
			FillDefaultSettings( settings, S_MEDIUM );
			settingsChangedFlags = SetDefaultSettings( S_MEDIUM );
			break;
		case 3:
			FillDefaultSettings( settings, S_STRONG );
			settingsChangedFlags = SetDefaultSettings( S_STRONG );
			break;
		case 4:
			FillDefaultSettings( settings, S_ULTRA );
			settingsChangedFlags = SetDefaultSettings( S_ULTRA );
			break;
		case 5:
			{
				settings.mesh_quality			= (int)args[17].GetNumber();
				settings.texture_quality		= (int)args[20].GetNumber();
				settings.terrain_quality		= (int)args[11].GetNumber();
				settings.shadows_quality		= (int)args[14].GetNumber()-1;
				settings.lighting_quality		= (int)args[15].GetNumber();
				settings.environment_quality	= (int)args[12].GetNumber();
				settings.anisotropy_quality		= (int)args[18].GetNumber();
				settings.postprocess_quality	= (int)args[19].GetNumber();
				SaveCustomSettings( settings );
			}
			break;
		default:
			r3d_assert( false );
	}

	// AA\SSAO is separate and can be changed at any overall quality level
	settings.antialiasing_quality	= (int)args[9].GetNumber();
	settings.ssao_quality			= (int)args[10].GetNumber();

	settingsChangedFlags |= GraphSettingsToVars( settings );

	AddSettingsChangeFlag( settingsChangedFlags );

	// clamp brightness and contrast, otherwise if user set it to 0 the screen will be white
	//r_brightness			->SetFloat( R3D_CLAMP((float)args[2].GetNumber(), 0.25f, 0.75f) );
	//r_contrast				->SetFloat( R3D_CLAMP((float)args[3].GetNumber(), 0.25f, 0.75f) );
	r_gamma_pow->SetFloat(2.2f + (float(args[2].GetNumber())*2.0f-1.0f));

	s_sound_volume			->SetFloat( R3D_CLAMP((float)args[4].GetNumber(), 0.0f, 1.0f) );
	s_music_volume			->SetFloat( R3D_CLAMP((float)args[5].GetNumber(), 0.0f, 1.0f) );
	//s_comm_volume			->SetFloat( R3D_CLAMP((float)args[6].GetNumber(), 0.0f, 1.0f) );


	SetNeedUpdateSettings();
	SyncGraphicsUI();

	// write to ini file
	writeGameOptionsFile();

	// if we changed resolution, we need to reset scaleform, otherwise visual artifacts will show up
//	needScaleformReset = true;
}

void FrontendWarZ::SetNeedUpdateSettings()
{
	needUpdateSettings_ = true;
}

void FrontendWarZ::UpdateSettings()
{
	r3dRenderer->UpdateSettings( ) ;

	gfxMovie.SetCurentRTViewport( Scaleform::GFx::Movie::SM_ExactFit );

	Mouse->SetRange( r3dRenderer->HLibWin );

	void applyGraphicsOptions( uint32_t settingsFlags );

	applyGraphicsOptions( settingsChangeFlags_ );

	gfxMovie.UpdateTextureMatrices("merc_rendertarget", (int)Scaleform_RenderToTextureRT->Width, (int)Scaleform_RenderToTextureRT->Height);
}

void FrontendWarZ::eventOptionsControlsRequestKeyRemap(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(args);
	r3d_assert(argCount == 1);

	int remapIndex = (int)args[0].GetNumber();
	r3d_assert(m_waitingForKeyRemap == -1);
	
	r3d_assert(remapIndex>=0 && remapIndex<r3dInputMappingMngr::KS_NUM);
	m_waitingForKeyRemap = remapIndex;
}

void FrontendWarZ::eventOptionsControlsReset(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(args);
	r3d_assert(argCount == 0);

//	InputMappingMngr->resetKeyMappingsToDefault();
	for(int i=0; i<r3dInputMappingMngr::KS_NUM; ++i)
	{
		Scaleform::GFx::Value args[2];
		args[0].SetString(gLangMngr.getString(InputMappingMngr->getMapName((r3dInputMappingMngr::KeybordShortcuts)i)));
		args[1].SetString(InputMappingMngr->getKeyName((r3dInputMappingMngr::KeybordShortcuts)i));
		gfxMovie.Invoke("_root.api.setKeyboardMapping", args, 2);
	}
	void writeInputMap();
	writeInputMap();

	// update those to match defaults in Vars.h
	g_vertical_look			->SetBool(false);
	g_mouse_wheel			->SetBool(true);
	g_mouse_sensitivity		->SetFloat(1.0f);
	g_mouse_acceleration	->SetBool(false);
	g_toggle_aim			->SetBool(false);
	g_toggle_crouch			->SetBool(false);

	// write to ini file
	writeGameOptionsFile();
	SyncGraphicsUI();
}

void FrontendWarZ::eventOptionsControlsApply(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(args);
	r3d_assert(argCount == 7);

	g_vertical_look			->SetBool( !!(int)args[0].GetNumber() );
	g_mouse_wheel			->SetBool( !!(int)args[2].GetNumber() );
	g_mouse_sensitivity		->SetFloat( (float)args[3].GetNumber() );
	g_mouse_acceleration	->SetBool( !!(int)args[4].GetNumber() );
	g_toggle_aim			->SetBool( !!(int)args[5].GetNumber() );
	g_toggle_crouch			->SetBool( !!(int)args[6].GetNumber() );

	// write to ini file
	writeGameOptionsFile();

	SyncGraphicsUI();
}

void FrontendWarZ::eventOptionsVoipApply(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(args);
	r3d_assert(argCount == 5);

	g_voip_enable			->SetBool( !!(int)args[0].GetNumber() );
	g_voip_showChatBubble	->SetBool( !!(int)args[1].GetNumber() );
	g_voip_InputDeviceInd	->SetInt( (int)args[2].GetNumber() );
	g_voip_OutputDeviceInd	->SetInt( (int)args[3].GetNumber() );
	g_voip_volume			->SetFloat( (float)args[4].GetNumber() );

	// write to ini file
	writeGameOptionsFile();

	SyncGraphicsUI();
}

void FrontendWarZ::eventOptionsVoipReset(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(args);
	r3d_assert(argCount == 0);

	// update those to match defaults in Vars.h
	g_voip_enable			->SetBool(true);
	g_voip_showChatBubble	->SetBool(true);
	g_voip_InputDeviceInd	->SetInt(-1);
	g_voip_OutputDeviceInd	->SetInt(-1);
	g_voip_volume			->SetFloat(1.0f);
	
	initVoipOptions();

	// write to ini file
	writeGameOptionsFile();
	SyncGraphicsUI();
}

void FrontendWarZ::eventMsgBoxCallback(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	if(loginMsgBoxOK_Exit)
	{
		loginMsgBoxOK_Exit = false;
		LoginMenuExitFlag = -1;
	}
	if(needReturnFromQuickJoin)
	{
		gfxMovie.Invoke("_root.api.Main.showScreen", "PlayGame");
		needReturnFromQuickJoin = false;
	}
}

void FrontendWarZ::eventBrowseGamesRequestFilterStatus(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(args);
	r3d_assert(argCount == 0);

	// setBrowseGamesOptions(regus:Boolean, regeu:Boolean, regru:Boolean, regsa:Boolean, filt_gw:Boolean, filt_sh:Boolean, filt_empt:Boolean, filt_full:Boolean, opt_trac:Boolean, opt_nm:Boolean, opt_ch:Boolean, opt_enable:Boolean)
	Scaleform::GFx::Value var[14];
	var[0].SetBoolean(gUserSettings.BrowseGames_Filter.region_us);
	var[1].SetBoolean(gUserSettings.BrowseGames_Filter.region_eu);
	var[2].SetBoolean(gUserSettings.BrowseGames_Filter.region_ru);
	var[3].SetBoolean(gUserSettings.BrowseGames_Filter.region_sa);
	var[4].SetBoolean(false);
	var[5].SetBoolean(false);
	var[6].SetBoolean(gUserSettings.BrowseGames_Filter.hideempty);
	var[7].SetBoolean(gUserSettings.BrowseGames_Filter.hidefull);
	var[8].SetBoolean(false);
	var[9].SetBoolean(gUserSettings.BrowseGames_Filter.tracers2);
	var[10].SetBoolean(gUserSettings.BrowseGames_Filter.nameplates2);
	var[11].SetBoolean(gUserSettings.BrowseGames_Filter.crosshair2);
	var[12].SetBoolean(gUserSettings.BrowseGames_Filter.enable_options);
	var[13].SetBoolean(gUserSettings.BrowseGames_Filter.password);
	gfxMovie.Invoke("_root.api.setBrowseGamesOptions", var, 14);
}

void FrontendWarZ::eventBrowseGamesSetFilter(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	//regus:Boolean, regeu:Boolean, regru:Boolean, regsa:Boolean, filt_gw:Boolean, filt_sh:Boolean, filt_empt:Boolean, filt_full:Boolean, opt_trac:Boolean, opt_nm:Boolean, opt_ch:Boolean, timeLimit:uint
	r3d_assert(args);
	r3d_assert(argCount == 16);
	gUserSettings.BrowseGames_Filter.region_us = args[0].GetBool();
	gUserSettings.BrowseGames_Filter.region_eu = args[1].GetBool();
	gUserSettings.BrowseGames_Filter.region_ru = args[2].GetBool();
	gUserSettings.BrowseGames_Filter.region_sa = args[3].GetBool();
	//gUserSettings.BrowseGames_Filter.gameworld = args[4].GetBool();
	//gUserSettings.BrowseGames_Filter.stronghold = args[5].GetBool();
	gUserSettings.BrowseGames_Filter.hideempty = args[6].GetBool();
	gUserSettings.BrowseGames_Filter.hidefull = args[7].GetBool();
	//gUserSettings.BrowseGames_Filter.showPrivateServers = args[8].GetBool();
	gUserSettings.BrowseGames_Filter.tracers2 = args[9].GetBool();
	gUserSettings.BrowseGames_Filter.nameplates2 = args[10].GetBool();
	gUserSettings.BrowseGames_Filter.crosshair2 = args[11].GetBool();
	r3dscpy(m_browseGamesNameFilter, args[12].GetString());
	gUserSettings.BrowseGames_Filter.enable_options= args[13].GetBool();
	gUserSettings.BrowseGames_Filter.password = args[14].GetBool();
	gUserSettings.BrowseGames_Filter.timeLimit = args[15].GetUInt();

	gUserSettings.saveSettings();
}

class callbackEnterPassword : public Scaleform::GFx::FunctionHandler
{
  public:
	virtual void Call(const Params& params)
	{
		FrontendWarZ* This = (FrontendWarZ*)params.pUserData;
		bool res = params.pArgs[0].GetBool();
		if(res) // user pressed OK
		{
			const char* inputText = params.pArgs[1].GetString();
			r3dscpy(This->m_joinGamePwd, inputText);
		
			Scaleform::GFx::Value var[3];
			var[0].SetString(gLangMngr.getString("WaitConnectingToServer"));
			var[1].SetBoolean(false);
			var[2].SetString("");
			This->gfxMovie.Invoke("_root.api.showInfoMsg", var, 3);

			This->StartAsyncOperation(&FrontendWarZ::as_JoinGameThread);
		}
	}
};
static callbackEnterPassword g_callbackEnterPassword;

void FrontendWarZ::eventBrowseGamesJoin(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	// gameID:int
	r3d_assert(args);
	r3d_assert(argCount == 1);

	if(gUserProfile.ProfileData.NumSlots == 0)
		return;
		
	m_joinGameServerId = args[0].GetInt();
	m_joinGamePwd[0]   = 0;
	r3d_assert(m_joinGameServerId > 0);

	// check if we need to input password
	bool needPassword = false;
	bool foundGameInMaster = false;

	// search in active games
	const GBPKT_M2C_GameData_s* gdata = gMasterServerLogic.FindGameById(m_joinGameServerId);
	if(gdata)
	{
		foundGameInMaster = true;
		if(gdata->info.flags & GBGameInfo::SFLAGS_Passworded)
			needPassword = true;

		if (gClientLogic().isGameStarted == 1)//gamehardcore
		{
				Scaleform::GFx::Value var[3];
				var[0].SetString(gLangMngr.getString("GameIsStarted"));
				var[1].SetBoolean(true);
				var[2].SetString("");
				gfxMovie.Invoke("_root.api.showInfoMsg", var, 3);
				gClientLogic().isGameStarted=0;
			return;
		}

		if(gUserProfile.ProfileData.isDevAccount==0) // only check non dev accounts
		{
			if(gdata->info.gameTimeLimit > int(gUserProfile.ProfileData.TimePlayed/60/60)) // check if we can join this game (gametime limit)
			{
				Scaleform::GFx::Value var[3];
				char tmpStr[512];
				sprintf(tmpStr, gLangMngr.getString("CannotJoinGameTimeLimit"), gdata->info.gameTimeLimit);
				var[0].SetString(tmpStr);
				var[1].SetBoolean(true);
				var[2].SetString("");
				gfxMovie.Invoke("_root.api.showInfoMsg", var, 3);
				return;
			}
		}
		// check if server doesn't allow trial accounts (exclude premium servers as trials can connect to it as well if they have premium account)
		if(gdata->info.channel != 4 && !(gdata->info.flags & GBGameInfo::SFLAGS_TrialsAllowed) && gUserProfile.IsTrialAccount())
		{
			Scaleform::GFx::Value var[3];
			var[0].SetString(gLangMngr.getString("CannotJoinGameTrialLimit"));
			var[1].SetBoolean(true);
			var[2].SetString("");
			gfxMovie.Invoke("_root.api.showInfoMsg", var, 3);
			return;
		}
		// Disable o enable Grass on server
		r3dGameLevel::SetGassEnable(getLevelDirBasedOnLevelID(gdata->info.mapId),gdata->info.EnableGass==1?true:false);
	}

	// check if we joining to our own server (joining from my server menu via eventMyServerJoinServer callback)
	const CUserServerRental::myServer_s* srv = gUserProfile.serverRent->GetServerById(m_joinGameServerId);
	if(srv)
	{
		needPassword = srv->pwd[0] ? true : false;
	}
	
	if(!foundGameInMaster)
	{
		// check if we joining recent or favorites
		for(UserSettings::TGameList::iterator it = gUserSettings.FavoriteGames.begin(); it != gUserSettings.FavoriteGames.end(); ++it)
		{
			if(it->gameServerId == m_joinGameServerId && it->flags & GBGameInfo::SFLAGS_Passworded)
			{
				needPassword = true;
				break;
			}
		}
		for(UserSettings::TGameList::iterator it = gUserSettings.RecentGames.begin(); it != gUserSettings.RecentGames.end(); ++it)
		{
			if(it->gameServerId == m_joinGameServerId && it->flags & GBGameInfo::SFLAGS_Passworded)
			{
				needPassword = true;
				break;
			}
		}
	}
	
	if(needPassword)
	{
		Scaleform::GFx::Value pfunc;
		gfxMovie.GetMovie()->CreateFunction(&pfunc, &g_callbackEnterPassword, this);
		Scaleform::GFx::Value var[3];
		var[0].SetString(gLangMngr.getString("$FR_EnterPasswordToJoinGame"));
		var[1].SetString(gLangMngr.getString("$FR_PASSWORD_PROTECTED"));
		var[2] = pfunc;
		gfxMovie.Invoke("_root.api.Main.MsgBox.showPasswordInputMsg", var, 3);
		return;
	}
	
	Scaleform::GFx::Value var[3];
	var[0].SetString(gLangMngr.getString("WaitConnectingToServer"));
	var[1].SetBoolean(false);
	var[2].SetString("");
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 3);

	StartAsyncOperation(&FrontendWarZ::as_JoinGameThread);
}
void FrontendWarZ::eventBrowseGamesOnAddToFavorites(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	// gameID:int
	r3d_assert(args);
	r3d_assert(argCount == 1);

	uint32_t gameID = (uint32_t)args[0].GetInt();
	const GBPKT_M2C_GameData_s* gdata = gMasterServerLogic.FindGameById(gameID);
	if(gdata)
	{
		gUserSettings.addGameToFavorite(gdata->info);
		gUserSettings.saveSettings();
	}
}

void FrontendWarZ::eventBrowseGamesRequestList(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	// type:String, sort:String, order:int, oper:int
	// type (browse, recent, favorites)
	// sort (name, mode, map, ping)
	// order (1, -1)
	// oper (0 - top, -1 - left 100 , +1 - right 100)
	r3d_assert(args);
	r3d_assert(argCount == 4);

	Scaleform::GFx::Value var[3];
	var[0].SetString(gLangMngr.getString("FetchingGamesListFromServer"));
	var[1].SetBoolean(false);
	var[2].SetString("");
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 3);

	const char* btype = args[0].GetString();
	const char* sort = args[1].GetString();
	m_browseGamesSortOrder = args[2].GetInt();
	m_browseGamesRequestedOper = args[3].GetInt();

	if(strcmp(sort, "name")==0)
		m_browseGamesSortMode = 0;
	else if(strcmp(sort, "mode")==0)
		m_browseGamesSortMode = 1;
	else if(strcmp(sort, "map")==0)
		m_browseGamesSortMode = 2;
	else if(strcmp(sort, "ping")==0)
		m_browseGamesSortMode = 3;

	if(strcmp(btype, "browse")==0)
		m_browseGamesMode = 0;
	else if(strcmp(btype, "recent")==0)
		m_browseGamesMode = 1;
	else
		m_browseGamesMode = 2;

	if(m_browseGamesMode==0 && m_browseGamesRequestedOper==0) // fetch new list from server only when requesting TOP list (activate and each refresh list click)
	{
		StartAsyncOperation(&FrontendWarZ::as_BrowseGamesThread, &FrontendWarZ::OnGameListReceived);
	}
	else
	{
		// this works only if we already have a list of games from server. but, browse games shows by default in mode 0, so we should always have a list
		gfxMovie.Invoke("_root.api.hideInfoMsg", "");
		processNewGameList();	
		gfxMovie.Invoke("_root.api.Main.BrowseGamesAnim.showGameList", "");
	}
}

unsigned int WINAPI FrontendWarZ::as_BrowseGamesThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;
	
	This->DelayServerRequest();
	if(!This->ConnectToMasterServer())
		return 0;
		
	GBPKT_C2M_RefreshList_s n;

	// copy filters
	n.region         = GBNET_REGION_US_West;
	n.browseChannel  = (BYTE)This->CurrentBrowseChannel;
	n.hideempty      = gUserSettings.BrowseGames_Filter.hideempty;
	n.hidefull       = gUserSettings.BrowseGames_Filter.hidefull;
	n.enable_options = gUserSettings.BrowseGames_Filter.enable_options;
	n.tracers2       = gUserSettings.BrowseGames_Filter.tracers2;
	n.nameplates2    = gUserSettings.BrowseGames_Filter.nameplates2;
	n.crosshair2     = gUserSettings.BrowseGames_Filter.crosshair2;
	n.password       = gUserSettings.BrowseGames_Filter.password;
	n.timeLimit      = gUserSettings.BrowseGames_Filter.timeLimit;

	gMasterServerLogic.RequestGameList(n);

	const float endTime = r3dGetTime() + 120.0f;
	while(r3dGetTime() < endTime)
	{
		::Sleep(10);
		if(gMasterServerLogic.gameListReceived_)
		{
			This->ProcessSupervisorPings();
			return 1;
		}

		if(!gMasterServerLogic.IsConnected())
			break;
	}

	This->SetAsyncError(0, gLangMngr.getString("FailedReceiveGameList"));
	return 0;
}

void FrontendWarZ::OnGameListReceived()
{
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");
	processNewGameList();	
	gfxMovie.Invoke("_root.api.Main.BrowseGamesAnim.showGameList", "");
}

static bool SortGamesByNameAsc(const GBPKT_M2C_GameData_s& g1, const GBPKT_M2C_GameData_s& g2) {
	return stricmp(g1.info.name, g2.info.name)<0; }
static bool SortGamesByNameDec(const GBPKT_M2C_GameData_s& g1, const GBPKT_M2C_GameData_s& g2) {
	return stricmp(g2.info.name, g1.info.name)<0; }

static bool SortGamesByMapAsc(const GBPKT_M2C_GameData_s& g1, const GBPKT_M2C_GameData_s& g2) {
	return g1.info.mapId<g2.info.mapId; }
static bool SortGamesByMapDec(const GBPKT_M2C_GameData_s& g1, const GBPKT_M2C_GameData_s& g2) {
	return g2.info.mapId<g1.info.mapId; }

static bool SortGamesByModeAsc(const GBPKT_M2C_GameData_s& g1, const GBPKT_M2C_GameData_s& g2) {
	return g1.info.IsGameworld()==g2.info.IsGameworld(); }
static bool SortGamesByModeDec(const GBPKT_M2C_GameData_s& g1, const GBPKT_M2C_GameData_s& g2) {
	return g2.info.IsGameworld()==g1.info.IsGameworld(); }

static bool SortGamesByPingAsc(const GBPKT_M2C_GameData_s& g1, const GBPKT_M2C_GameData_s& g2) {
	int ping1 = GetGamePing(g1.superId);
	int ping2 = GetGamePing(g2.superId);
	return ping1<ping2; }
static bool SortGamesByPingDec(const GBPKT_M2C_GameData_s& g1, const GBPKT_M2C_GameData_s& g2) {
	int ping1 = GetGamePing(g1.superId);
	int ping2 = GetGamePing(g2.superId);
	return ping2<ping1; }

void FrontendWarZ::processNewGameList()
{
	r3dgameVector(GBPKT_M2C_GameData_s) filteredGames;

	if(m_browseGamesMode == 0)
	{
		// note: all additional filters are server side now
		for(size_t i=0; i<gMasterServerLogic.games_.size(); i++) 
		{
			const GBPKT_M2C_GameData_s& gd = gMasterServerLogic.games_[i];
			const GBGameInfo& ginfo = gd.info;
			
			// simple name filter
			if(strlen(m_browseGamesNameFilter)>1)
			{
				if(!stristr(ginfo.name, m_browseGamesNameFilter))
					continue;
			}
			filteredGames.push_back(gd);
		}
	}
	else if(m_browseGamesMode == 1) // recent
	{
		GBPKT_M2C_GameData_s gd;
		gd.curPlayers = 0;
		gd.status     = 0;
		gd.superId    = 0;
		
		UserSettings::TGameList& list = gUserSettings.RecentGames;
		for(UserSettings::TGameList::iterator it = list.begin(); it != list.end(); ++it)
		{
			gd.info = *it;
			filteredGames.push_back(gd);
		}
	}
	else if(m_browseGamesMode == 2) // favorite
	{
		GBPKT_M2C_GameData_s gd;
		gd.curPlayers = 0;
		gd.status     = 0;
		gd.superId    = 0;
		
		UserSettings::TGameList& list = gUserSettings.FavoriteGames;
		for(UserSettings::TGameList::iterator it = list.begin(); it != list.end(); ++it)
		{
			gd.info = *it;
			filteredGames.push_back(gd);
		}
	}
	else
		r3d_assert(false); // shouldn't happen


	// sort
	if(m_browseGamesSortMode == 0 && m_browseGamesMode != 1) // do not sort recent games by name, kill whole idea
	{
		if(m_browseGamesSortOrder == 1)
			std::sort(filteredGames.begin(), filteredGames.end(), SortGamesByNameAsc);
		else
			std::sort(filteredGames.begin(), filteredGames.end(), SortGamesByNameDec);
	}
	if(m_browseGamesSortMode == 1)
	{
		if(m_browseGamesSortOrder == 1)
			std::sort(filteredGames.begin(), filteredGames.end(), SortGamesByMapAsc);
		else
			std::sort(filteredGames.begin(), filteredGames.end(), SortGamesByMapDec);
	}
	if(m_browseGamesSortMode == 2)
	{
		if(m_browseGamesSortOrder == 1)
			std::sort(filteredGames.begin(), filteredGames.end(), SortGamesByModeAsc);
		else
			std::sort(filteredGames.begin(), filteredGames.end(), SortGamesByModeDec);
	}
	if(m_browseGamesSortMode == 3)
	{
		if(m_browseGamesSortOrder == 1)
			std::sort(filteredGames.begin(), filteredGames.end(), SortGamesByPingAsc);
		else
			std::sort(filteredGames.begin(), filteredGames.end(), SortGamesByPingDec);
	}

	int numGames = (int)filteredGames.size();

	if(m_browseGamesRequestedOper == -1)
		m_browseGamesCurrentCur -= 100;
	else if(m_browseGamesRequestedOper == 1)
		m_browseGamesCurrentCur += 100;
	else if(m_browseGamesRequestedOper == 0)
		m_browseGamesCurrentCur = 0;

	m_browseGamesCurrentCur = R3D_CLAMP(m_browseGamesCurrentCur, 0, numGames-100);

	for(int i=0; i<numGames; i++) 
	{
		if(i < m_browseGamesCurrentCur || i>(m_browseGamesCurrentCur+100))
			continue;

		const GBPKT_M2C_GameData_s& gd = filteredGames[i];
		const GBGameInfo& ginfo = gd.info;

		int ping = GetGamePing(gd.superId);
		if(ping > 0)
			ping = R3D_CLAMP(ping + random(10)-5, 1, 1000);
		ping = R3D_CLAMP(ping/10, 1, 100); // UI accepts ping from 0 to 100 and shows bars instead of actual number

		const char* mapName = getMapName(ginfo.mapId);

		//addGameToList(id:Number, name:String, mode:String, map:String, tracers:Boolean, nametags:Boolean, crosshair:Boolean, players:String, ping:int, isPassword:Boolean)
		Scaleform::GFx::Value var[16];
		var[0].SetNumber(ginfo.gameServerId);
		var[1].SetString(ginfo.name);
		var[2].SetString(ginfo.IsGameworld() ? "GAMEWORLD" : "STRONGHOLD");
		var[3].SetString(mapName);
		var[4].SetBoolean((ginfo.flags & GBGameInfo::SFLAGS_Tracers) ? true : false);
		var[5].SetBoolean((ginfo.flags & GBGameInfo::SFLAGS_Nameplates) ? true : false);
		var[6].SetBoolean((ginfo.flags & GBGameInfo::SFLAGS_CrossHair) ? true : false);
		char players[16];
		sprintf(players, "%d/%d", R3D_MIN(gd.curPlayers, ginfo.maxPlayers), ginfo.maxPlayers); // hide if dev connected
		var[7].SetString(players);
		var[8].SetInt(ping);
		var[9].SetBoolean(gUserSettings.isInFavoriteGamesList(ginfo.gameServerId));
		var[10].SetBoolean((ginfo.flags & GBGameInfo::SFLAGS_Passworded) ? true : false);
		var[11].SetBoolean(ginfo.gameTimeLimit>0?true:false);
		var[12].SetBoolean((ginfo.flags & GBGameInfo::SFLAGS_TrialsAllowed) ? true : false);
		var[13].SetBoolean(ginfo.channel==3 && m_browseGamesMode == 0); // allow donating GC only to private servers
		var[14].SetBoolean((ginfo.flags & GBGameInfo::SFLAGS_DisableASR || ginfo.flags & GBGameInfo::SFLAGS_DisableSNP) ? true : false);
		var[15].SetBoolean(ginfo.flags & GBGameInfo::SFLAGS_DisableWPN ? true : false);
		if (ginfo.mapId != GBGameInfo::MAPID_WZ_Devmap)
		{
			gfxMovie.Invoke("_root.api.Main.BrowseGamesAnim.addGameToList", var, 16);
		}
		else {
			 if( gUserProfile.ProfileData.isDevAccount>0)
				 gfxMovie.Invoke("_root.api.Main.BrowseGamesAnim.addGameToList", var, 16);
		}
	}
}

int FrontendWarZ::GetSupervisorPing(DWORD ip)
{
	HANDLE hIcmpFile = IcmpCreateFile();
	if(hIcmpFile == INVALID_HANDLE_VALUE) {
		r3dOutToLog("IcmpCreatefile returned error: %d\n", GetLastError());
		return -1;
	}    

	char  sendData[32]= "Data Buffer";
	DWORD replySize   = sizeof(ICMP_ECHO_REPLY) + sizeof(sendData);
	void* replyBuf    = (void*)_alloca(replySize);
	
	// send single ping with 1000ms, without payload as it alert most firewalls
	DWORD sendResult = IcmpSendEcho(hIcmpFile, ip, sendData, 0, NULL, replyBuf, replySize, 1000);
#ifndef FINAL_BUILD	
	if(sendResult == 0) {
		DWORD lastE = GetLastError();
		char ips[128];
		r3dscpy(ips, inet_ntoa(*(in_addr*)&ip));
		r3dOutToLog("PING failed to %s : %d\n", ips, lastE);
	}
#endif

	IcmpCloseHandle(hIcmpFile);

	if(sendResult == 0) {
		//r3dOutToLog("IcmpSendEcho returned error: %d\n", GetLastError());
		return -2;
	}

	PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)replyBuf;
	if(pEchoReply->Status == IP_SUCCESS)
	{
		return pEchoReply->RoundTripTime;
	}

	//r3dOutToLog("IcmpSendEcho returned status %d\n", pEchoReply->Status);
	return -3;
}

void FrontendWarZ::ProcessSupervisorPings()
{
	for(size_t i = 0; i < gMasterServerLogic.supers_.size(); ++i)
	{
		const GBPKT_M2C_SupervisorData_s& super = gMasterServerLogic.supers_[i];
		if(super.ID >= R3D_ARRAYSIZE(superPings_))
		{
#ifndef FINAL_BUILD		
			r3dError("Too Many servers, please contact support@warz.kongsi.asia");
#endif
			continue;
		}

		// if we already have stored ping, don't query again
		if(superPings_[super.ID] > 1)
			continue;

		int ping = GetSupervisorPing(super.ip);
		superPings_[super.ID] = ping ? ping : 1;
		
		// fill all other supervisors in /24 IP mask with same value to prevent pinging machines in same DC
		for(size_t j = i + 1; j < gMasterServerLogic.supers_.size(); ++j)
		{
			const GBPKT_M2C_SupervisorData_s& s2 = gMasterServerLogic.supers_[j];
			if(s2.ID >= R3D_ARRAYSIZE(superPings_))
				continue;
				
			if((s2.ip & 0x00FFFFFF) == (super.ip & 0x00FFFFFF)) // network order is big engianess
				superPings_[s2.ID] = superPings_[super.ID];
		}
	}
}

void FrontendWarZ::eventRequestMyClanInfo(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==0);

	setClanInfo();
	gfxMovie.Invoke("_root.api.Main.Clans.showClanList", "");
}

void FrontendWarZ::setClanInfo()
{
	wiCharDataFull& slot = gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID];
	CUserClans* clans = gUserProfile.clans[gUserProfile.SelectedCharID];

	// fill clanCurData_.clanID and invites/application list.
	// TODO: implement timer to avoid API spamming? or check async every N seconds...
	clans->ApiGetClanStatus();
	
	if(clans->clanCurData_.ClanID != slot.ClanID)
	{
		slot.ClanID = clans->clanCurData_.ClanID;
		// we joined or left clan. do something
	}


	// if we don't have clan data yet - retrieve it. NOTE: need switch to async call
	if(slot.ClanID && clans->clanInfo_.ClanID == 0)
	{
		clans->ApiClanGetInfo(slot.ClanID, &clans->clanInfo_, &clans->clanMembers_);
	}
	
	{
		//		public function setClanInfo(clanID:uint, isAdmin:Boolean, name:String, availableSlots:uint, clanReserve:uint)
		Scaleform::GFx::Value var[6];
		var[0].SetUInt(slot.ClanID); // if ClanID is zero, then treated by UI as user is not in clan
		var[1].SetBoolean(slot.ClanRank<=1); // should be true only for admins of the clan (creator=0 and officers=1)
		var[2].SetString(clans->clanInfo_.ClanName);
		var[3].SetUInt(clans->clanInfo_.MaxClanMembers-clans->clanInfo_.NumClanMembers);
		var[4].SetUInt(clans->clanInfo_.ClanGP);
		var[5].SetUInt(clans->clanInfo_.ClanEmblemID);
		gfxMovie.Invoke("_root.api.setClanInfo", var, 6);
	}

	{
		Scaleform::GFx::Value var[10];
		for(CUserClans::TClanMemberList::iterator iter=clans->clanMembers_.begin(); iter!=clans->clanMembers_.end(); ++iter)
		{
			CUserClans::ClanMember_s& memberInfo = *iter;
			//public function addClanMemberInfo(customerID:uint, name:String, exp:uint, time:String, rep:String, kzombie:uint, ksurvivor:uint, kbandits:uint, donatedgc:uint)
			var[0].SetUInt(memberInfo.CharID);
			var[1].SetString(memberInfo.gamertag);
			var[2].SetUInt(memberInfo.stats.XP);
			var[3].SetString(getTimePlayedString(memberInfo.stats.TimePlayed));
			var[4].SetString(getReputationString(memberInfo.stats.Reputation));
			var[5].SetUInt(memberInfo.stats.KilledZombies);
			var[6].SetUInt(memberInfo.stats.KilledSurvivors);
			var[7].SetUInt(memberInfo.stats.KilledBandits);
			var[8].SetUInt(memberInfo.ContributedGP);
			var[9].SetUInt(memberInfo.ClanRank);
			gfxMovie.Invoke("_root.api.addClanMemberInfo", var, 10);
		}
	}
	checkForInviteFromClan();
}

void FrontendWarZ::eventRequestClanList(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 2);
	//sort:int, mode:int
	int sortID = args[0].GetInt();
	int mode = args[1].GetInt(); // 0-top, 1-left, 2-right

	clanListRequest_SortID = sortID;
	if(mode == 0)
		clanListRequest_StartPos = 0;
	else if(mode == 1)
		clanListRequest_StartPos = R3D_CLAMP(clanListRequest_StartPos-100, 0, clanListRequest_ClanListSize-100);
	else if(mode == 2)
		clanListRequest_StartPos = R3D_CLAMP(clanListRequest_StartPos+100, 0, clanListRequest_ClanListSize-100);
	else
		r3d_assert(false);

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	StartAsyncOperation(&FrontendWarZ::as_RequestClanListThread, &FrontendWarZ::OnRequestClanListSuccess);
}

unsigned int WINAPI FrontendWarZ::as_RequestClanListThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;

	CUserClans* clans = game_new CUserClans;
	int api = clans->ApiClanGetLeaderboard2(This->clanListRequest_SortID, This->clanListRequest_StartPos, &This->clanListRequest_ClanListSize);
	This->cachedClanList = clans->leaderboard_;
	delete clans;

	if(api!=0)
	{
		if(!This->processClanError(api))
		{
			This->SetAsyncError(api, gLangMngr.getString("FailedToGetClanLeaderboard"));
		}
		return 0;
	}
	return 1;
}

void FrontendWarZ::OnRequestClanListSuccess()
{
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");

	//r3d_assert(!cachedClanList.empty());
	{
		gfxMovie.Invoke("_root.api.Main.Clans.clearClanInfo", "");

		Scaleform::GFx::Value var[7];
		for(ClanList::iterator iter=cachedClanList.begin(); iter!=cachedClanList.end(); ++iter)
		{
			CUserClans::ClanInfo_s& clanInfo = *iter;
			//public function addClanInfo(clanID:uint, name:String, creator:String, xp:uint, numMembers:uint, description:String, icon:String)
			var[0].SetUInt(clanInfo.ClanID);
			var[1].SetString(clanInfo.ClanName);
			var[2].SetString(clanInfo.OwnerGamertag);
			var[3].SetUInt(clanInfo.ClanXP);
			var[4].SetUInt(clanInfo.NumClanMembers);
			var[5].SetString(clanInfo.ClanLore);
			var[6].SetUInt(clanInfo.ClanEmblemID);
			gfxMovie.Invoke("_root.api.Main.Clans.addClanInfo", var, 7);
		}
		gfxMovie.Invoke("_root.api.Main.Clans.populateClanList", "");
	}
}

void FrontendWarZ::eventCreateClan(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 6);

	if(gUserProfile.ProfileData.NumSlots == 0)
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("YouNeedCharBeforeCreatingClan"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	if (gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].Hardcore == 1)
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("NotAllowedHardcorePlayers"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	if(gUserProfile.IsTrialAccount())
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("TrialAccountLimit_NoClans"));
		var[1].SetString(gLangMngr.getString("$TrialAccountLimit"));
		gfxMovie.Invoke("_root.api.showTrialAccountLimit", var, 2);
		return;
	}

	//eventCreateClan(name:String, tag:String, desc:String, nameColor:int, tagColor:int, iconID:int)
	r3dscpy(clanCreateParams.ClanName, args[0].GetString());
	r3dscpy(clanCreateParams.ClanTag, args[1].GetString());
	clanCreateParam_Desc = args[2].GetString();
	clanCreateParams.ClanNameColor = args[3].GetInt();
	clanCreateParams.ClanTagColor = args[4].GetInt();
	clanCreateParams.ClanEmblemID = args[5].GetInt();

	if(strpbrk(clanCreateParams.ClanName, "!@#$%^&*()-=+_<>,./?'\":;|{}[]")!=NULL) // do not allow this symbols
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("ClanNameNoSpecSymbols"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}
	if(strpbrk(clanCreateParams.ClanTag, "!@#$%^&*()-=+_<>,./?'\":;|{}[]")!=NULL) // do not allow this symbols
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("ClanTagNoSpecSymbols"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	/*int pos = 0;
	while((pos= clanCreateParam_Desc.find('<'))!=-1)
		clanCreateParam_Desc.replace(pos, 1, "&lt;");
	while((pos = clanCreateParam_Desc.find('>'))!=-1)
		clanCreateParam_Desc.replace(pos, 1, "&gt;");*/

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	StartAsyncOperation(&FrontendWarZ::as_CreateClanThread, &FrontendWarZ::OnCreateClanSuccess);
}

bool FrontendWarZ::processClanError(int api)
{
	if(api >= 20 && api <= 29)
	{
		char tmpStr[64];
		sprintf(tmpStr, "ClanError_Code%d", api);
		SetAsyncError(0, gLangMngr.getString(tmpStr));
		return true;
	}
	return false;
}

unsigned int WINAPI FrontendWarZ::as_CreateClanThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;

	CUserClans* clans = gUserProfile.clans[gUserProfile.SelectedCharID];
	int api = clans->ApiClanCreate(This->clanCreateParams);
	if(api!=0)
	{
		if(!This->processClanError(api))
		{
			if(api==7)
				This->SetAsyncError(0, gLangMngr.getString("NotEnoughTimePlayedToCreateClan"));
			else
				This->SetAsyncError(api, gLangMngr.getString("FailedToCreateClan"));
		}
		return 0;
	}
	return 1;
}

void FrontendWarZ::OnCreateClanSuccess()
{
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");
	CUserClans* clans = gUserProfile.clans[gUserProfile.SelectedCharID];
	int api = clans->ApiClanSetLore(clanCreateParam_Desc.c_str());
	if(api!=0)
	{
		r3dOutToLog("failed to set clan desc, api=%d\n", api);
		r3d_assert(false);
	}

	cachedClanList.clear(); // to refresh and get newly created clan
	setClanInfo();
	gfxMovie.Invoke("_root.api.Main.showScreen", "MyClan");
}

void FrontendWarZ::eventClanAdminDonateGC(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 2);
	uint32_t charID = args[0].GetUInt();
	uint32_t numGC = args[1].GetUInt();
	CUserClans* clans = gUserProfile.clans[gUserProfile.SelectedCharID];

	if(clans->clanInfo_.ClanGP < int(numGC))
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("ClanReserveNotEnoughGC"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	int api = clans->ApiClanDonateGPToMember(charID, numGC);
	if(api != 0)
	{
		r3dOutToLog("Failed to donate to member, api=%d\n", api);

		Scaleform::GFx::Value var[2];
		if(api==9) // one week timeout
		{
			var[0].SetString(gLangMngr.getString("FailToDonateGCToClanMemberTime"));
			var[1].SetBoolean(true);
		}
		else
		{
			var[0].SetString(gLangMngr.getString("FailToDonateGCToClanMember"));
			var[1].SetBoolean(true);
		}
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	char tmpStr[32]; sprintf(tmpStr, "%d GC", clans->clanInfo_.ClanGP);
	gfxMovie.SetVariable("_root.api.Main.ClansMyClan.MyClan.OptionsBlock3.GC.text", tmpStr);
}

void FrontendWarZ::refreshClanUIMemberList()
{
	setClanInfo();
	gfxMovie.Invoke("_root.api.Main.ClansMyClan.showClanMembers", "");
}

void FrontendWarZ::eventClanAdminAction(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 2);
	uint32_t charID = args[0].GetUInt();
	const char* actionType = args[1].GetString(); // promote, demote, kick
	CUserClans* clans = gUserProfile.clans[gUserProfile.SelectedCharID];
	if(strcmp(actionType, "promote") == 0)
	{
		CUserClans::ClanMember_s* member = clans->GetMember(charID);
		r3d_assert(member);
		if(member->ClanRank>0)
		{
			int newRank = member->ClanRank;
			if(newRank > 2)
				newRank = 1;
			else
				newRank = newRank-1;
			int api = clans->ApiClanSetRank(charID, newRank);
			if(api != 0)
			{
				r3dOutToLog("Failed to promote rank, api=%d\n", api);

				Scaleform::GFx::Value var[2];
				var[0].SetString(gLangMngr.getString("FailToPromote"));
				var[1].SetBoolean(true);
				gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
			}
			else
			{
				if(newRank == 0) // promoted someone else to leader -> demote itself
				{
					wiCharDataFull& slot = gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID];
					slot.ClanRank = 1;
					CUserClans::ClanMember_s* m = clans->GetMember(slot.LoadoutID);
					if(m)
						m->ClanRank = 1;
				}
				refreshClanUIMemberList();
			}
		}
		else
		{
			Scaleform::GFx::Value var[2];
			var[0].SetString(gLangMngr.getString("MemberAlreadyHasHighestRank"));
			var[1].SetBoolean(true);
			gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		}
	}
	if(strcmp(actionType, "demote") == 0)
	{
		CUserClans::ClanMember_s* member = clans->GetMember(charID);
		r3d_assert(member);
		if(member->ClanRank<2)
		{
			int api = clans->ApiClanSetRank(charID, member->ClanRank+1);
			if(api != 0)
			{
				r3dOutToLog("Failed to demote rank, api=%d\n", api);

				Scaleform::GFx::Value var[2];
				var[0].SetString(gLangMngr.getString("FailToDemote"));
				var[1].SetBoolean(true);
				gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
			}
			else
				refreshClanUIMemberList();
		}
		else
		{
			Scaleform::GFx::Value var[2];
			var[0].SetString(gLangMngr.getString("MemberAlreadyHasLowestRank"));
			var[1].SetBoolean(true);
			gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		}
	}
	if(strcmp(actionType, "kick") == 0)
	{
		if(clans->GetMember(charID)== NULL)
		{
			Scaleform::GFx::Value var[2];
			var[0].SetString(gLangMngr.getString("NoSuchClanMember"));
			var[1].SetBoolean(true);
			gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
			return;
		}

		int api = clans->ApiClanKick(charID);
		if(api != 0)
		{
			if(api == 6)
			{
				Scaleform::GFx::Value var[2];
				var[0].SetString(gLangMngr.getString("YouCannotKickYourself"));
				var[1].SetBoolean(true);
				gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
			}
			else
			{
				r3dOutToLog("Failed to kick, api=%d\n", api);

				Scaleform::GFx::Value var[2];
				var[0].SetString(gLangMngr.getString("FailToKickMember"));
				var[1].SetBoolean(true);
				gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
			}
		}
		else
		{
			Scaleform::GFx::Value var[2];
			var[0].SetString(gLangMngr.getString("ClanMemberWasKickedFromClan"));
			var[1].SetBoolean(true);
			gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
			refreshClanUIMemberList();
		}
	}
}

void FrontendWarZ::eventClanLeaveClan(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 0);
	CUserClans* clans = gUserProfile.clans[gUserProfile.SelectedCharID];

	// do not allow leader to leave clan if clan has GC balance
	if(clans->clanInfo_.NumClanMembers==1 && clans->clanInfo_.ClanGP > 0)
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("CannotLeaveClanWithGCBalance"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	int api = clans->ApiClanLeave();
	if(api != 0)
	{
		r3dOutToLog("Failed to leave clan, api=%d\n", api);

		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("FailToLeaveClan"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
	}
	else
	{
		gfxMovie.Invoke("_root.api.Main.showScreen", "Clans");
	}
}

void FrontendWarZ::eventClanDonateGCToClan(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 1);
	uint32_t amount = args[0].GetUInt();

	if(amount == 0)
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("ClanError_AmountToDonateShouldBeMoreThanZero"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}
	if(amount > (uint32_t)gUserProfile.ProfileData.GamePoints)
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("ClanError_NotEnoughGPToDonate"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	CUserClans* clans = gUserProfile.clans[gUserProfile.SelectedCharID];
	int api = clans->ApiClanDonateGPToClan(amount);
	if(api != 0)
	{
		r3dOutToLog("Failed to donate to clan, api=%d\n", api);

		Scaleform::GFx::Value var[2];
		if(api==9) // one week timeout
		{
			var[0].SetString(gLangMngr.getString("FailToDonateGCToClanTime"));
			var[1].SetBoolean(true);
		}
		else
		{
			var[0].SetString(gLangMngr.getString("FailToDonateGCToClan"));
			var[1].SetBoolean(true);
		}
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}
	char tmpStr[32]; sprintf(tmpStr, "%d GC", clans->clanInfo_.ClanGP);
	gfxMovie.SetVariable("_root.api.Main.ClansMyClan.MyClan.OptionsBlock3.GC.text", tmpStr);
}

void FrontendWarZ::eventRequestClanApplications(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 0);

	CUserClans* clans = gUserProfile.clans[gUserProfile.SelectedCharID];

	clans->ApiGetClanStatus();

	Scaleform::GFx::Value var[9];
	for(CUserClans::ClanApplicationList::iterator it = clans->clanApplications_.begin(); it!=clans->clanApplications_.end(); ++it)
	{
		//public function addApplication(appID:uint, appText:String, name:String, exp:uint, stime:String, rep:String, kz:uint, ks:uint, kb:uint)
		CUserClans::ClanApplication_s& appl = *it;
		var[0].SetUInt(appl.ClanApplID);
		var[1].SetString(appl.Note.c_str());
		var[2].SetString(appl.Gamertag.c_str());
		var[3].SetUInt(appl.stats.XP);
		var[4].SetString(getTimePlayedString(appl.stats.TimePlayed));
		var[5].SetString(getReputationString(appl.stats.Reputation));
		var[6].SetUInt(appl.stats.KilledZombies);
		var[7].SetUInt(appl.stats.KilledSurvivors);
		var[8].SetUInt(appl.stats.KilledBandits);
		gfxMovie.Invoke("_root.api.Main.ClansMyClanApps.addApplication", var, 9);
	}

	gfxMovie.Invoke("_root.api.Main.ClansMyClanApps.showApplications", "");
}

void FrontendWarZ::eventClanApplicationAction(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 2);
	uint32_t applicationID = args[0].GetUInt();
	bool isAccepted = args[1].GetBool();

	CUserClans* clans = gUserProfile.clans[gUserProfile.SelectedCharID];
	int api = clans->ApiClanApplyAnswer(applicationID, isAccepted);
	if(api != 0)
	{
		r3dOutToLog("Failed to answer application, api=%d\n", api);

		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("FailToAnswerApplication"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
	}
}

void FrontendWarZ::eventClanInviteToClan(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 1);
	const char* playerNameToInvite = args[0].GetString();

	CUserClans* clans = gUserProfile.clans[gUserProfile.SelectedCharID];
	
	int api = clans->ApiClanSendInvite(playerNameToInvite);
	if(api != 0)
	{
		r3dOutToLog("Failed to send invite, api=%d\n", api);

		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("FailToSendInvite"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
	}
	else
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("InviteSentSuccess"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
	}
}

void FrontendWarZ::eventClanRespondToInvite(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 2);
	uint32_t inviteID = args[0].GetUInt();
	bool isAccepted = args[1].GetBool();

	CUserClans* clans = gUserProfile.clans[gUserProfile.SelectedCharID];
	int api = clans->ApiClanAnswerInvite(inviteID, isAccepted);
	// remove this invite from the list
	{
		struct clanInviteSearch
		{
			uint32_t inviteID;

			clanInviteSearch(uint32_t id): inviteID(id) {};

			bool operator()(const CUserClans::ClanInvite_s &a)
			{
				return a.ClanInviteID == inviteID;
			}
		};

		clanInviteSearch prd(inviteID);
		clans->clanInvites_.erase(std::find_if(clans->clanInvites_.begin(), clans->clanInvites_.end(), prd));
	}
	if(api != 0)
	{
		r3dOutToLog("Failed to accept invite, api=%d\n", api);

		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("FailAcceptInvite"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
	}
	else if(isAccepted)
	{
		setClanInfo();
		gfxMovie.Invoke("_root.api.Main.showScreen", "MyClan");
	}
	else if(!isAccepted)
	{
		checkForInviteFromClan();
	}
}

void FrontendWarZ::checkForInviteFromClan()
{
	CUserClans* clans = gUserProfile.clans[gUserProfile.SelectedCharID];

	if(!clans->clanInvites_.empty())
	{
		CUserClans::ClanInvite_s& invite = clans->clanInvites_[0];
		//		public function showClanInvite(inviteID:uint, clanName:String, numMembers:uint, desc:String, iconID:uint)
		Scaleform::GFx::Value var[5];
		var[0].SetUInt(invite.ClanInviteID);
		var[1].SetString(invite.ClanName.c_str());
		var[2].SetUInt(0); // todo: need data
		var[3].SetString(""); // todo: need data
		var[4].SetUInt(invite.ClanEmblemID);
		gfxMovie.Invoke("_root.api.showClanInvite", var, 5);
	}
}

void FrontendWarZ::eventClanBuySlots(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 1);
	uint32_t buyIdx = args[0].GetUInt();
	
	CUserClans* clans = gUserProfile.clans[gUserProfile.SelectedCharID];

	int api = clans->ApiClanBuyAddMembers(buyIdx);
	if(api != 0)
	{
		r3dOutToLog("Failed to buy slots, api=%d\n", api);

		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("FailToBuyMoreSlots"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	gfxMovie.SetVariable("_root.api.Main.ClansMyClan.MyClan.OptionsBlock2.Slots.text", clans->clanInfo_.MaxClanMembers);
}

void FrontendWarZ::eventClanApplyToJoin(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 2);
	uint32_t clanID = args[0].GetUInt();
	const char* applText = args[1].GetString();
	CUserClans* clans = gUserProfile.clans[gUserProfile.SelectedCharID];

	if (gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].Hardcore == 1)
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("NotAllowedHardcorePlayers"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	wiCharDataFull& slot = gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID];
	if(slot.ClanID != 0)
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("YouAreAlreadyInClan"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	int api = clans->ApiClanApplyToJoin(clanID, applText);
	if(api != 0)
	{
		r3dOutToLog("Failed to apply to clan, api=%d\n", api);

		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("FailApplyToClan"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
	}
	else
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("SuccessApplyToClan"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
	}
}

static int GetGPConversionRateFromPrice(int price)
{
	for(int i=0; i<32; i++)
	{
            if (price <= gUserProfile.m_GCPriceTable[i * 2 + 0])
                return gUserProfile.m_GCPriceTable[i * 2 + 1];
	}

	return 0;
}

void FrontendWarZ::eventStorePurchaseGPRequest(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 0);
	if(gUserProfile.IsRussianAccount() || gUserProfile.ProfileData.AccountType == 21)
	{
		Scaleform::GFx::Value vars[1];
		vars[0].SetInt(0);
		eventStorePurchaseGP(pMovie, vars, 1);
	}
	else
	{
		gfxMovie.Invoke("_root.api.Main.PurchaseGC.showPopUp", 1); // 1 - to indicate that we want GC dialog, otherwise it will be GD dialog
	}
}

void FrontendWarZ::eventStorePurchaseGP(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==1);

	int priceInCents = args[0].GetInt();
	int howMuchGC = priceInCents * GetGPConversionRateFromPrice(priceInCents) / 100; // calc in cents to avoid rounding issues

	// steam purchase
	if(gSteam.inited_)
	{
		if(gSteam.RecheckUser() == false)
		{
			// steam was closed or user logged off
			Scaleform::GFx::Value args[2];
			args[0].SetString("You are logged out from Steam, please login again");
			args[1].SetBoolean(true);
			gfxMovie.Invoke("_root.api.showInfoMsg", args, 2);		
			return;
		}

		m_buyGpPriceCents  = priceInCents;

		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("OneMomentPlease"));
		var[1].SetBoolean(false);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		
		StartAsyncOperation(&FrontendWarZ::as_SteamBuyGPThread, &FrontendWarZ::OnSteamBuyGPSuccess);
		return;
	}


	// web site purchase
	{
		// open browser with web store open
		char authKey[128];
		gUserProfile.GenerateSessionKey(authKey);
		char url[256];
		//@TH
		sprintf(url, "https://warz.kongsi.asia/account/buygc.php?WzLogin=%s&out=%d", authKey, howMuchGC);
		ShellExecute(NULL, "open", url, "", NULL, SW_SHOW);
	}

	// minimize our window
	ShowWindow(win::hWnd, SW_MINIMIZE);

	/* need to copy from WarInc - update profile after window activation.. OR simply EXIT the game
	needUpdateProfileOnActivate = true;

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
	*/
}

void FrontendWarZ::eventStorePurchaseGD(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==1);
	m_buyGDForGC = args[0].GetInt();

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	StartAsyncOperation(&FrontendWarZ::as_BuyGDForGCThread, &FrontendWarZ::OnBuyGDForGCSuccess);
}

unsigned int FrontendWarZ::as_BuyGDForGCThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;

	This->DelayServerRequest();

	int apiCode = gUserProfile.ApiConvertGamePoints(This->m_buyGDForGC);
	if(apiCode != 0)
	{
		This->SetAsyncError(apiCode, gLangMngr.getString("FailedToConvertGCtoGD"));
		return 0;
	}

	return 1;
}

void FrontendWarZ::OnBuyGDForGCSuccess()
{
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");

	Scaleform::GFx::Value var[1];
	var[0].SetInt(gUserProfile.ProfileData.GamePoints);
	gfxMovie.Invoke("_root.api.setGC", var, 1);

	var[0].SetInt(gUserProfile.ProfileData.GameDollars);
	gfxMovie.Invoke("_root.api.setDollars", var, 1);
}


void FrontendWarZ::eventStorePurchaseGDCallback(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 1);
	int priceInGC = args[0].GetInt();

	int conv = 0;
	int GDResult = gUserProfile.GetConvertedGP2GD(priceInGC, conv);

	char tmpStr[64];
	sprintf(tmpStr, "1GC = %d", conv);

	gfxMovie.SetVariable("_root.api.Main.PurchaseGC.Bar.Rate.text", tmpStr);

	gfxMovie.Invoke("_root.api.Main.PurchaseGC.setGCValue", GDResult);
}

void FrontendWarZ::eventStorePurchaseGPCallback(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 1);
	int priceInCents = args[0].GetInt();

	int conv = GetGPConversionRateFromPrice(priceInCents);

	char tmpStr[64];
	sprintf(tmpStr, "$1 = %d", conv);

	gfxMovie.SetVariable("_root.api.Main.PurchaseGC.Bar.Rate.text", tmpStr);

	int howMuchGC = priceInCents * conv / 100; // calc in cents to avoid rounding issues
	gfxMovie.Invoke("_root.api.Main.PurchaseGC.setGCValue", howMuchGC);
}

unsigned int WINAPI FrontendWarZ::as_SteamBuyGPThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;
	
	int apiCode = gUserProfile.ApiSteamStartBuyGP(This->m_buyGpPriceCents);
	if(apiCode != 0)
	{
		if(apiCode == 7)
		{
			This->SetAsyncError(0, "Your purchase could not be completed because it looks like the currency of funds in your Steam Wallet does not match the currency of this purchase (USD)");
			return 0;
		}
		
		This->SetAsyncError(0, gLangMngr.getString("BuyItemFail"));
		return 0;
	}
	
	// wait for authorization result - for 10min and pray that callback successfully passed to client
	const float endTime = r3dGetTime() + 600.0f;
	while(true)
	{
		::Sleep(10);
		if(gUserProfile.steamAuthResp.gotResp)
			break;
			
		// if we didn't got steam callback
		if(r3dGetTime() > endTime) {
			This->SetAsyncError(0, gLangMngr.getString("BuyItemFail"));
			return 0;
		}
	}

	// if authorization was denied, just silently quit
	r3d_assert(gUserProfile.steamAuthResp.gotResp);
	if(gUserProfile.steamAuthResp.bAuthorized != 1)
		return 1;
			
	// auth ok, finish transaction.
	apiCode = gUserProfile.ApiSteamFinishBuyGP(gUserProfile.steamAuthResp.ulOrderID);
	if(apiCode != 0)
	{
		This->SetAsyncError(0, gLangMngr.getString("BuyItemFail"));
		return 0;
	}
	
	return 1;
}

void FrontendWarZ::OnSteamBuyGPSuccess()
{
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");

	Scaleform::GFx::Value var[1];
	var[0].SetInt(gUserProfile.ProfileData.GamePoints);
	gfxMovie.Invoke("_root.api.setGC", var, 1);
	return;
}

void FrontendWarZ::eventRequestLeaderboardData(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==3);
	int boardType = args[0].GetInt(); // 0-soft, 1-hard
	int boardSelect = args[1].GetInt(); // which board to display, starts with 0
	int mode = args[2].GetInt(); // 0 - default list when you activate leaderboard, 1-top100, 2-move left, 3-move right

	// static is safe here
	static int prevType = 0;

	if(prevType!=boardType)
	{
		prevType = boardType;
		leaderboard_requestStartPos = -1; // reset
	}
	if(leaderboard_BoardSelected != boardSelect)
	{
		leaderboard_BoardSelected = boardSelect;
		leaderboard_requestStartPos = -1; // reset
	}
	if(mode == 1)
		leaderboard_requestStartPos = 0;
	else if(mode == 0)
		leaderboard_requestStartPos = -1;
	else if(mode == 2)
		leaderboard_requestStartPos = R3D_CLAMP(leaderboard_requestStartPos-100, 0, leaderboardSize-100);
	else if(mode == 3)
		leaderboard_requestStartPos = R3D_CLAMP(leaderboard_requestStartPos+100, 0, leaderboardSize-100);

	leaderboard_requestTableID = boardSelect + boardType*50;

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	StartAsyncOperation(&FrontendWarZ::as_RequestLeaderboardThread, &FrontendWarZ::OnRequestLeaderboardSuccess);
}

unsigned int FrontendWarZ::as_RequestLeaderboardThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;

	int outPos = 0;
	int lbSize = 0;
	int apiCode = gUserProfile.ApiGetLeaderboard(This->leaderboard_requestTableID, This->leaderboard_requestStartPos, &outPos, &lbSize);
	if(apiCode != 0)
	{
		This->SetAsyncError(apiCode, gLangMngr.getString("FailedLeaderboardData"));
		return 0;
	}
	if(lbSize)
		This->leaderboardSize = lbSize;

	if(This->leaderboard_requestStartPos==-1)
		This->leaderboard_requestStartPos = 0; // for proper pos index

	return 1;
}

void FrontendWarZ::OnRequestLeaderboardSuccess()
{
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");

	gfxMovie.Invoke("_root.api.Main.LeaderboardAnim.clearLeaderboardList", "");
	{
		Scaleform::GFx::Value args[4];
		char tmpStr[32]={0};
		char tmpPlrName[256]={0};
		for(size_t i=0; i<gUserProfile.m_lbData[leaderboard_requestTableID].size(); ++i)
		{
			args[0].SetInt(leaderboard_requestStartPos + i + 1);
			if(gUserProfile.m_lbData[leaderboard_requestTableID][i].ClanID > 0)
			{
				const char* tagColor = "#000000";
				int ClanTagColor = gUserProfile.m_lbData[leaderboard_requestTableID][i].ClanTagColor;
				switch(ClanTagColor)
				{
				case 1: tagColor = "#aa0000"; break;
				case 2: tagColor = "#a6780c"; break;
				case 3: tagColor = "#10a49e"; break;
				case 4: tagColor = "#20a414"; break;
				case 5: tagColor = "#104ea4"; break;
				case 6: tagColor = "#9610a4"; break;
				case 7: tagColor = "#444444"; break;
				case 8: tagColor = "#a4a4a4"; break;
				default:
					break;
				}
				sprintf(tmpPlrName, "<font color='%s'>[%s]</font> <font color='#D42F2F'>%s</font>", tagColor, gUserProfile.m_lbData[leaderboard_requestTableID][i].ClanTag, gUserProfile.m_lbData[leaderboard_requestTableID][i].gamertag);
			}
			else
				sprintf(tmpPlrName, "<font color='#D42F2F'>%s</font>", gUserProfile.m_lbData[leaderboard_requestTableID][i].gamertag);
			args[1].SetString(tmpPlrName);
			args[2].SetBoolean(gUserProfile.m_lbData[leaderboard_requestTableID][i].Alive>0);
			switch(leaderboard_BoardSelected)
			{
			case 0:
				sprintf(tmpStr, "%d", gUserProfile.m_lbData[leaderboard_requestTableID][i].stats.XP);
				break;
			case 1:
				r3dscpy(tmpStr, getTimePlayedString(gUserProfile.m_lbData[leaderboard_requestTableID][i].stats.TimePlayed));
				break;
			case 2:
				sprintf(tmpStr, "%d", gUserProfile.m_lbData[leaderboard_requestTableID][i].stats.KilledZombies);
				break;
			case 3:
				sprintf(tmpStr, "%d", gUserProfile.m_lbData[leaderboard_requestTableID][i].stats.KilledSurvivors);
				break;
			case 4:
				sprintf(tmpStr, "%d", gUserProfile.m_lbData[leaderboard_requestTableID][i].stats.KilledBandits);
				break;
			case 5:
			case 6:
				sprintf(tmpStr, "%d", gUserProfile.m_lbData[leaderboard_requestTableID][i].stats.Reputation);
				break;
			case 7:
				sprintf(tmpStr, "%d", gUserProfile.m_lbData[leaderboard_requestTableID][i].stats.VictorysHardGames); //gamehardcore
				break;
			default:
				r3d_assert(false);
				break;
			}

			args[3].SetString(tmpStr);
			gfxMovie.Invoke("_root.api.Main.LeaderboardAnim.addLeaderboardData", args, 4);
		}
	}

	switch(leaderboard_BoardSelected)
	{
	case 0:
		gfxMovie.Invoke("_root.api.Main.LeaderboardAnim.setLeaderboardText", gLangMngr.getString("$FR_LB_TOP_XP"));
		break;
	case 1:
		gfxMovie.Invoke("_root.api.Main.LeaderboardAnim.setLeaderboardText", gLangMngr.getString("$FR_LB_TOP_ST"));
		break;
	case 2:
		gfxMovie.Invoke("_root.api.Main.LeaderboardAnim.setLeaderboardText", gLangMngr.getString("$FR_LB_TOP_KZ"));
		break;
	case 3:
		gfxMovie.Invoke("_root.api.Main.LeaderboardAnim.setLeaderboardText", gLangMngr.getString("$FR_LB_TOP_KS"));
		break;
	case 4:
		gfxMovie.Invoke("_root.api.Main.LeaderboardAnim.setLeaderboardText", gLangMngr.getString("$FR_LB_TOP_KB"));
		break;
	case 5:
	case 6:
		gfxMovie.Invoke("_root.api.Main.LeaderboardAnim.setLeaderboardText", gLangMngr.getString("$FR_LB_TOP_RT"));
		break;
	case 7:
		gfxMovie.Invoke("_root.api.Main.LeaderboardAnim.setLeaderboardText", gLangMngr.getString("$FR_LB_TOP_HG")); //gamehardcore
		break;
	default:
		r3d_assert(false);
		break;
	}

	gfxMovie.Invoke("_root.api.Main.LeaderboardAnim.populateLeaderboard", "");
}

void FrontendWarZ::eventRequestGCTransactionData(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==0);

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	StartAsyncOperation(&FrontendWarZ::as_RequestGCTransactionThread, &FrontendWarZ::OnRequestGCTransactionSuccess);
}

unsigned int FrontendWarZ::as_RequestGCTransactionThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;
	
	This->DelayServerRequest();

	int apiCode = gUserProfile.ApiGetGPTransactions();
	if(apiCode != 0)
	{
		This->SetAsyncError(apiCode, gLangMngr.getString("FailedToGetGCTransactions"));
		return 0;
	}

	return 1;
}

void FrontendWarZ::OnRequestGCTransactionSuccess()
{
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");

	// update GamePoints in UI
	{
		Scaleform::GFx::Value var[1];
		var[0].SetInt(gUserProfile.ProfileData.GamePoints);
		gfxMovie.Invoke("_root.api.setGC", var, 1);
	}

	// fill log
	for(size_t i=0; i<gUserProfile.GPLog_.size(); i++)
	{
		const CClientUserProfile::GPLog_s& gl = gUserProfile.GPLog_[i];
		
		Scaleform::GFx::Value args[5];

		char strAmount[64];
		sprintf(strAmount, "%+d", gl.Amount);
		
		struct tm* tm = _gmtime32(&gl.Time);
		char strTime[64];
		sprintf(strTime, "%d/%d/%d", tm->tm_mon + 1, tm->tm_mday, 1900 + tm->tm_year);
		
		char strBalance[64];
		sprintf(strBalance, "%d", gl.Previous + gl.Amount);
		
		char strDesc[256*2] = "";
		r3dscpy(strDesc, gl.Description.c_str());

		// shop override, display bought item
		if(strncmp(gl.Description.c_str(), "Shop: ", 6) == 0)
		{
			int ItemID = atoi(gl.Description.c_str() + 6);
			const BaseItemConfig* itemCfg = g_pWeaponArmory->getConfig(ItemID);
			if(itemCfg)
			{
				if(ItemID == 301159 || ItemID == 301257 || ItemID == 301399) // early revival item or premium acc
					sprintf(strDesc, "%s", itemCfg->m_StoreName);
				else
					sprintf(strDesc, "Marketplace: %s", itemCfg->m_StoreName);
			}
		}
		
		//public function addTransactionData(id:int, date:String, name:String, amount:String, balance:String)
		args[0].SetInt(gl.TransactionID);
		args[1].SetString(strTime);
		args[2].SetString(strDesc);
		args[3].SetString(strAmount);
		args[4].SetString(strBalance);

		gfxMovie.Invoke("_root.api.Main.Marketplace.addTransactionData", args, 5);
	}
	
	gUserProfile.GPLog_.clear();

	gfxMovie.Invoke("_root.api.Main.Marketplace.showTransactionsPopup", "");
}

void FrontendWarZ::eventRequestMyServerList(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==0);

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	StartAsyncOperation(&FrontendWarZ::as_MyServerListThread, &FrontendWarZ::OnMyServerListSuccess);
}

unsigned int WINAPI FrontendWarZ::as_MyServerListThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;

	// grab server prices
	int apiCode = gUserProfile.serverRent->ApiGetServerRentPrices();
	if(apiCode != 0)
	{
		This->SetAsyncError(0, gLangMngr.getString("ServerRent_FailGetPrice"));
		return 0;
	}
	
	// grab my servers
	apiCode = gUserProfile.serverRent->ApiGetServersList();
	if(apiCode != 0)
	{
		This->SetAsyncError(0, gLangMngr.getString("ServerRent_FailGetList"));
		return 0;
	}
	
	// ask masterserver about each server
	for(size_t i=0; i<gUserProfile.serverRent->serverList.size(); i++)
	{
		This->DelayServerRequest();
		if(!This->ConnectToMasterServer())
			return 0;

		CUserServerRental::myServer_s& srv = gUserProfile.serverRent->serverList[i];

		gMasterServerLogic.SendMyServerGetInfo(srv.ginfo.gameServerId, srv.AdminKey);
	
		const float endTime = r3dGetTime() + 10.0f;
		while(r3dGetTime() < endTime)
		{
			::Sleep(10);

			if(!gMasterServerLogic.IsConnected())
				return 0;

			if(gMasterServerLogic.serverInfoAnswered_)
			{
				srv.status     = gMasterServerLogic.serverInfoAnswer_.status;
				srv.curPlayers = (int)gMasterServerLogic.serverInfoPlayers_.size();
				break;
			}
		}
	}
	
	return 1;
}

void FrontendWarZ::OnMyServerListSuccess()
{
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");

	static bool shownSetupServerInfo = false;
	bool needtoShowSetupServerInfo = false;

	// fill server list
	for(size_t i=0; i<gUserProfile.serverRent->serverList.size(); i++)
	{
		const CUserServerRental::myServer_s& srv = gUserProfile.serverRent->serverList[i];

		// addServerInfo(0, "first", "east", "us", "online", 50, 100, "99D 45H 53M", "GAME SERVER", "COLORADO");
		// addServerInfo(serverID:uint, name:String, location:String, region:String, status:String, playersOnline:uint, maxPlayers:uint, timeLeft:String, type:String, map:String)
		
		const char* location = "";
		const char* region = "??";

		if (srv.ginfo.region == GBNET_REGION_US_West)
			region = "US";
		else if (srv.ginfo.region == GBNET_REGION_Europe)
			region = "EU";
		else if (srv.ginfo.region == GBNET_REGION_Russia)
			region = "RU";
		else if (srv.ginfo.region == GBNET_REGION_SouthAmerica)
			region = "SA";

		char timeLeft[64];
		bool serverExpired = false;
		if(srv.WorkHours >= srv.RentHours)
		{
			sprintf(timeLeft, gLangMngr.getString("ServerStatus_Expired"));
			serverExpired = true;
		}
		else
			sprintf(timeLeft, gLangMngr.getString("ServerRental_TimeLeft"), (srv.RentHours - srv.WorkHours) / 24, (srv.RentHours - srv.WorkHours) % 24);

		const char* status = gLangMngr.getString("ServerStatus_Unknown");
		switch(srv.status) 
		{
			case 1: status = gLangMngr.getString("ServerStatus_Pending"); if(!serverExpired) needtoShowSetupServerInfo=true; break;
			case 2: //FAKE: all servers are always ONLINE. 
			case 3: status = gLangMngr.getString("ServerStatus_Online"); break;
			case 4: status = gLangMngr.getString("ServerStatus_Starting"); break;
		}

		if(serverExpired)
			status = gLangMngr.getString("ServerStatus_Offline");
		
		const char* mapName = getMapName(srv.ginfo.mapId);

		Scaleform::GFx::Value vars[11];
		vars[0].SetInt(srv.ginfo.gameServerId);
		vars[1].SetString(srv.ginfo.name);
		vars[2].SetString(location);
		vars[3].SetString(region);
		vars[4].SetString(status);
		vars[5].SetInt(srv.curPlayers);
		vars[6].SetInt(srv.ginfo.maxPlayers);
		vars[7].SetString(timeLeft);
		vars[8].SetString(srv.ginfo.IsGameworld() ? gLangMngr.getString("MapType_Gameworld") : gLangMngr.getString("MapType_Stronghold"));
		vars[9].SetString(mapName);
		vars[10].SetBoolean(true); // is Renew button visible
		gfxMovie.Invoke("_root.api.Main.PlayGameMyServers.addServerInfo", vars, 11);
	}

	gfxMovie.Invoke("_root.api.Main.PlayGameMyServers.showServerList", "");

	if(needtoShowSetupServerInfo && !shownSetupServerInfo)
	{
		shownSetupServerInfo = true;
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("ServerRental_SetupStatusInfo"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
	}
}

void FrontendWarZ::eventRequestMyServerInfo(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==1);
	m_myGameServerId       = args[0].GetUInt();

	const CUserServerRental::myServer_s* srv = gUserProfile.serverRent->GetServerById(m_myGameServerId);
	m_myGameServerAdminKey = srv->AdminKey;

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	StartAsyncOperation(&FrontendWarZ::as_MyServerInfoThread, &FrontendWarZ::OnMyServerInfoSuccess);
}

unsigned int WINAPI FrontendWarZ::as_MyServerInfoThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;

	This->DelayServerRequest();
	if(!This->ConnectToMasterServer())
		return 0;
		
	gMasterServerLogic.SendMyServerGetInfo(This->m_myGameServerId, This->m_myGameServerAdminKey);
	
	const float endTime = r3dGetTime() + 60.0f;
	while(r3dGetTime() < endTime)
	{
		::Sleep(10);

		if(!gMasterServerLogic.IsConnected())
			return 0;

		if(gMasterServerLogic.serverInfoAnswered_)
			return 1;
	}

	This->SetAsyncError(0, gLangMngr.getString("FailedReceiveGameList"));
	return 0;
}

void FrontendWarZ::OnMyServerInfoSuccess()
{
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");

	// add all players
	for(size_t i=0; i<gMasterServerLogic.serverInfoPlayers_.size(); i++)
	{
		const GBPKT_M2C_MyServerAddPlayer_s& plr = gMasterServerLogic.serverInfoPlayers_[i];
	
		// addPlayerInfo(name:String, status:String, rep:String, xp:int)
		Scaleform::GFx::Value vars[4];
		vars[0].SetString(plr.gamertag);
		vars[1].SetString(gLangMngr.getString("PlayerStatus_Alive"));
		vars[2].SetString(getReputationString(plr.reputation));
		vars[3].SetInt(plr.XP);
		gfxMovie.Invoke("_root.api.Main.PlayGameMyServers.addPlayerInfo", vars, 4);
	}
	gfxMovie.Invoke("_root.api.Main.PlayGameMyServers.showPlayerList", "");

	const CUserServerRental::myServer_s* srv = gUserProfile.serverRent->GetServerById(m_myGameServerId);
	
	// update player count in server info
	{
		Scaleform::GFx::Value vars[2];
		vars[0].SetUInt(m_myGameServerId);
		vars[1].SetUInt(gMasterServerLogic.serverInfoPlayers_.size());
		gfxMovie.Invoke("_root.api.Main.PlayGameMyServers.updatePlayerOnlineForServerID", vars, 2);
	}

	{
		Scaleform::GFx::Value var[8];
		var[0].SetBoolean(true);
		var[1].SetInt((srv->ginfo.flags & GBGameInfo::SFLAGS_Nameplates) ? 1 : 0); // nameplate
		var[2].SetInt((srv->ginfo.flags & GBGameInfo::SFLAGS_CrossHair) ? 1 : 0); // crosshair
		var[3].SetInt((srv->ginfo.flags & GBGameInfo::SFLAGS_Tracers) ? 1 : 0); // tracers
		var[4].SetUInt(srv->ginfo.gameTimeLimit); // gametime limit, if any
		var[5].SetInt((srv->ginfo.flags & GBGameInfo::SFLAGS_TrialsAllowed) ? 1 : 0); // trials
		var[6].SetInt((srv->ginfo.flags & GBGameInfo::SFLAGS_DisableASR) ? 1 : 0);
		var[7].SetInt((srv->ginfo.flags & GBGameInfo::SFLAGS_DisableSNP) ? 1 : 0);
		gfxMovie.Invoke("_root.api.Main.PlayGameMyServers.setServerInfoSettingsVisibility", var, 8);
	}

	gfxMovie.SetVariable("_root.api.Main.PlayGameMyServers.MyServers.Settings.Password.text", srv->pwd);
}

void FrontendWarZ::eventMyServerKickPlayer(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==2);
	uint32_t serverID = args[0].GetUInt();
	const char* plrName = args[1].GetString();
	
	m_myKickCharID = 0;
	for(size_t i=0; i<gMasterServerLogic.serverInfoPlayers_.size(); i++)
	{
		const GBPKT_M2C_MyServerAddPlayer_s& plr = gMasterServerLogic.serverInfoPlayers_[i];
		if(strcmp(plrName, plr.gamertag) == 0)
		{
			m_myKickCharID = plr.CharID;
			break;
		}
	}
	r3d_assert(m_myKickCharID);

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	StartAsyncOperation(&FrontendWarZ::as_MyServerKickPlayerThread, &FrontendWarZ::OnMyServerKickPlayerSuccess);
}

unsigned int WINAPI FrontendWarZ::as_MyServerKickPlayerThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;

	This->DelayServerRequest();
	if(!This->ConnectToMasterServer())
		return 0;
		
	gMasterServerLogic.SendMyServerKickPlayer(This->m_myGameServerId, This->m_myGameServerAdminKey, This->m_myKickCharID);
	
	// there will be no answer, so just wait a few seconds and return to reload server info
	::Sleep(1000);
	return 1;
}

void FrontendWarZ::OnMyServerKickPlayerSuccess()
{
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");
}

//
// WARNING: following price functions must be in sync with backend api_ServersMgr.aspx calcServerRentPrice
//
static int getServerRentOptionPrice(int base, int opt)
{
	// get percent of base price
	int add = base * opt / 100;
	return add;
}

static int calcServerRentPrice(const Scaleform::GFx::Value* args)
{
	//isGameServer:int, mapID:int, regionID:int, slotID:int, rentID:int, name:String, password:String, pveID:int, nameplates:int, crosshair:int, tracers:int
	int isGameServer = args[0].GetInt();
	int mapID        = args[1].GetInt();
	int regionID     = args[2].GetInt();
	int slotID       = args[3].GetInt();
	int rentID       = args[4].GetInt();
	const char* name     = args[5].GetString();
	const char* password = args[6].GetString();
	int pveID        = args[7].GetInt();
	int nameplates   = args[8].GetInt();
	int crosshair    = args[9].GetInt();
	int tracers      = args[10].GetInt();

	CUserServerRental::priceOps_s& opt = isGameServer ? gUserProfile.serverRent->optGameServer : gUserProfile.serverRent->optStronghold;
	
	// calc base
	int base = 0;
	switch(regionID)
	{
		case 1:  base = opt.Base_US; break;
		case 10: base = opt.Base_EU; break;
		case 20: base = opt.Base_RU; break;
		case 30: base = opt.Base_SA; break;
		default: r3dError("bad regionID");
	}
	if(base == 0) return 0;
		
	// calc all other options
	int price = base;

	//[TH] price is set for slot, not by percents
	switch(slotID)
	{
		case 0: price = opt.Slot1; break;
		case 1: price = opt.Slot2; break;
		case 2: price = opt.Slot3; break;
		case 3: price = opt.Slot4; break;
		case 4: price = opt.Slot5; break;
		default: r3dError("bad slotID");
	}
	
	if(*password)
		price += getServerRentOptionPrice(base, opt.Passworded);
	
	if(pveID)
		price += getServerRentOptionPrice(base, opt.PVE);
	else
		price += getServerRentOptionPrice(base, opt.PVP);
		
	if(nameplates)
		price += getServerRentOptionPrice(base, opt.OptNameplates);
	if(crosshair)
		price += getServerRentOptionPrice(base, opt.OptCrosshair);
	if(tracers)
		price += getServerRentOptionPrice(base, opt.OptTracers);

	// hourly rent
	if(rentID >= CUserServerRental::HOURLY_RENTID_START)
	{
		// hourly rent is used for renew ONLY.
		// so we can ignore negative prices (not enough capacity)
		// that'll fix rounding issue with API
		if(price < 0) price = -price;
	
		// 10% add for hourly rent
		price += price/10;
		price  = (int)ceilf((float)price/24.0f/31.0f);
		price  = (rentID-CUserServerRental::HOURLY_RENTID_START)*price;
		return price;
	}
	
	// [TH] adjust per day coeff
	switch(rentID)
	{
		case 0: price *= 3; break;
		case 1: price *= 7; break;
		case 2: price *= 15; break;
		case 3: price *= 30; break;
		case 4: price *= 60; break;
		default: r3dError("bad rentID");
	}
	
	return price;
}

// some weird conversion functions, legacy.
static void gameInfoTo_calcServerRentPrice_Args(Scaleform::GFx::Value* targs, const GBGameInfo& ginfo, int slotID, int rentID)
{
	targs[0].SetInt(ginfo.IsGameworld() ? 1 : 0);	// int isGameServer = args[0].GetInt();
	targs[1].SetInt(ginfo.mapId);			// int mapID        = args[1].GetInt();
	targs[2].SetInt(ginfo.region);			// int regionID     = args[2].GetInt();
	targs[3].SetInt(slotID);			// int slotID       = args[3].GetInt();
	targs[4].SetInt(rentID);			// int rentID       = args[4].GetInt();
	targs[5].SetString("temp_name");		// const char* name
	targs[6].SetString("");				// const char* password = args[6].GetString();
	targs[7].SetInt(ginfo.flags & GBGameInfo::SFLAGS_PVE ? 1 : 0);	// int pveID        = args[7].GetInt();
	targs[8].SetInt(0);				// int nameplates   = args[8].GetInt();
	targs[9].SetInt(0);				// int crosshair    = args[9].GetInt();
	targs[10].SetInt(0);				// int tracers      = args[10].GetInt();
}

static void calcServerRentPrice_toRentParams(Scaleform::GFx::Value* targs, CUserServerRental::rentParams_s& params)
{
	params.isGameServer = targs[0].GetInt();
	params.mapID        = targs[1].GetInt();
	params.regionID     = targs[2].GetInt();
	params.slotID       = targs[3].GetInt();
	params.rentID       = targs[4].GetInt();
	r3dscpy(params.name,     targs[5].GetString());
	r3dscpy(params.password, targs[6].GetString());
	params.pveID        = targs[7].GetInt();
	params.nameplates   = targs[8].GetInt();
	params.crosshair    = targs[9].GetInt();
	params.tracers      = targs[10].GetInt();
}

void FrontendWarZ::eventRenewServerUpdatePrice(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==2);
	//serverIDToRenew:uint, rentID:int
	DWORD gameServerId = args[0].GetUInt();
	int   rentID       = args[1].GetInt();
	
	CUserServerRental::myServer_s* srv = gUserProfile.serverRent->GetServerById(gameServerId);
	r3d_assert(srv);
	
	// detect slotID from slot numbers
	int slotID = CUserServerRental::GetSlotID(srv->ginfo);
	if(slotID == -1)
	{
		gfxMovie.Invoke("_root.api.Main.RentServerPopup.updateServerPrice", 0);

		// we wasn't able to determine slots
		Scaleform::GFx::Value vars[3];
		vars[0].SetString(gLangMngr.getString("ServerRent_BadRenewSlots"));
		vars[1].SetBoolean(true);
		vars[2].SetString("$ERROR");
		gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
		return;
	}

	// fill temporary array to be in sync with calcServerRentPrice arguments
	Scaleform::GFx::Value targs[11];
	gameInfoTo_calcServerRentPrice_Args(targs, srv->ginfo, slotID, rentID);

	// fill params array as in eventRentServer
	CUserServerRental::rentParams_s params;
	calcServerRentPrice_toRentParams(targs, params);

	m_rentServerParams = params;
	m_myGameServerId   = gameServerId;
	m_rentServerPrice  = calcServerRentPrice(targs);

	// hack: if we do not have server capacity base price will be *negative* here.
	// but we should always renew, so invert and process
	if(m_rentServerPrice < 0) m_rentServerPrice = -m_rentServerPrice;

	if(m_rentServerPrice == 0)
	{
		Scaleform::GFx::Value vars[3];
		vars[0].SetString(gLangMngr.getString("ServerRent_RegionDisabled"));
		vars[1].SetBoolean(true);
		vars[2].SetString("$ERROR");
		gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
		return;
	}

	gfxMovie.Invoke("_root.api.Main.RentServerPopup.updateServerPrice", m_rentServerPrice);
}

void FrontendWarZ::eventRenewServer(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==2);

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	StartAsyncOperation(&FrontendWarZ::as_RenewServerThread, &FrontendWarZ::OnRenewServerSuccess);
}

unsigned int WINAPI FrontendWarZ::as_RenewServerThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;
	
	int apiCode = gUserProfile.serverRent->ApiRenewServer(This->m_myGameServerId, This->m_rentServerParams, This->m_rentServerPrice);
	if(apiCode != 0)
	{
		This->SetAsyncError(apiCode, gLangMngr.getString("ServerRent_RentFail"));
		return 0;
	}

	return 1;
}

void FrontendWarZ::OnRenewServerSuccess()
{
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");

	Scaleform::GFx::Value var[1];
	var[0].SetInt(gUserProfile.ProfileData.GamePoints);
	gfxMovie.Invoke("_root.api.setGC", var, 1);

	// return back to server screen
	gfxMovie.Invoke("_root.api.Main.showScreen", "MyServers");
	
	// and force to reread servers list
	eventRequestMyServerList(&gfxMovie, NULL, 0);
	
	return;
}

void FrontendWarZ::eventRentServerUpdatePrice(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==11);
	//isGameServer:int, mapID:int, regionID:int, slotID:int, rentID:int, name:String, password:String, pveID:int, nameplates:int, crosshair:int, tracers:int

	m_rentServerPrice = calcServerRentPrice(args);
	if(m_rentServerPrice <= 0)
	{
		Scaleform::GFx::Value vars[3];
		vars[0].SetString(gLangMngr.getString("ServerRent_RegionDisabled"));
		vars[1].SetBoolean(true);
		vars[2].SetString("$ERROR");
		gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
		return;
	}
	
	gfxMovie.Invoke("_root.api.Main.RentServerPopup.updateServerPrice", m_rentServerPrice);
}

void FrontendWarZ::eventRentServer(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==11);
	//isGameServer:int, mapID:int, regionID:int, slotID:int, rentID:int, name:String, password:String, pveID:int, nameplates:int, crosshair:int, tracers:int

	CUserServerRental::rentParams_s params;
	params.isGameServer = args[0].GetInt();
	params.mapID        = args[1].GetInt();
	params.regionID     = args[2].GetInt();
	params.slotID       = args[3].GetInt();
	params.rentID       = args[4].GetInt();
	r3dscpy(params.name,     args[5].GetString());
	r3dscpy(params.password, args[6].GetString());
	params.pveID        = args[7].GetInt();
	params.nameplates   = args[8].GetInt();
	params.crosshair    = args[9].GetInt();
	params.tracers      = args[10].GetInt();
	
	if(strlen(params.name) == 0)
	{
		Scaleform::GFx::Value vars[3];
		vars[0].SetString(gLangMngr.getString("ServerRent_NameTooShort"));
		vars[1].SetBoolean(true);
		vars[2].SetString("$ERROR");
		gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
		return;
	}
	if(strlen(params.name) >= 32)
	{
		Scaleform::GFx::Value vars[3];
		vars[0].SetString(gLangMngr.getString("ServerRent_NameTooBig"));
		vars[1].SetBoolean(true);
		vars[2].SetString("$ERROR");
		gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
		return;
	}

	if(strlen(params.password) >= 16)
	{
		Scaleform::GFx::Value vars[3];
		vars[0].SetString(gLangMngr.getString("ServerRent_PasswordTooBig"));
		vars[1].SetBoolean(true);
		vars[2].SetString("$ERROR");
		gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
		return;
	}
	
	m_rentServerParams = params;
	if(m_rentServerPrice <= 0)
	{
		Scaleform::GFx::Value vars[3];
		vars[0].SetString(gLangMngr.getString("ServerRent_RegionDisabled"));
		vars[1].SetBoolean(true);
		vars[2].SetString("$ERROR");
		gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
		return;
	}
	
	if(m_rentServerPrice > gUserProfile.ProfileData.GamePoints)
	{
		Scaleform::GFx::Value vars[3];
		vars[0].SetString(gLangMngr.getString("NotEnoughGP"));
		vars[1].SetBoolean(true);
		vars[2].SetString("$ERROR");
		gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
		return;
	}

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	StartAsyncOperation(&FrontendWarZ::as_RentServerThread, &FrontendWarZ::OnRentServerSuccess);
}

unsigned int WINAPI FrontendWarZ::as_RentServerThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;
	
	int apiCode = gUserProfile.serverRent->ApiRentServer(This->m_rentServerParams, This->m_rentServerPrice);
	if(apiCode != 0)
	{
		if(apiCode == 9)
			This->SetAsyncError(0, gLangMngr.getString("ServerRent_NameAlreadyInUse"));
		else if(apiCode == 3)
			This->SetAsyncError(0, gLangMngr.getString("ServerRent_PriceDesync"));
		else
			This->SetAsyncError(apiCode, gLangMngr.getString("ServerRent_RentFail"));
		return 0;
	}

	return 1;
}

void FrontendWarZ::OnRentServerSuccess()
{
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");

	Scaleform::GFx::Value var[1];
	var[0].SetInt(gUserProfile.ProfileData.GamePoints);
	gfxMovie.Invoke("_root.api.setGC", var, 1);

	// return back to server screen
	gfxMovie.Invoke("_root.api.Main.showScreen", "MyServers");
	
	// and force to reread servers list
	eventRequestMyServerList(&gfxMovie, NULL, 0);
	
	return;
}

void FrontendWarZ::eventMyServerJoinServer(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==1);
	uint32_t serverID = args[0].GetUInt();

	if (gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].Hardcore == 1)
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("NotAllowedHardcorePlayers"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	// join server
	Scaleform::GFx::Value var[1];
	var[0].SetInt(serverID);
	eventBrowseGamesJoin(pMovie, var, 1);
}

void FrontendWarZ::eventMyServerUpdateSettings(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==9);
	uint32_t serverID = args[0].GetUInt();
	const char* newPassword = args[1].GetString();
	int isNameplateOn = args[2].GetInt();
	int isCrosshairOn = args[3].GetInt();
	int isTracersOn = args[4].GetInt();
	uint32_t gameTimeLimit = args[5].GetUInt();
	if(gameTimeLimit > 99999)
		gameTimeLimit = 0;
	int isTrialsAllowed = args[6].GetInt();
	int disableASR = args[7].GetInt();
	int disableSNP = args[8].GetInt();

	if(strlen(newPassword) >= 16)
	{
		Scaleform::GFx::Value vars[3];
		vars[0].SetString(gLangMngr.getString("ServerRent_PasswordTooBig"));
		vars[1].SetBoolean(true);
		vars[2].SetString("$ERROR");
		gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
		return;
	}

	CUserServerRental::myServer_s* srv = gUserProfile.serverRent->GetServerById(m_myGameServerId);
	m_myServerFlags = srv->ginfo.flags;
	m_myServerFlags &= ~GBGameInfo::SFLAGS_Nameplates;
	m_myServerFlags &= ~GBGameInfo::SFLAGS_CrossHair;
	m_myServerFlags &= ~GBGameInfo::SFLAGS_Tracers;
	m_myServerFlags &= ~GBGameInfo::SFLAGS_TrialsAllowed;
	m_myServerFlags &= ~GBGameInfo::SFLAGS_DisableASR;
	m_myServerFlags &= ~GBGameInfo::SFLAGS_DisableSNP;
	if(isNameplateOn) m_myServerFlags |= GBGameInfo::SFLAGS_Nameplates;
	if(isCrosshairOn) m_myServerFlags |= GBGameInfo::SFLAGS_CrossHair;
	if(isTracersOn)   m_myServerFlags |= GBGameInfo::SFLAGS_Tracers;
	if(isTrialsAllowed)   m_myServerFlags |= GBGameInfo::SFLAGS_TrialsAllowed;
	if(disableASR)   m_myServerFlags |= GBGameInfo::SFLAGS_DisableASR;
	if(disableSNP)   m_myServerFlags |= GBGameInfo::SFLAGS_DisableSNP;
	
	r3dscpy(m_myServerPwd, newPassword);

	m_myServerGameTimeLimit = gameTimeLimit;

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	StartAsyncOperation(&FrontendWarZ::as_MyServerSetSettingsThread, &FrontendWarZ::OnMyServerSetSettingsSuccess);
}

unsigned int WINAPI FrontendWarZ::as_MyServerSetSettingsThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;
	
	int apiCode = gUserProfile.serverRent->ApiSetServerParams(This->m_myGameServerId, This->m_myServerPwd, This->m_myServerFlags, This->m_myServerGameTimeLimit);
	if(apiCode != 0)
	{
		This->SetAsyncError(apiCode, gLangMngr.getString("ServerRent_SetPwdFail"));
		return 0;
	}

	// send password change request to server
	This->DelayServerRequest();
	if(!This->ConnectToMasterServer())
		return 0;
		
	gMasterServerLogic.SendMyServerSetParams(This->m_myGameServerId, This->m_myGameServerAdminKey, This->m_myServerPwd, This->m_myServerFlags, This->m_myServerGameTimeLimit);

	return 1;
}

void FrontendWarZ::OnMyServerSetSettingsSuccess()
{
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");

	CUserServerRental::myServer_s* srv = gUserProfile.serverRent->GetServerById(m_myGameServerId);
	r3dscpy(srv->pwd, m_myServerPwd);
	srv->ginfo.flags = (BYTE)m_myServerFlags;
	srv->ginfo.gameTimeLimit = m_myServerGameTimeLimit;

	// set again
	gfxMovie.SetVariable("_root.api.Main.PlayGameMyServers.MyServers.Settings.Password.text", srv->pwd);

	return;
}

void FrontendWarZ::eventLearnSkill(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 1);
	m_SkillID = args[0].GetUInt();

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	StartAsyncOperation(&FrontendWarZ::as_LearnSkillThread, &FrontendWarZ::OnLearnSkillSuccess);
}

unsigned int WINAPI FrontendWarZ::as_LearnSkillThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;

	int apiCode = gUserProfile.ApiSkillLearn(This->m_SkillID, 1);
	if(apiCode != 0)
	{
		This->SetAsyncError(apiCode, gLangMngr.getString("SkillLearnError"));
		return 0;
	}

	return 1;
}

void FrontendWarZ::OnLearnSkillSuccess()
{
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");

	wiCharDataFull& slot = gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID];

	{
		Scaleform::GFx::Value var[2];
		char tmpGamertag[128];
		if(slot.ClanID != 0)
			sprintf(tmpGamertag, "[%s] %s", slot.ClanTag, slot.Gamertag);
		else
			r3dscpy(tmpGamertag, slot.Gamertag);
		var[0].SetString(tmpGamertag);
		var[1].SetInt(m_SkillID);
		gfxMovie.Invoke("_root.api.setSkillLearnedSurvivor", var, 2);
	}

	updateClientSurvivor(slot);

	gfxMovie.Invoke("_root.api.Main.SkillTree.refreshSkillTree", "");

	return;
}

void FrontendWarZ::eventChangeOutfit(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 4);
	uint32_t slotID = args[0].GetUInt();
	m_ChangeOutfit_newHead = args[1].GetUInt();
	m_ChangeOutfit_newBody = args[2].GetUInt();
	m_ChangeOutfit_newLegs = args[3].GetUInt();

	r3d_assert(gUserProfile.SelectedCharID == slotID); 

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	StartAsyncOperation(&FrontendWarZ::as_ChangeOutfitThread, &FrontendWarZ::OnChangeOutfitSuccess);
}

unsigned int WINAPI FrontendWarZ::as_ChangeOutfitThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;

	int apiCode = gUserProfile.ApiCharChangeSkin(This->m_ChangeOutfit_newHead, This->m_ChangeOutfit_newBody, This->m_ChangeOutfit_newLegs);
	if(apiCode != 0)
	{
		This->SetAsyncError(apiCode, gLangMngr.getString("ChangeOutfitError"));
		return 0;
	}

	return 1;
}

void FrontendWarZ::OnChangeOutfitSuccess()
{
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");

	wiCharDataFull& slot = gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID];

	updateClientSurvivor(slot);

	// during change outfit we show player without items, so once we are returning back, re-load player with all his items
	m_Player->uberAnim_->anim.StopAll();	// prevent animation blending on loadout switch
	m_Player->UpdateLoadoutSlot(slot, -1);

	gfxMovie.Invoke("_root.api.Main.ChangeOutfit.EventChangeOnSuccess", "");

	return;
}

unsigned int WINAPI FrontendWarZ::as_CharRenameThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;

	if(gUserProfile.ProfileData.GamePoints < gUserProfile.ShopCharRename)
	{
		This->SetAsyncError(0, gLangMngr.getString("NotEnoughGC"));
		return 0;
	}

	int minutesLeft = 0;
	int apiCode = gUserProfile.ApiCharRename(This->m_CreateGamerTag, &minutesLeft);
	if(apiCode == 4)
	{
		char buf[1024];
		// temp sring. maybe create a valid date and show it
		int daysLeft = minutesLeft/1440;
		int hoursLeft = minutesLeft/60;
		sprintf(buf, gLangMngr.getString("ChangeNameTimeError"), (daysLeft>0)?(daysLeft):(hoursLeft), (daysLeft>0)?gLangMngr.getString("$FR_Store_PluralDay"):gLangMngr.getString("$FR_PAUSE_INVENTORY_HOURS"));
		This->SetAsyncError(0, buf);
		return 0;
	}
	if(apiCode != 0)
	{
		if(apiCode == 9)
			This->SetAsyncError(0, gLangMngr.getString("ThisNameIsAlreadyInUse"));
		else
			This->SetAsyncError(apiCode, gLangMngr.getString("CharRenameError"));
		return 0;
	}

	return 1;
}

void FrontendWarZ::OnCharRenameSuccess()
{
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");

	wiCharDataFull& slot = gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID];

	{
		char tmpGamertag_old[128];
		char tmpGamertag[128];
		if(slot.ClanID != 0)
		{
			sprintf(tmpGamertag_old, "[%s] %s", slot.ClanTag, CharRename_PreviousName);
			sprintf(tmpGamertag, "[%s] %s", slot.ClanTag, slot.Gamertag);
		}
		else
		{
			r3dscpy(tmpGamertag_old, CharRename_PreviousName);
			r3dscpy(tmpGamertag, slot.Gamertag);
		}

		Scaleform::GFx::Value var[2];
		var[0].SetString(tmpGamertag_old);
		var[1].SetString(tmpGamertag);
		gfxMovie.Invoke("_root.api.changeSurvivorName", var, 2);
	}

	updateClientSurvivor(slot);

	Scaleform::GFx::Value var[2];
	var[0].SetInt(gUserProfile.ProfileData.GamePoints);
	gfxMovie.Invoke("_root.api.setGC", var, 1);

	return;
}

void FrontendWarZ::eventSetCurrentBrowseChannel(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==2);
	uint32_t channel = args[0].GetUInt();
	bool quickGameRequest = args[1].GetBool();

	if(channel == 4 && gUserProfile.ProfileData.PremiumAcc == 0) // premium required
	{
		gfxMovie.Invoke("_root.api.Main.PremiumNeededPopUp.showPopUp", "");
		return;
	}

	if(channel == 1 && gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].Hardcore == 0) // hardcore character required
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("OnlyForHardcorePlayers"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	if(channel != 1 && gUserProfile.ProfileData.ArmorySlots[gUserProfile.SelectedCharID].Hardcore == 1) // normal character required
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("NotAllowedHardcorePlayers"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	CurrentBrowseChannel = channel;
	if(quickGameRequest)
		gfxMovie.Invoke("_root.api.Main.showScreen", "PlayGameQuick");
	else
		gfxMovie.Invoke("_root.api.Main.showScreen", "ServerBrowse");
}

void FrontendWarZ::eventTrialRequestUpgrade(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 0);

	// web site purchase
	{
		char url[256];
		if(gUserProfile.ProfileData.AccountType == 21)
			sprintf(url, "http://warz.kongsi.asia/buy");
		else
			sprintf(url, "https://warz.kongsi.asia/#getgame");
		ShellExecute(NULL, "open", url, "", NULL, SW_SHOW);
	}

	// minimize our window
	ShowWindow(win::hWnd, SW_MINIMIZE);

	// show upgrade dialog?
	gfxMovie.Invoke("_root.api.showTrialUpgradeWindow", "");
	
}

void FrontendWarZ::eventTrialUpgradeAccount(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==1);
	const char* upgradeCode = args[0].GetString();

	if(strlen(upgradeCode) != 19 && strlen(upgradeCode) != 29) {
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("UpgradeTrialAccount_WrongSerialFormat"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	int apiCode = gUserProfile.ApiUpgradeTrialAccount(upgradeCode);
	const char* msg = "";
	if(apiCode==0)
		msg = gLangMngr.getString("UpgradeTrialAccount_Success");
	else if(apiCode==2)
		msg = gLangMngr.getString("UpgradeTrialAccount_CannotUpgrade");
	else if(apiCode==3)
		msg = gLangMngr.getString("UpgradeTrialAccount_SerialNotValid");
	else if(apiCode==4)
		msg = gLangMngr.getString("UpgradeTrialAccount_SerialUsed");
	else
		msg = gLangMngr.getString("UpgradeTrialAccount_Failed");

	if(apiCode==0)
	{
		gfxMovie.SetVariable("_root.api.Main.SurvivorsAnim.Survivors.BtnUpgtrial.visible", false);
		gfxMovie.SetVariable("_root.api.isTrialAccount", false);
	}

	Scaleform::GFx::Value var[2];
	var[0].SetString(msg);
	var[1].SetBoolean(true);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	return;
}

void FrontendWarZ::eventBuyPremiumAccount(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==0);

	mStore_BuyItemID = 301257; // premium account itemID
	mStore_BuyPrice = gUserProfile.ShopPremiumAcc;
	mStore_BuyPriceGD = 0;

	if (gUserProfile.ProfileData.GamePoints < mStore_BuyPrice)
	{
		Scaleform::GFx::Value var[2];
		var[0].SetString(gLangMngr.getString("NotEnougMoneyToBuyItem"));
		var[1].SetBoolean(true);
		gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);
		return;
	}

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	StartAsyncOperation(&FrontendWarZ::as_BuyItemThread, &FrontendWarZ::OnBuyPremiumSuccess);
}

void FrontendWarZ::OnBuyPremiumSuccess()
{
	Scaleform::GFx::Value var[2];
	var[0].SetInt(gUserProfile.ProfileData.GamePoints);
	gfxMovie.Invoke("_root.api.setGC", var, 1);

	var[0].SetInt(gUserProfile.ProfileData.GameDollars);
	gfxMovie.Invoke("_root.api.setDollars", var, 1);

	gfxMovie.Invoke("_root.api.hideInfoMsg", "");

	gUserProfile.ProfileData.PremiumAcc = 43200; // 30 days of premium
	gfxMovie.SetVariable("_root.api.Main.SurvivorsAnim.Survivors.PremiumAcc.visible", true);

	var[0].SetString(gLangMngr.getString("PremiumAccountBoughtPleaseRestart"));
	var[1].SetBoolean(true);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);


	return;
}

void FrontendWarZ::eventMarketplaceActive(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==0);
	g_shown_marketplace_tutorial->SetBool(true);
	writeGameOptionsFile();
}

void FrontendWarZ::eventRequestShowDonateGCtoServer(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==1);
	int serverID = args[0].GetInt();

	const GBPKT_M2C_GameData_s* gdata = gMasterServerLogic.FindGameById(serverID);
	if(gdata == NULL)
	{
		// server not found by some reason
		r3d_assert(gdata);
		return;
	}

	// detect slotID
	int slotID = CUserServerRental::GetSlotID(gdata->info);
	if(slotID == -1)
	{
		// we wasn't able to determine slots
		Scaleform::GFx::Value vars[3];
		vars[0].SetString(gLangMngr.getString("ServerRent_BadRenewSlots"));
		vars[1].SetBoolean(true);
		vars[2].SetString("$ERROR");
		gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
		return;
	}

	// grab server prices
	static bool haveServerPrices = false;
	if(!haveServerPrices)
	{
		int apiCode = gUserProfile.serverRent->ApiGetServerRentPrices();
		if(apiCode != 0)
		{
			Scaleform::GFx::Value vars[3];
			vars[0].SetString(gLangMngr.getString("ServerRent_FailGetPrice"));
			vars[1].SetBoolean(true);
			vars[2].SetString("$ERROR");
			gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
			return;
		}
		haveServerPrices = true;
	}
	
	// check if region is enabled, check with 1 hour rent
	int rentID = CUserServerRental::HOURLY_RENTID_START + 1;

	// fill temporary array to be in sync with calcServerRentPrice arguments
	Scaleform::GFx::Value targs[11];
	gameInfoTo_calcServerRentPrice_Args(targs, gdata->info, slotID, rentID);

	int rentPrice = calcServerRentPrice(targs);
	if(rentPrice == 0)
	{
		Scaleform::GFx::Value vars[3];
		vars[0].SetString(gLangMngr.getString("ServerRent_RegionDisabled"));
		vars[1].SetBoolean(true);
		vars[2].SetString("$ERROR");
		gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
		return;
	}

	gfxMovie.Invoke("_root.api.Main.DonateGCSrvPopUp.showPopUp", serverID);
}

void FrontendWarZ::eventDonateGCtoServerCallback(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 2);
	int numHours = args[0].GetInt();
	int serverID = args[1].GetInt();

	const GBPKT_M2C_GameData_s* gdata = gMasterServerLogic.FindGameById(serverID);
	r3d_assert(gdata);

	// WARNING: all following is somehow modified copy-paste from eventRenewServerUpdatePrice

	// detect slotID from slot numbers
	int slotID = CUserServerRental::GetSlotID(gdata->info);
	r3d_assert(slotID != -1);
	
	// rentID as hourly rent
	int rentID = CUserServerRental::HOURLY_RENTID_START + numHours;

	// fill temporary array to be in sync with calcServerRentPrice arguments
	Scaleform::GFx::Value targs[11];
	gameInfoTo_calcServerRentPrice_Args(targs, gdata->info, slotID, rentID);

	// fill params array as in eventRentServer
	CUserServerRental::rentParams_s params;
	calcServerRentPrice_toRentParams(targs, params);

	m_rentServerParams = params;
	m_myGameServerId   = serverID;
	m_rentServerPrice  = calcServerRentPrice(targs);

	// hack: if we do not have server capacity base price will be *negative* here.
	// but we should always renew, so invert and process
	if(m_rentServerPrice < 0) m_rentServerPrice = -m_rentServerPrice;

	gfxMovie.Invoke("_root.api.Main.DonateGCSrvPopUp.setGCValue", m_rentServerPrice);
}


void FrontendWarZ::eventDonateGCtoServer(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 2);
	int numHours = args[0].GetInt();
	int serverID = args[1].GetInt();

	Scaleform::GFx::Value var[2];
	var[0].SetString(gLangMngr.getString("OneMomentPlease"));
	var[1].SetBoolean(false);
	gfxMovie.Invoke("_root.api.showInfoMsg", var, 2);

	r3d_assert(m_rentServerParams.rentID > CUserServerRental::HOURLY_RENTID_START);
	r3d_assert(m_rentServerPrice > 0);

	StartAsyncOperation(&FrontendWarZ::as_DonateToServerThread, &FrontendWarZ::OnDonateToServerSuccess);
}

unsigned int WINAPI FrontendWarZ::as_DonateToServerThread(void* in_data)
{
	r3dThreadAutoInstallCrashHelper crashHelper;
	FrontendWarZ* This = (FrontendWarZ*)in_data;
	
	int HoursLeft = 0;
	int apiCode = gUserProfile.serverRent->ApiRenewServer(This->m_myGameServerId, This->m_rentServerParams, This->m_rentServerPrice, &HoursLeft);
	if(apiCode == 4)
	{
		char buf[512];
		sprintf(buf, gLangMngr.getString("ServerRent_DonateNeedWait"), HoursLeft);
		This->SetAsyncError(0, buf);
		return 0;
	}
	if(apiCode != 0)
	{
		This->SetAsyncError(apiCode, gLangMngr.getString("ServerRent_RentFail"));
		return 0;
	}

	return 1;
}

void FrontendWarZ::OnDonateToServerSuccess()
{
	gfxMovie.Invoke("_root.api.hideInfoMsg", "");

	Scaleform::GFx::Value var[1];
	var[0].SetInt(gUserProfile.ProfileData.GamePoints);
	gfxMovie.Invoke("_root.api.setGC", var, 1);

	// display some success message?
	{
		Scaleform::GFx::Value vars[3];
		vars[0].SetString(gLangMngr.getString("ServerRent_DonateOk"));
		vars[1].SetBoolean(true);
		vars[2].SetString("$FR_OK");
		gfxMovie.Invoke("_root.api.showInfoMsg", vars, 3);
	}
	
	return;
}

void FrontendWarZ::eventOpenURL(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==1);
	const char* urlID = args[0].GetString();
	r3d_assert(urlID);

	/*
	if(strcmp(urlID, "clickLink_TOS")==0)
	{
		ShellExecute(NULL, "open", "http://127.0.0.1/index.php?/topic/133636-regarding-bans-for-cheating/", "", NULL, SW_SHOW);

		// minimize our window
		ShowWindow(win::hWnd, SW_MINIMIZE);
	}
	*/
}