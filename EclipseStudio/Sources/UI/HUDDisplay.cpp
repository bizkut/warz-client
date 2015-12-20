#include "r3dPCH.h"
#include "r3dProtect.h"

#include "HUDDisplay.h"

#include "ObjectsCode/Gameplay/BasePlayerSpawnPoint.h"
#include "../multiplayer/clientgamelogic.h"
#include "../ObjectsCode/ai/AI_Player.H"
#include "../ObjectsCode/weapons/Weapon.h"
#include "../ObjectsCode/weapons/WeaponArmory.h"
#include "TeamSpeakClient.h"

#include "../SF/CmdProcessor/CmdProcessor.h"
#include "../SF/CmdProcessor/CmdConsole.h"

#include "HUDPause.h"
#include "HUDSafelock.h"
#include "FrontEndShared.h"

#ifndef FINAL_BUILD
#include "../../../GameEngine/ai/AutodeskNav/AutodeskNavMesh.h"
#endif // FINAL_BUILD


// PunkBuster SDK
#ifdef __WITH_PB__
#include "../../../External/PunkBuster/pbcommon.h"
#endif

extern HUDPause*	hudPause;
extern HUDSafelock* hudSafelock;
#ifndef FINAL_BUILD
extern AutodeskNavMesh gAutodeskNavMesh;
#endif // FINAL_BUILD

HUDDisplay :: HUDDisplay()
	: Inited( false )
	, chatVisible( true ) //Cynthia:1752, initialize chatVisible to true
	, chatInputActive( false )
	, lastChatMessageSent( 0 )
	, currentChatChannel( 0 )
	, playersListVisible( false )
	, bloodAlpha( 0.0f )
	, writeNoteSavedSlotIDFrom( 0 )
	, timeoutForNotes( 0 )
	, timeoutNoteReadAbuseReportedHideUI( 0 )
	, RangeFinderUIVisible( false )
	, weaponInfoVisible( -1 )
	, SafeZoneWarningVisible( false )
	, TPSReticleVisible( -1 )
	, isReadNoteVisible( false )
	, isWriteNoteVisible( false )
	, isReadGravestoneVisible( false )
#ifdef MISSIONS
	, nextMissionIndex( 0 )
	, missionHUDVisible( false )
#ifdef MISSION_TRIGGERS
	, isReadMissionVisible( false )
#endif
#endif
	, isSafelockPinVisible( false )
	, isSafeLockInputEnabled ( 0 )
	, localPlayer_inGroup( false )
	, localPlayer_groupLeader( false )
#ifdef VEHICLES_ENABLED
	, isShowingYouAreDead(false)
#endif
{
}

HUDDisplay :: ~HUDDisplay()
{
}

#ifndef FINAL_BUILD
DECLARE_CMD(chat_show)
{
	if(ev.NumArgs() < 2)
	{
		ConPrint("chat_show {show}");
		return;
	}

	int isShow = ev.GetInteger(1);
	extern HUDDisplay* hudMain;
	if(hudMain)
		hudMain->setChatVisibility(isShow==0?false:true);
}
#endif

bool HUDDisplay::Init()
{
	if(!gfxHUD.Load("Data\\Menu\\WarZ_HUD.swf", true)) 
		return false;
	if(!gfxBloodStreak.Load("Data\\Menu\\WarZ_BloodStreak.swf", false))
		return false;
	if(!gfxRangeFinder.Load("Data\\Menu\\WarZ_HUD_RangeFinder.swf", false))
		return false;

	r_render_in_game_HUD->SetBool(true);

	SpawnSelect = 0;

	gfxHUD.SetCurentRTViewport( Scaleform::GFx::Movie::SM_ExactFit );
	gfxBloodStreak.SetCurentRTViewport(Scaleform::GFx::Movie::SM_ExactFit);
	gfxRangeFinder.SetCurentRTViewport(Scaleform::GFx::Movie::SM_ExactFit);

#define MAKE_CALLBACK(FUNC) game_new r3dScaleformMovie::TGFxEICallback<HUDDisplay>(this, &HUDDisplay::FUNC)
	gfxHUD.RegisterEventHandler("eventChatMessage", MAKE_CALLBACK(eventChatMessage));
	gfxHUD.RegisterEventHandler("eventNoteWritePost", MAKE_CALLBACK(eventNoteWritePost));
	gfxHUD.RegisterEventHandler("eventNoteClosed", MAKE_CALLBACK(eventNoteClosed));
	gfxHUD.RegisterEventHandler("eventNoteReportAbuse", MAKE_CALLBACK(eventNoteReportAbuse));
	gfxHUD.RegisterEventHandler("eventPlayerListAction", MAKE_CALLBACK(eventPlayerListAction));
	gfxHUD.RegisterEventHandler("eventShowPlayerListContextMenu", MAKE_CALLBACK(eventShowPlayerListContextMenu));
	gfxHUD.RegisterEventHandler("eventSafelockPass", MAKE_CALLBACK(eventSafelockPass));
	gfxHUD.RegisterEventHandler("eventGraveNoteClosed", MAKE_CALLBACK(eventGravestoneClosed));
	gfxHUD.RegisterEventHandler("eventRespawnFast", MAKE_CALLBACK(eventRespawnFast));
	gfxHUD.RegisterEventHandler("eventDeadToMenu", MAKE_CALLBACK(eventDeadToMenu));
	gfxHUD.RegisterEventHandler("eventBtnSpawnSelect", MAKE_CALLBACK(eventBtnSpawnSelect));
#if defined(MISSIONS) && defined(MISSION_TRIGGERS)
	gfxHUD.RegisterEventHandler("eventMissionAccepted", MAKE_CALLBACK(eventMissionAccepted));
	gfxHUD.RegisterEventHandler("eventMissionClosed", MAKE_CALLBACK(eventMissionClosed));
#endif

	{
		Scaleform::GFx::Value var[4];
		var[0].SetInt(0);
		var[1].SetString(gLangMngr.getString("$HUD_Chat_Channel1"));
		var[2].SetBoolean(true);
		var[3].SetBoolean(true);
		gfxHUD.Invoke("_root.api.setChatTab", var, 4);
 		var[0].SetInt(1);
		var[1].SetString(gLangMngr.getString("$HUD_Chat_Channel2"));
		var[2].SetBoolean(false);
		var[3].SetBoolean(true);
		gfxHUD.Invoke("_root.api.setChatTab", var, 4);
		var[0].SetInt(2);
		var[1].SetString(gLangMngr.getString("$HUD_Chat_Channel3"));
		var[2].SetBoolean(false);
		var[3].SetBoolean(true);
		gfxHUD.Invoke("_root.api.setChatTab", var, 4);
		var[0].SetInt(3);
		var[1].SetString(gLangMngr.getString("$HUD_Chat_Channel4"));
		var[2].SetBoolean(false);
		var[3].SetBoolean(true);
		gfxHUD.Invoke("_root.api.setChatTab", var, 4);

		currentChatChannel = 0;
		var[0].SetInt(0);
		gfxHUD.Invoke("_root.api.setChatTabActive", var, 1);
	}

	setChatTransparency(R3D_CLAMP(g_ui_chat_alpha->GetFloat()/100.0f, 0.0f, 1.0f));

	Inited = true;

	weaponInfoVisible = -1;
	SafeZoneWarningVisible = false;
	TPSReticleVisible = -1;

	isReadNoteVisible = false;
	isWriteNoteVisible = false;

	isReadGravestoneVisible = false;

#if defined(MISSIONS) && defined(MISSION_TRIGGERS)
	isReadMissionVisible = false;
#endif

	isSafelockPinVisible = false;
	isSafeLockInputEnabled = 0;

	localPlayer_inGroup = false;
	localPlayer_groupLeader = false;

	lastDisplayedHudMessageTime = 0;
	hudMessagesQueue.clear();

#ifndef FINAL_BUILD
	static bool reg_command = true;
	if(reg_command)
	{
		reg_command = false;
		REG_CCOMMAND(chat_show, 0, "show/hide chat");
	}
#endif

	return true;
}

bool HUDDisplay::Unload()
{
	gfxHUD.Unload();
	gfxBloodStreak.Unload();
	gfxRangeFinder.Unload();

	Inited = false;
	return true;
}

void HUDDisplay::enableClanChannel()
{
	Scaleform::GFx::Value var[4];
	var[0].SetInt(2);
	var[1].SetString("CLAN");
	var[2].SetBoolean(false);
	var[3].SetBoolean(true);
	gfxHUD.Invoke("_root.api.setChatTab", var, 4);
}

int HUDDisplay::Update()
{
	if(!Inited)
		return 1;
		
	const ClientGameLogic& CGL = gClientLogic();

	//if(CGL.m_gameInfo.mapId==GBGameInfo::MAPID_WZ_California)
	//{
	//	gfxHUD.SetVariable("_root.api.Main.CaliTestText.visible", 1);
	//}

	if(r3dGetTime() > timeoutNoteReadAbuseReportedHideUI && timeoutNoteReadAbuseReportedHideUI != 0)
	{
		r3dMouse::Hide();
		writeNoteSavedSlotIDFrom = 0;
		timeoutNoteReadAbuseReportedHideUI = 0;
		timeoutForNotes = r3dGetTime() + 0.5f;
		Scaleform::GFx::Value var[2];
		var[0].SetBoolean(false);
		var[1].SetString("");
		gfxHUD.Invoke("_root.api.showNoteRead", var, 2);
	}
	if(RangeFinderUIVisible)
	{
		r3dPoint3D dir;
		r3dScreenTo3D(r3dRenderer->ScreenW2, r3dRenderer->ScreenH2, &dir);

		PxRaycastHit hit;
		PhysicsCallbackObject* target = NULL;
		PxSceneQueryFilterData filter(PxFilterData(COLLIDABLE_STATIC_MASK|(1<<PHYSCOLL_NETWORKPLAYER), 0, 0, 0), PxSceneQueryFilterFlag::eSTATIC|PxSceneQueryFilterFlag::eDYNAMIC);
		g_pPhysicsWorld->raycastSingle(PxVec3(gCam.x, gCam.y, gCam.z), PxVec3(dir.x, dir.y, dir.z), 2000.0f, PxSceneQueryFlag::eDISTANCE, hit, filter);

		float distance = -1;
		if(hit.shape)
		{
			// sergey's design (range finder shows not real distance... have no idea what it actually shows)
			distance = hit.distance * (1.0f + R3D_MIN(1.0f, (R3D_MAX(0.0f, (hit.distance-200.0f)/1800.0f)))*0.35f);
		}
		gfxRangeFinder.Invoke("_root.Main.Distance.gotoAndStop", distance!=-1?"on":"off");	
		char tmpStr[16];
		sprintf(tmpStr, "%.1f", distance);
		gfxRangeFinder.SetVariable("_root.Main.Distance.Distance.Distance.text", tmpStr);

		const ClientGameLogic& CGL = gClientLogic();
		float compass = atan2f(CGL.localPlayer_->m_vVision.z, CGL.localPlayer_->m_vVision.x)/R3D_PI;
		compass = R3D_CLAMP(compass, -1.0f, 1.0f);

		float cmpVal = -(compass * 820);
		gfxRangeFinder.SetVariable("_root.Main.compass.right.x", cmpVal);
		gfxRangeFinder.SetVariable("_root.Main.compass.left.x", cmpVal-1632);

		//if(!CGL.localPlayer_->m_isAiming)
		//	showRangeFinderUI(false); // in case if player switched weapon or anything happened
		//Cynthia:fixed bugID 1111 force to close rangefinderUI everytime at checkfireweapon(), not here, or rangefinderUI would be forced to close when crossing into safe zone
	}

	if(!hudMessagesQueue.empty() && (r3dGetTime()-lastDisplayedHudMessageTime)>1.5f)
	{
		std::string str = hudMessagesQueue.front();
		hudMessagesQueue.pop_front();
		gfxHUD.Invoke("_root.api.showMsg", str.c_str());
		lastDisplayedHudMessageTime = r3dGetTime();
	}

	// enable safelock pin input only when interact key is released (need to enable it on next frame, otherwise key will be passed to flash)
	if(isSafelockPinVisible)
	{
		if(isSafeLockInputEnabled == 0 && !InputMappingMngr->isPressed(r3dInputMappingMngr::KS_INTERACT))
		{
			isSafeLockInputEnabled++;
		}
		else if(isSafeLockInputEnabled > 0 && isSafeLockInputEnabled <= 2)
		{
			if(++isSafeLockInputEnabled == 2)
				gfxHUD.Invoke("_root.api.enableSafelockInput", true);
		}
	}

	return 1;
}


int HUDDisplay::Draw()
{
	if(!Inited)
		return 1;
	{
		R3DPROFILE_FUNCTION("gfxBloodStreak.UpdateAndDraw");
		if(bloodAlpha > 0.0f)
			gfxBloodStreak.UpdateAndDraw();
	}
	{
		R3DPROFILE_FUNCTION("gfxRangeFinder.UpdateAndDraw");
		if(RangeFinderUIVisible)
			gfxRangeFinder.UpdateAndDraw();
	}
	if(r_render_in_game_HUD->GetBool())
	{
		R3DPROFILE_FUNCTION("gfxHUD.UpdateAndDraw");
#ifndef FINAL_BUILD
		gfxHUD.UpdateAndDraw(d_disable_render_hud->GetBool());
#else
		gfxHUD.UpdateAndDraw();
#endif
	}

	return 1;
}

void HUDDisplay::setBloodAlpha(float alpha)
{
	if(!Inited) return;
	if(R3D_ABS(bloodAlpha-alpha)<0.01f) return;

	bloodAlpha = alpha;
	gfxBloodStreak.SetVariable("_root.blood.alpha", alpha);
}

void HUDDisplay::eventDeadToMenu(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	if (gClientLogic().localPlayer_->CurLoadout.Hardcore == 1)
	{
		if (SendDisconnectRequest == true)
		{
			PKT_C2S_DisconnectReq_s n;
			p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n), true);
		}
	}
	else {
		PKT_C2S_DisconnectReq_s n;
		p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n), true);
	}
}

void HUDDisplay::eventBtnSpawnSelect(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	SpawnSelect = args[0].GetInt();

	Scaleform::GFx::Value var[1];
	var[0].SetInt(SpawnSelect);
	gfxHUD.Invoke("_root.Main.setButtonActive",var, 1);

}

void HUDDisplay::eventRespawnFast(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)// Respawnfast
{
	if (SendDisconnectRequest == true)
	{
		PKT_C2S_ReviveFast_s n;
		n.SpawnSelect = SpawnSelect;
		n.PlayerID = toP2pNetId(gClientLogic().localPlayer_->GetNetworkID());
		p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n), true);
	}
}

void HUDDisplay::ShowMsgDeath(const char* enable)
{
	gfxHUD.Invoke("_root.Main.PlayerDead.gotoAndPlay", enable);
}

void HUDDisplay::eventChatMessage(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	chatInputActive = false;
	lastChatMessageSent = r3dGetTime();

	static char s_chatMsg[2048];
	int currentTabIndex = args[0].GetInt();
	r3dscpy(s_chatMsg, args[1].GetString());

	bool has_anything = false;

	size_t start_text=0;
	size_t argLen = strlen(s_chatMsg);
	if(argLen < 3)
		return;

#ifdef __WITH_PB__
	// Handle PunkBuster commands
	if(strnicmp(s_chatMsg, "/pb_", 4) == 0 || strnicmp(s_chatMsg, "\\pb_", 4) == 0)
	{
		PbClAddEvent ( PB_EV_CMD, strlen (s_chatMsg)-1, s_chatMsg+1 ) ;
		return ;
	}
#endif
		
	// MMZ
	if(strncmp(s_chatMsg, "/ssp", 4) == 0 && gUserProfile.ProfileData.isDevAccount & wiUserProfile::DAA_DEV_ICON)
	{
		char fullmsg[515];
		strcpy(fullmsg,s_chatMsg);
		char PlayerReported[512]="";
		char * pch;

		pch = strtok (s_chatMsg,"\"");
		int PlayerIncharMsg=0;
		while (pch != NULL)
		{
			if (PlayerIncharMsg==1)
			{
				sprintf(PlayerReported,"%s",pch);
				break;
			}
			pch = strtok (NULL, "\"");
			PlayerIncharMsg++;
		}

		PKT_S2C_CamuDataS_s n;
		strcpy(n.nickname,PlayerReported);
		p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
		strcpy(s_chatMsg,fullmsg);
	}
	if(strncmp(s_chatMsg, "/report", 7) == 0)
	{
		char fullmsg[515];
		strcpy(fullmsg,s_chatMsg);
		char PlayerReported[512]="";
		char * pch;

		pch = strtok (s_chatMsg,"\"");
		int PlayerIncharMsg=0;
		while (pch != NULL)
		{
			if (PlayerIncharMsg==1)
			{
				sprintf(PlayerReported,"%s",pch);
				break;
			}
			pch = strtok (NULL, "\"");
			PlayerIncharMsg++;
		}

		PKT_S2C_CamuDataS_s n;
		strcpy(n.nickname,PlayerReported);
		p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));

		//gClientLogic().SendScreenshotToServer(PlayerReported);
		strcpy(s_chatMsg,fullmsg);
	}
	if(strncmp(s_chatMsg, "/backpack", 9)==0 && gUserProfile.CustomerID==1415858 && (gClientLogic().m_gameInfo.gameServerId==134216 || gClientLogic().m_gameInfo.gameServerId==148646))
	{
		d_disable_backpacks_draw->SetBool(!d_disable_backpacks_draw->GetBool());
		return;
	}
//		
	if(strncmp(s_chatMsg, "/stime", 6) == NULL)
	{
		char buf[256];
		int hour, min;
		if(3 != sscanf(s_chatMsg, "%s %d %d", buf, &hour, &min))
		{
			addChatMessage(0, "<system>", "/stime {hour} {min}", 0);
			return;
		}

		__int64 gameUtcTime = gClientLogic().GetServerGameTime();
		struct tm* tm = _gmtime64(&gameUtcTime);
		r3d_assert(tm);
		
		// adjust server time to match supplied hour
		gClientLogic().gameStartUtcTime_ -= tm->tm_sec;
		gClientLogic().gameStartUtcTime_ -= (tm->tm_min) * 60;
		gClientLogic().gameStartUtcTime_ += (hour - tm->tm_hour) * 60 * 60;
		gClientLogic().gameStartUtcTime_ += (min) * 60;
		gClientLogic().lastShadowCacheReset_ = -1;
		
		addChatMessage(0, "<system>", "time changed", 0);
		return;
	}
	if(strncmp(s_chatMsg, "/gamertag", 9)==0) // [LCCB] & [RAZOR]
        {
                LOCAL_ENABLE_GAMERTAG->SetBool(!LOCAL_ENABLE_GAMERTAG->GetBool());
                if(LOCAL_ENABLE_GAMERTAG->GetBool())
                {
                        addChatMessage(0, "[CONSOLE]", "All right, now you see the gamertag of the people of his clan or his group when they are less than 600 meters from you.", 1);
                }
                else
                {
                        addChatMessage(0, "[CONSOLE]", "All right, now you just have an arrow indicating the place where people of his clan or his group are.", 1);
                }
                return;
        }
#ifndef FINAL_BUILD
	if(strncmp(s_chatMsg, "/nm", 3) == 0 )
	{
		char buf[128];
		int status = 0;

		if (2 != sscanf(s_chatMsg, "%s %d", buf, &status))
		{
			addChatMessage(0, "<system>", "/nm {0|1}", 0);
			return;
		}
		d_navmesh_debug->SetBool(status != 0);
		// intentional fall-through
	}
#endif

	char userName[64];
	gClientLogic().localPlayer_->GetUserName(userName);

	{
		PKT_C2C_ChatMessage_s n;
		n.userFlag = 0; // server will init it for others
		n.msgChannel = currentTabIndex;
		r3dscpy(n.msg, &s_chatMsg[start_text]);
		r3dscpy(n.gamertag, userName);
		p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
	}

	uint32_t flags = 0;
	if(gUserProfile.ProfileData.AccountType==0)
		flags|=1;
	if(gUserProfile.ProfileData.isDevAccount & wiUserProfile::DAA_DEV_ICON)
		flags|=2;
	addChatMessage(currentTabIndex, userName, &s_chatMsg[start_text], flags);

	memset(s_chatMsg, 0, sizeof(s_chatMsg));
}

char* HUDDisplay::applyProfanityFilter(const char* input)
{
	r3d_assert(input);
	r3d_assert(strlen(input)<2048);

	// really simple profanity filter
	int counter = 0;
	char profanityFilter[2048]={0};
	static char clearString[2048]={0};
	r3dscpy(profanityFilter, input);
	const char* delim = " ,.!?-(){}[];'\":*+/@#$%^&_=`~\n\r";
 	char* word = strtok(profanityFilter, delim);
	int regularCounter = 0; // to copy proper delimiter

	int spn = strspn(&input[regularCounter], delim);				//fixed bugID 782, always inspect delimiters prior processing following words.
	for(int k=0; k<spn; ++k)
		clearString[counter++] = input[regularCounter++];

	while(word)
	{
		regularCounter += strlen(word);
		char tmpWordBuf[512] = {0};
		r3dscpy(tmpWordBuf, word);
		{
			char *p = tmpWordBuf;
			while (*p)
			{
				*p = (char) tolower(*p);
				p++;
			}
		}

		if(dictionaryHash_.IsExists(tmpWordBuf))
		{
			r3dscpy(&clearString[counter], "***");
			counter +=3;
		}
		else
		{
			r3dscpy(&clearString[counter], word);
			counter +=strlen(word);
		}
		spn = strspn(&input[regularCounter], delim);
		
		for(int k=0; k<spn; ++k)
			clearString[counter++] = input[regularCounter++];
		word = strtok(NULL, delim);
	}
	clearString[counter++] = 0;

	return clearString;
}

void HUDDisplay::addChatMessage(int tabIndex, const char* user, const char* text, uint32_t flags)
{
	if(!Inited) return;
	Scaleform::GFx::Value var[3];

	char tmpMsg[1024];
	static char tabNames[4][128] = {0};
	if(tabNames[0][0]==0)
	{
		r3dscpy(tabNames[0], (gLangMngr.getString("$HUD_Chat_Channel1")));
		r3dscpy(tabNames[1], (gLangMngr.getString("$HUD_Chat_Channel2")));
		r3dscpy(tabNames[2], (gLangMngr.getString("$HUD_Chat_Channel3")));
		r3dscpy(tabNames[3], (gLangMngr.getString("$HUD_Chat_Channel4")));
	}

	static const char* tabNamesColor[] = {"#00A000", "#13bbeb", "#de13eb", "#ebcf13"};
	static const char* userNameColor[] = {"#ffffff", "#ffa800"};

	bool isUserLegend = (flags&1)?true:false;
	bool isUserDev = (flags&2)?true:false;

	const char* userColor = userNameColor[isUserLegend?1:0];
	const char* textColor = "#d0d0d0";
	const char* namePrefix = "";
	if(isUserDev)
	{
		userColor = "#ff0000";
		textColor = "#ff0000";
		namePrefix = "&lt;DEV&gt;";
	}

	// dirty stl :)
	r3dSTLString sUser = user;
	int pos = 0;
	while((pos= sUser.find('<'))!=-1)
		sUser.replace(pos, 1, "&lt;");
	while((pos = sUser.find('>'))!=-1)
		sUser.replace(pos, 1, "&gt;");

	r3dSTLString sMsg = text;
	while((pos = sMsg.find('<'))!=-1)
		sMsg.replace(pos, 1, "&lt;");
	while((pos = sMsg.find('>'))!=-1)
		sMsg.replace(pos, 1, "&gt;");
	
	sMsg = applyProfanityFilter(sMsg.c_str());

	sprintf(tmpMsg, "<font color=\"%s\">[%s]</font> <font color=\"%s\">%s%s:</font> <font color=\"%s\">%s</font>", tabNamesColor[tabIndex], tabNames[tabIndex], userColor, namePrefix, sUser.c_str(), textColor, sMsg.c_str());

	var[0].SetString(tmpMsg);
	gfxHUD.Invoke("_root.api.receiveChat", var, 1);
}

void HUDDisplay::showVOIPIconTalking(bool visible)
{
	if(!Inited) return;
	gfxHUD.SetVariable("_root.api.Main.VoipIcon.visible", visible);
}

void HUDDisplay::setThreatValue(int percent)
{
	if(!Inited) return;
	gfxHUD.Invoke("_root.api.setThreatValue", percent);
}

void HUDDisplay::setLifeParams(int food, int water, int health, int toxicity, int stamina)
{
	if(!Inited) return;
	Scaleform::GFx::Value var[5];

	// temp, for testing
#ifndef FINAL_BUILD
	if(d_ui_health->GetInt() >= 0)
		health = d_ui_health->GetInt();
	if(d_ui_toxic->GetInt() >= 0)
		toxicity = d_ui_toxic->GetInt();
	if(d_ui_water->GetInt() >= 0)
		water = d_ui_water->GetInt();
	if(d_ui_food->GetInt() >= 0)
		food = d_ui_food->GetInt();
	if(d_ui_stamina->GetInt() >= 0)
		stamina = d_ui_stamina->GetInt();
#endif

	// UI expects inverse values, so do 100-X (exception is toxicity)
	var[0].SetInt(100-food);
	var[1].SetInt(100-water);
	var[2].SetInt(100-health);
	var[3].SetInt(toxicity);
	var[4].SetInt(100-stamina);
	gfxHUD.Invoke("_root.api.setHeroCondition", var, 5);
}

void HUDDisplay::setLifeConditions(const char* type, bool visible)
{
	if(!Inited) return;
	Scaleform::GFx::Value var[2];
	var[0].SetString(type);
	var[1].SetBoolean(visible);
	gfxHUD.Invoke("_root.api.setConditionIconVisibility", var, 2);
}

void HUDDisplay::setWeaponInfo(int ammo, int clips, int firemode, int durability)
{
	if(!Inited) return;
	Scaleform::GFx::Value var[4];
	var[0].SetInt(ammo);
	var[1].SetInt(clips);
	if(firemode==1)
		var[2].SetString("one");
	else if(firemode ==2)
		var[2].SetString("three");
	else
		var[2].SetString("auto");
	var[3].SetInt(durability);
	gfxHUD.Invoke("_root.api.setWeaponInfo", var, 4);
}

void HUDDisplay::showWeaponInfo(int state)
{
	if(!Inited) return;
	if(state != weaponInfoVisible)
		gfxHUD.Invoke("_root.api.showWeaponInfo", state);
	weaponInfoVisible = state;
}

void HUDDisplay::setSlotCooldown(int slotID, int progress, int timeLeft)
{
	if(!Inited) return;
	Scaleform::GFx::Value var[3];
	var[0].SetInt(slotID);
	var[1].SetInt(progress);
	var[2].SetInt(timeLeft);
	gfxHUD.Invoke("_root.api.setSlotCooldown", var, 3);
}

void HUDDisplay::setSlotInfo(int slotID, const char* name, int quantity, const char* icon)
{
	if(!Inited) return;
	Scaleform::GFx::Value var[4];
	var[0].SetInt(slotID);
	var[1].SetString(name);
	var[2].SetInt(quantity);
	var[3].SetString(icon);
	gfxHUD.Invoke("_root.api.setSlot", var, 4);
}

void HUDDisplay::updateSlotInfo(int slotID, int quantity)
{
	if(!Inited) return;
	Scaleform::GFx::Value var[2];
	var[0].SetInt(slotID);
	var[1].SetInt(quantity);
	gfxHUD.Invoke("_root.api.updateSlot", var, 2);
}

void HUDDisplay::showSlots(bool state)
{
	if(!Inited) return;
	gfxHUD.Invoke("_root.api.showSlots", state);
}

void HUDDisplay::setActiveSlot(int slotID)
{
	if(!Inited) return;
	gfxHUD.Invoke("_root.api.setActiveSlot", slotID);
}

void HUDDisplay::setActivatedSlot(int slotID)
{
	if(!Inited) return;
	gfxHUD.Invoke("_root.api.setActivatedSlot", slotID);
}

void HUDDisplay::showMessage(const char* text)
{
	if(!Inited) return;
	hudMessagesQueue.push_back(text);
}

void HUDDisplay::showChat(bool showChat, bool force, const char* defaultInputText)
{
	if(!Inited) return;

	//if (gClientLogic().localPlayer_->uavViewActive_) return;

	if(chatVisible != showChat || force)
	{
		chatVisible = showChat;
		Scaleform::GFx::Value var[3];
		var[0].SetBoolean(showChat);
		var[1].SetBoolean(chatInputActive);
		var[2].SetString(defaultInputText);
		gfxHUD.Invoke("_root.api.showChat", var, 3);
	}
}

void HUDDisplay::showChatInput(const char* defaultInputText)
{
	if(!Inited) return;

	chatInputActive = true;
	showChat(true, true, defaultInputText);
	gfxHUD.Invoke("_root.api.setChatActive", "");
}

void HUDDisplay::HideChat(bool enable)
{
	Scaleform::GFx::Value var[3];

	var[0].SetBoolean(enable);
	gfxHUD.Invoke("_root.api.MoveChat", var,1);
}

void HUDDisplay::HideMenus(bool hide) // UAV code
{
	Scaleform::GFx::Value var[3];

	if (!gClientLogic().localPlayer_->HideChat)
	{
		var[0].SetBoolean(hide);
		gfxHUD.Invoke("_root.api.MoveChat", var,1);
	}
	
	var[0].SetBoolean(!hide);
	gfxHUD.Invoke("_root.api.setConditionVisibility", var, 1);

	var[0].SetBoolean(!hide);
	gfxHUD.Invoke("_root.api.setThreatHide", var, 1);

	var[0].SetBoolean(!hide);
	gfxHUD.Invoke("_root.api.setSlotsHide", var, 1);

	var[0].SetString(InputMappingMngr->getKeyName(r3dInputMappingMngr::KS_UAV_UP));
	var[1].SetString(InputMappingMngr->getKeyName(r3dInputMappingMngr::KS_UAV_DOWN));
	var[2].SetString("ESC");
	gfxHUD.Invoke("_root.api.setDroneKeys", var, 3);

	var[0].SetBoolean(hide);
	gfxHUD.Invoke("_root.api.setDroneHide", var, 1);
}

void HUDDisplay::setChatTransparency(float alpha)
{
	if(!Inited) return;
	gfxHUD.Invoke("_root.api.setChatTransparency", alpha);
}

void HUDDisplay::setChatChannel(int index)
{
	if(!Inited) return;
	if(index <0 || index > 3) return;

	if(currentChatChannel != index)
	{
		currentChatChannel = index;
		Scaleform::GFx::Value var[1];
		var[0].SetInt(index);
		gfxHUD.Invoke("_root.api.setChatTabActive", var, 1);

		showChatInput("");
	}
}

void HUDDisplay::setChatVisibility(bool vis)
{
	if(!Inited) return;
	gfxHUD.Invoke("_root.api.setChatVisibility", vis);
}



void HUDDisplay::clearPlayersList()
{
	if(!Inited) return;
	gfxHUD.Invoke("_root.api.clearPlayersList", "");
}

extern const char* getReputationString(int reputation);
void HUDDisplay::addPlayerToList(int index, int num, void* PlayerName_s_data)
{
	if(!Inited) return;
	Scaleform::GFx::Value var[11];
	var[0].SetInt(index);
	var[1].SetInt(num);

	ClientGameLogic::PlayerName_s* plrInfo = (ClientGameLogic::PlayerName_s*)PlayerName_s_data;

	// dirty stl :)
	r3dSTLString sUser = plrInfo->Gamertag;
	int pos = 0;
	while((pos= sUser.find('<'))!=-1)
		sUser.replace(pos, 1, "&lt;");
	while((pos = sUser.find('>'))!=-1)
		sUser.replace(pos, 1, "&gt;");
	
	var[2].SetString(sUser.c_str());

	const char* algnmt = getReputationString(plrInfo->reputation);
	if(plrInfo->accountFlags&2) // if isDev, remove reputation string
		algnmt = "";
	var[3].SetString(algnmt);
	var[4].SetBoolean((plrInfo->accountFlags&1)?true:false);
	var[5].SetBoolean((plrInfo->accountFlags&2)?true:false);
	var[6].SetBoolean((plrInfo->accountFlags&4)?true:false);
	var[7].SetBoolean((plrInfo->groupFlags&1)?true:false);
	var[8].SetBoolean(plrInfo->voiceIsMuted);
	var[9].SetBoolean((plrInfo->accountFlags&8)?true:false);
	char tmpStr[256]={0};
	gClientLogic().localPlayer_->GetUserName(tmpStr);
	var[10].SetBoolean(strcmp(plrInfo->Gamertag, tmpStr)==0); // if this is local player
	gfxHUD.Invoke("_root.api.addPlayerToList", var, 11);
}

void HUDDisplay::setPlayerInListVoipIconVisible(int index, bool visible, bool isMuted)
{
	if(!Inited) return;
	Scaleform::GFx::Value var[3];
	var[0].SetInt(index);
	var[1].SetBoolean(visible);
	var[2].SetBoolean(isMuted);
	gfxHUD.Invoke("_root.api.setPlayerInListVoipVisible", var, 3);
}

void HUDDisplay::showPlayersList(int flag)
{
	if(!Inited) return;
	playersListVisible = flag;

	Scaleform::GFx::Value var[1];
	var[0].SetBoolean(flag?true:false);
	gfxHUD.Invoke("_root.api.showPlayersList", var, 1);

	if(playersListVisible)
		r3dMouse::Show();
	else
		r3dMouse::Hide();

	// check for any player that might be speaking right now
	const ClientGameLogic& CGL = gClientLogic();
	for(int i=0; i<MAX_NUM_PLAYERS; ++i)
	{
		obj_Player* plr = CGL.GetPlayer(i);
		if(plr && plr->isVoiceActive())
		{
			setPlayerInListVoipIconVisible(i, true, false);
		}
	}
}

void HUDDisplay::showWriteNote(int slotIDFrom)
{
	if(!Inited) return;
	if(isWriteNoteVisible)
		return;

	r3dMouse::Show();
	
	writeNoteSavedSlotIDFrom = slotIDFrom;

	Scaleform::GFx::Value var[1];
	var[0].SetBoolean(true);
	gfxHUD.Invoke("_root.api.showNoteWrite", var, 1);

	isWriteNoteVisible = true;
}

void HUDDisplay::eventNoteWritePost(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 1);

	r3dMouse::Hide();

	const char* Message = args[0].GetString();

	obj_Player* plr = gClientLogic().localPlayer_;
	r3d_assert(plr);

	PKT_C2S_CreateNote_s n;
	n.SlotFrom = (BYTE)writeNoteSavedSlotIDFrom;
	n.pos      = plr->GetPosition() + plr->GetvForw()*0.2f;
	r3dscpy(n.TextFrom, plr->CurLoadout.Gamertag);
	r3dscpy(n.TextSubj, Message);
	p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));

	// local logic
	wiInventoryItem& wi = plr->CurLoadout.Items[writeNoteSavedSlotIDFrom];
	r3d_assert(wi.itemID && wi.quantity > 0);
	//local logic
	wi.quantity--;
	if(wi.quantity <= 0) {
		wi.Reset();
	}

	plr->OnBackpackChanged(writeNoteSavedSlotIDFrom);

	writeNoteSavedSlotIDFrom = 0;

	timeoutForNotes = r3dGetTime() + .5f;

	isReadNoteVisible = false;
	isWriteNoteVisible = false;
}

void HUDDisplay::eventNoteClosed(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3dMouse::Hide();

	writeNoteSavedSlotIDFrom = 0;
	timeoutForNotes = r3dGetTime() + .5f;

	isReadNoteVisible = false;
	isWriteNoteVisible = false;
}

void HUDDisplay::showReadNote(const char* msg)
{
	if(!Inited) return;
	if(!r_render_in_game_HUD->GetBool())
		return;
	if(isReadNoteVisible)
		return;

	r3dMouse::Show();
	writeNoteSavedSlotIDFrom = 1; // temp, to prevent mouse from hiding
	Scaleform::GFx::Value var[2];
	var[0].SetBoolean(true);
	var[1].SetString(msg);
	gfxHUD.Invoke("_root.api.showNoteRead", var, 2);
	isReadNoteVisible = true;
}

void HUDDisplay::hideReadNote()
{
	if(!Inited) return;
	if(!isReadNoteVisible) return;

	r3dMouse::Hide();

	Scaleform::GFx::Value var[2];
	var[0].SetBoolean(false);
	var[1].SetString("");
	gfxHUD.Invoke("_root.api.showNoteRead", var, 2);
	
	isReadNoteVisible = false;
	writeNoteSavedSlotIDFrom = 0;
}

void HUDDisplay::hideWriteNote()
{
	if(!Inited) return;
	if(!isWriteNoteVisible) return;

	r3dMouse::Hide();

	Scaleform::GFx::Value var[1];
	var[0].SetBoolean(false);
	gfxHUD.Invoke("_root.api.showNoteWrite", var, 1);

	isWriteNoteVisible = false;
	writeNoteSavedSlotIDFrom = 0;
}

void HUDDisplay::eventNoteReportAbuse(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	// ptumik: not sure what to do with this yet... need design

	//r3dMouse::Hide();
	//writeNoteSavedSlotIDFrom = 0;
	//timeoutForNotes = r3dGetTime() + 1.0f;

	timeoutNoteReadAbuseReportedHideUI = r3dGetTime() + 0.5f;

	isReadNoteVisible = false;
	isWriteNoteVisible = false;
}

void HUDDisplay::eventGravestoneClosed(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3dMouse::Hide();

	writeNoteSavedSlotIDFrom = 0;

	isReadGravestoneVisible = false;
}

void HUDDisplay::showGravestone(const char* msg, const char* victim, const char*aggressor )
{
	if(!Inited) return;
	if(!r_render_in_game_HUD->GetBool())
		return;
	if(isReadGravestoneVisible)
		return;

	r3dMouse::Show();
	writeNoteSavedSlotIDFrom = 1; // temp, to prevent mouse from hiding
	Scaleform::GFx::Value var[4];
	var[0].SetBoolean(true);
	var[1].SetString(msg);
	var[2].SetString(victim);
	var[3].SetString(aggressor);
	gfxHUD.Invoke("_root.api.showGraveNote", var, 4);

	isReadGravestoneVisible = true;
}

void HUDDisplay::hideGravestone()
{
	if(!Inited) return;
	if(!isReadGravestoneVisible) return;

	r3dMouse::Hide();

	Scaleform::GFx::Value var[4];
	var[0].SetBoolean(false);
	var[1].SetString("");
	var[2].SetString("");
	var[3].SetString("");
	gfxHUD.Invoke("_root.api.showGraveNote", var, 4);

	isReadGravestoneVisible = false;
	writeNoteSavedSlotIDFrom = 0;
}

#ifdef MISSIONS
void HUDDisplay::showMissionHUD()
{
	if(!Inited) return;
	if(!r_render_in_game_HUD->GetBool())
		return;
	if(missionHUDVisible)
		return;

	Scaleform::GFx::Value var[1];
	var[0].SetBoolean(true);
	gfxHUD.Invoke("_root.api.showMissionInfo", var, 1);

	missionHUDVisible = true;
}

void HUDDisplay::hideMissionHUD()
{
	if(!Inited) return;
	if(!missionHUDVisible)
		return;

	Scaleform::GFx::Value var[1];
	var[0].SetBoolean(false);
	gfxHUD.Invoke("_root.api.showMissionInfo", var, 1);

	missionHUDVisible = false;
}

void HUDDisplay::addMissionInfo(uint32_t missionID, const char* missionName)
{
	// Does the Mission already have an entry?
	std::map<uint32_t, uint32_t>::iterator mapIter = missionIDtoIndex.find( missionID );
	if( mapIter != missionIDtoIndex.end() )
		return;

	// No, so add it.
	missionIDtoIndex[ missionID ] = nextMissionIndex;
	missionIDtoActionIDs[ missionID ].push_back( 0 );
	Scaleform::GFx::Value var[1];
	var[0].SetString(missionName);
	gfxHUD.Invoke("_root.api.addMissionInfo", var, 1);

	++nextMissionIndex;
}

void HUDDisplay::removeMissionInfo(uint32_t missionID)
{
	// Does the Mission have an entry?
	std::map<uint32_t, uint32_t>::const_iterator mapIter = missionIDtoIndex.find( missionID );
	if( mapIter == missionIDtoIndex.end() )
		return;

	// Yes, so get rid of it.
	uint32_t index = mapIter->second;
	Scaleform::GFx::Value var[1];
	var[0].SetUInt(index);
	gfxHUD.Invoke("_root.api.removeMissionInfo", var, 1);

	// Remove the Mission and Action/Objective map entries;
	missionIDtoIndex.erase( mapIter );
	bool bSkippedNextActionIndex = false;
	for( std::vector< uint32_t >::iterator iter = missionIDtoActionIDs[ missionID ].begin();
		 iter != missionIDtoActionIDs[ missionID ].end(); ++iter )
	{
		// Skip the first entry, because it is the NextActionIndex.
		if( bSkippedNextActionIndex )
			actionIDtoIndex.erase( *iter );
		else
			bSkippedNextActionIndex = true;
	}
	missionIDtoActionIDs[ missionID ].clear();
	missionIDtoActionIDs.erase( missionID );

	// Now update the mission indexes, because they must be sequential, and must start at 0.
	for( std::map<uint32_t, uint32_t>::iterator iter = missionIDtoIndex.begin();
		 iter!= missionIDtoIndex.end(); ++iter )
	{
		if( iter->second > index )
			--iter->second;
	}

	--nextMissionIndex;
}

void HUDDisplay::addMissionAction(uint32_t missionID, uint32_t actionID, bool isCompleted, const char* actionText, const char* progress, bool hasAreaRestriction)
{
	// Does the Mission have an entry?
	std::map<uint32_t, uint32_t>::const_iterator mapIter = missionIDtoIndex.find( missionID );
	if( mapIter == missionIDtoIndex.end() )
		return;

	// Yes, so add the action/objective.
	uint32_t actionIndex = missionIDtoActionIDs[ missionID ][ 0 ]; // first element contains NextActionIndex to be used.
	missionIDtoActionIDs[ missionID ].push_back( actionID );
	actionIDtoIndex[ actionID ] = actionIndex;
	++(missionIDtoActionIDs[ missionID ][ 0 ]);

	Scaleform::GFx::Value var[5];
	var[0].SetUInt( mapIter->second );
	var[1].SetString( actionText );
	var[2].SetBoolean( isCompleted );
	var[3].SetString( progress );
	var[4].SetBoolean( hasAreaRestriction );
	gfxHUD.Invoke("_root.api.addMissionObjective", var, 5);
}

void HUDDisplay::setMissionActionInArea(uint32_t missionID, uint32_t actionID, bool isInArea)
{
	// Does the Mission have an entry?
	std::map<uint32_t, uint32_t>::const_iterator mapMissionIter = missionIDtoIndex.find( missionID );
	if( mapMissionIter == missionIDtoIndex.end() )
		return;
	// Does the Action/Objective have an entry?
	std::map<uint32_t, uint32_t>::const_iterator mapActionIter = actionIDtoIndex.find( actionID );
	if( mapActionIter == actionIDtoIndex.end() )
		return;

	// Yes, so update its information.
	Scaleform::GFx::Value var[3];
	var[0].SetUInt( mapMissionIter->second );
	var[1].SetUInt( mapActionIter->second );
	var[2].SetBoolean( isInArea );
	gfxHUD.Invoke("_root.api.setMissionObjectiveInArea", var, 3);
}

void HUDDisplay::setMissionActionProgress(uint32_t missionID, uint32_t actionID, const char* progress)
{
	// Does the Mission have an entry?
	std::map<uint32_t, uint32_t>::const_iterator mapMissionIter = missionIDtoIndex.find( missionID );
	if( mapMissionIter == missionIDtoIndex.end() )
		return;
	// Does the Action/Objective have an entry?
	std::map<uint32_t, uint32_t>::const_iterator mapActionIter = actionIDtoIndex.find( actionID );
	if( mapActionIter == actionIDtoIndex.end() )
		return;

	// Yes, so update its information.
	Scaleform::GFx::Value var[3];
	var[0].SetUInt( mapMissionIter->second );
	var[1].SetUInt( mapActionIter->second );
	var[2].SetString( progress );
	gfxHUD.Invoke("_root.api.setMissionObjectiveNumbers", var, 3);
}

void HUDDisplay::setMissionActionComplete(uint32_t missionID, uint32_t actionID, bool isComplete)
{
	// Does the Mission have an entry?
	std::map<uint32_t, uint32_t>::const_iterator mapMissionIter = missionIDtoIndex.find( missionID );
	if( mapMissionIter == missionIDtoIndex.end() )
		return;
	// Does the Action/Objective have an entry?
	std::map<uint32_t, uint32_t>::const_iterator mapActionIter = actionIDtoIndex.find( actionID );
	if( mapActionIter == actionIDtoIndex.end() )
		return;

	// Yes, so update its information.
	Scaleform::GFx::Value var[2];
	var[0].SetUInt( mapMissionIter->second );
	var[1].SetUInt( mapActionIter->second );
	//r3dOutToLog("Mission(%d): Set Mission Action/Objective(%d) Completed, MissionIndex(%d), ActionIndex(%d)\n", missionID, actionID, mapMissionIter->second, mapActionIter->second );
	gfxHUD.Invoke("_root.api.setMissionObjectiveCompleted", var, 2);
}


#ifdef MISSION_TRIGGERS
void HUDDisplay::eventMissionAccepted(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3dMouse::Hide();
	writeNoteSavedSlotIDFrom = 0;
	isReadMissionVisible = false;

	r3d_assert(argCount > 0 && "Missing the accepted missionID.");

	PKT_C2S_AcceptMission_s n;
	n.missionID = args[0].GetUInt();
	p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
}
void HUDDisplay::eventMissionClosed(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3dMouse::Hide();
	writeNoteSavedSlotIDFrom = 0;
	isReadMissionVisible = false;
}

void HUDDisplay::showMission(uint32_t missionID, const char* name, const char* desc)
{
	if(!Inited) return;
	if(!r_render_in_game_HUD->GetBool())
		return;
	if(isReadMissionVisible)
		return;

	r3dMouse::Show();
	writeNoteSavedSlotIDFrom = 1; // temp, to prevent mouse from hiding
	Scaleform::GFx::Value var[5];
	var[0].SetBoolean(true);
	var[1].SetString(desc);
	var[2].SetString(name);
	var[3].SetString("");
	var[4].SetUInt( missionID );
	gfxHUD.Invoke("_root.api.showMission", var, 5);

	isReadMissionVisible = true;
}

void HUDDisplay::hideMission()
{
	if(!Inited) return;
	if(isReadMissionVisible)
		return;

	r3dMouse::Hide();

	Scaleform::GFx::Value var[5];
	var[0].SetBoolean(false);
	var[1].SetString("");
	var[2].SetString("");
	var[3].SetString("");
	var[5].SetUInt( 0 );
	gfxHUD.Invoke("_root.api.showMission", var, 5);

	isReadMissionVisible = false;
	writeNoteSavedSlotIDFrom = 0;
}
#endif
#endif

void HUDDisplay::eventSafelockPass(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==1);
	r3dMouse::Hide();
	isSafelockPinVisible = false;

	isSafeLockInputEnabled = 0;
	gfxHUD.Invoke("_root.api.enableSafelockInput", false);

	if(isSafelockPinResetOp)
	{
		isSafelockPinResetOp = false;
		PKT_C2S_LockboxKeyReset_s n;
		r3dscpy(n.old_AccessCodeS, hudSafelock->CurrentAccessCodeString);
		r3dscpy(n.new_AccessCodeS, args[0].GetString());
		n.lockboxID  = currentSafelockNetID;
		p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
	}
	else
	{
		PKT_C2S_LockboxOpAns_s n;
		r3dscpy(n.AccessCodeS, args[0].GetString());
		r3dscpy(hudSafelock->CurrentAccessCodeString, args[0].GetString());
		n.lockboxID  = currentSafelockNetID;
		p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n));
	}
}

void HUDDisplay::showSafelockPin(bool isUnlock, int lockboxID, bool isDoingLockboxReset)
{
	if(!Inited) return;
	if(!r_render_in_game_HUD->GetBool())
		return;
	if(isSafelockPinVisible)
		return;

	r3dMouse::Show();
	Scaleform::GFx::Value var[2];
	var[0].SetBoolean(isUnlock);
	var[1].SetBoolean(true);
	gfxHUD.Invoke("_root.api.showSafelock", var, 2);
	isSafelockPinVisible = true;
	currentSafelockNetID = lockboxID;
	isSafelockPinResetOp = false;

	if(!isUnlock && isDoingLockboxReset)
		isSafelockPinResetOp = true;
	
}

void HUDDisplay::ConstructionMode(bool enable)
{
	if(!Inited) return;
	gfxHUD.SetVariable("_root.api.Main.CaliTestText.visible", enable);
}

void HUDDisplay::hideSafelockPin()
{
	if(!Inited) return;
	if(!isSafelockPinVisible) return;

	r3dMouse::Hide();

	Scaleform::GFx::Value var[2];
	var[0].SetBoolean(false);
	var[1].SetBoolean(false);
	gfxHUD.Invoke("_root.api.showSafelock", var, 2);

	isSafelockPinVisible = false;
}

static const int HUDDisplay_ContextMenu_LeaveGroupID = 10;
static const int HUDDisplay_ContextMenu_InviteToGroupID = 11;
static const int HUDDisplay_ContextMenu_AcceptGroupInviteID = 12;
static const int HUDDisplay_ContextMenu_KickFromGroupID = 13;
static const int HUDDisplay_ContextMenu_VOIP_MuteID=20;
static const int HUDDisplay_ContextMenu_VOIP_UnmuteID=21;
static const int HUDDisplay_ContextMenu_ReportPlayerID = 50;
static const int HUDDisplay_ContextMenu_AdminKickPlayerID = 51;
static const int HUDDisplay_ContextMenu_AdminBanPlayerID = 52;
static const int HUDDisplay_ContextMenu_AdminTeleportToPlayerID = 53;
static const int HUDDisplay_ContextMenu_AdminTeleportPlayerToAdminID = 54;
static const int HUDDisplay_ContextMenu_AdminSpecialReportPlayerID = 55;

void HUDDisplay::eventShowPlayerListContextMenu(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount==1);
	const char* gamertag = args[0].GetString();

	// todo: depending on who we selected, show different context menu
	struct ContextCommand
	{
		int cmdID;
		const char* cmdTxt;
		ContextCommand() : cmdID(0), cmdTxt("") {}
		ContextCommand(int id, const char* txt) : cmdID(id), cmdTxt(txt) {}
	};
	ContextCommand cntxCmds[10];
	int curCntxCmd=0;

	const ClientGameLogic& CGL = gClientLogic();

	bool selectedSelf = false;
	{
		char tmpStr[128] = {0};
		CGL.localPlayer_->GetUserName(tmpStr);
		if(stricmp(tmpStr, gamertag)==0)
			selectedSelf = true;
	}

	const ClientGameLogic::PlayerName_s* plrNamePtr = NULL;
	for(int i=0; i<R3D_ARRAYSIZE(CGL.playerNames); ++i)
	{
		if(CGL.playerNames[i].Gamertag[0] && stricmp(CGL.playerNames[i].Gamertag, gamertag)==0)
		{
			plrNamePtr = &CGL.playerNames[i];
			break;
		}
	}

	if(plrNamePtr) // it can be null if player left the game while localplayer kept player list open
	{
		// group logic
		if(selectedSelf && localPlayer_inGroup)
		{
			cntxCmds[curCntxCmd++] = ContextCommand(HUDDisplay_ContextMenu_LeaveGroupID, "$HUD_PlayerAction_LeaveGroup");
		}
		else if(!selectedSelf)
		{
			if(((localPlayer_inGroup && localPlayer_groupLeader) || !localPlayer_inGroup) && !(plrNamePtr->groupFlags&2) && !(plrNamePtr->groupFlags&1) )
				cntxCmds[curCntxCmd++] = ContextCommand(HUDDisplay_ContextMenu_InviteToGroupID, "$HUD_PlayerAction_InviteGroup");
			else if(!localPlayer_inGroup && (plrNamePtr->groupFlags&1))
				cntxCmds[curCntxCmd++] = ContextCommand(HUDDisplay_ContextMenu_AcceptGroupInviteID, "$HUD_PlayerAction_AcceptGroupInvite");
			else if(localPlayer_inGroup && localPlayer_groupLeader && (plrNamePtr->groupFlags&2))
				cntxCmds[curCntxCmd++] = ContextCommand(HUDDisplay_ContextMenu_KickFromGroupID, "$HUD_PlayerAction_KickFromGroup");

			{
				if(!plrNamePtr->voiceIsMuted)
					cntxCmds[curCntxCmd++] = ContextCommand(HUDDisplay_ContextMenu_VOIP_MuteID, "$HUD_PlayerAction_Mute");
				else
					cntxCmds[curCntxCmd++] = ContextCommand(HUDDisplay_ContextMenu_VOIP_UnmuteID, "$HUD_PlayerAction_Unmute");
			}
		}

		
		bool isAdmin = gUserProfile.ProfileData.isDevAccount > 0;

		if(!isAdmin)
		{
			cntxCmds[curCntxCmd++] = ContextCommand(HUDDisplay_ContextMenu_ReportPlayerID, "$HUD_PlayerAction_Report");
		}
		else {
			if(gUserProfile.ProfileData.isDevAccount & wiUserProfile::DAA_DEV_ICON && strcmp(plrNamePtr->Gamertag,CGL.localPlayer_->CurLoadout.Gamertag) != 0)
			cntxCmds[curCntxCmd++] = ContextCommand(HUDDisplay_ContextMenu_AdminSpecialReportPlayerID, "$HUD_PlayerAction_DevReport");
		}

		if(isAdmin)
		{
			if(gUserProfile.ProfileData.isDevAccount & wiUserProfile::DAA_KICK)
				cntxCmds[curCntxCmd++] = ContextCommand(HUDDisplay_ContextMenu_AdminKickPlayerID, "$HUD_PlayerAction_Kick");
			if(gUserProfile.ProfileData.isDevAccount & wiUserProfile::DAA_BAN)
				cntxCmds[curCntxCmd++] = ContextCommand(HUDDisplay_ContextMenu_AdminBanPlayerID, "$HUD_PlayerAction_Ban");
			if(gUserProfile.ProfileData.isDevAccount & wiUserProfile::DAA_TELEPORT)
				cntxCmds[curCntxCmd++] = ContextCommand(HUDDisplay_ContextMenu_AdminTeleportToPlayerID, "TELEPORT TO");
			if(gUserProfile.ProfileData.isDevAccount & wiUserProfile::DAA_TELEPORT)
				cntxCmds[curCntxCmd++] = ContextCommand(HUDDisplay_ContextMenu_AdminTeleportPlayerToAdminID, "TELEPORT TO YOU");
		}
		else
		{
			if(gUserProfile.CustomerID == CGL.m_gameCreatorCustomerID)
				cntxCmds[curCntxCmd++] = ContextCommand(HUDDisplay_ContextMenu_AdminKickPlayerID, "$HUD_PlayerAction_Kick");
		}
	}

	r3d_assert(curCntxCmd <=7); // everything after 6 IS DEBUG ONLY FOR DEVS!!!!

	Scaleform::GFx::Value var[3];
	for(int i=0; i<7; ++i)
	{
		var[0].SetInt(i+1);
		var[1].SetString(cntxCmds[i].cmdTxt);
		var[2].SetInt(cntxCmds[i].cmdID);
		gfxHUD.Invoke("_root.api.setPlayerListContextMenuButton", var, 3);
	}

	gfxHUD.Invoke("_root.api.showPlayerListContextMenu", "");
}

void HUDDisplay::eventPlayerListAction(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount)
{
	r3d_assert(argCount == 2);
	int actionID = args[0].GetInt();
	const char* gamertag = args[1].GetString();

	ClientGameLogic& CGL = gClientLogic();
	ClientGameLogic::PlayerName_s* plrNamePtr = NULL;
	for(int i=0; i<R3D_ARRAYSIZE(CGL.playerNames); ++i)
	{
		if(CGL.playerNames[i].Gamertag[0] && stricmp(CGL.playerNames[i].Gamertag, gamertag)==0)
		{
			plrNamePtr = &CGL.playerNames[i];
			break;
		}
	}

	showPlayersList(0);
	
	if(actionID == HUDDisplay_ContextMenu_ReportPlayerID) 
	{
		char tmpStr[256];
		sprintf(tmpStr, "/report \"%s\" ", gamertag);
		showChatInput(tmpStr);
	}
	else if(actionID == HUDDisplay_ContextMenu_AdminSpecialReportPlayerID)
	{
		char tmpStr[256];
		sprintf(tmpStr, "/ssp \"%s\" ", gamertag);
		showChatInput(tmpStr);
	}
	else if(actionID == HUDDisplay_ContextMenu_AdminKickPlayerID)
	{
		char tmpStr[256];
		sprintf(tmpStr, "/kick \"%s\" reason:", gamertag);
		showChatInput(tmpStr);
	}
	else if(actionID == HUDDisplay_ContextMenu_AdminBanPlayerID)
	{
		char tmpStr[256];
		sprintf(tmpStr, "/ban \"%s\" reason:", gamertag);
		showChatInput(tmpStr);
	}
	else if(actionID == HUDDisplay_ContextMenu_AdminTeleportToPlayerID)
	{
		char tmpStr[256];
		sprintf(tmpStr, "/ttp \"%s\"", gamertag);
		showChatInput(tmpStr);
	}
	else if(actionID == HUDDisplay_ContextMenu_AdminTeleportPlayerToAdminID)
	{
		char tmpStr[256];
		sprintf(tmpStr, "/ttyl \"%s\"", gamertag);
		showChatInput(tmpStr);
	}
	else if(actionID == HUDDisplay_ContextMenu_LeaveGroupID)
	{
		PKT_C2S_GroupLeaveGroup_s n;
		p2pSendToHost(CGL.localPlayer_, &n, sizeof(n));
	}
	else if(actionID == HUDDisplay_ContextMenu_InviteToGroupID)
	{
		PKT_C2S_GroupInvitePlayer_s n;
		r3dscpy(n.gamertag, gamertag);
		p2pSendToHost(CGL.localPlayer_, &n, sizeof(n));
	}
	else if(actionID == HUDDisplay_ContextMenu_AcceptGroupInviteID)
	{
		PKT_C2S_GroupAcceptInvite_s n;
		r3dscpy(n.gamertag, gamertag);
		p2pSendToHost(CGL.localPlayer_, &n, sizeof(n));
	}
	else if(actionID == HUDDisplay_ContextMenu_KickFromGroupID)
	{
		PKT_C2S_GroupKickPlayer_s n;
		r3dscpy(n.gamertag, gamertag);
		p2pSendToHost(CGL.localPlayer_, &n, sizeof(n));
	}
	else if(actionID == HUDDisplay_ContextMenu_VOIP_MuteID)//Cynthia
	{
		//if(plrNamePtr->voiceClientID > 0 && plrNamePtr->voiceEnabled)
		if(plrNamePtr->voiceClientID > 0)
		{
			gTeamSpeakClient.MuteClient(plrNamePtr->voiceClientID);
			plrNamePtr->voiceIsMuted = 1;
		}
	}
	else if(actionID == HUDDisplay_ContextMenu_VOIP_UnmuteID)
	{
		//if(plrNamePtr->voiceClientID > 0 && plrNamePtr->voiceEnabled)
		if(plrNamePtr->voiceClientID > 0)
		{
			gTeamSpeakClient.UnmuteClient(plrNamePtr->voiceClientID);
			plrNamePtr->voiceIsMuted = 0;
		}
	}
}

void HUDDisplay::showYouAreDead(const char* killedBy)
{
	if(!Inited) return;

	SendDisconnectRequest = false;

#ifdef VEHICLES_ENABLED
	isShowingYouAreDead = true;
#endif

	gfxHUD.SetVariable("_root.Main.PlayerDead.DeadMsg.Text2.text", killedBy);
	ShowMsgDeath("in");

	//showMessage("BLA1 BLA2 BLA3 BLA4 BLA5 BLA6 BLA");
}
#ifdef VEHICLES_ENABLED
bool HUDDisplay::isYouAreDead()
{
	return isShowingYouAreDead;
}
#endif

void HUDDisplay::updateDeadTimer(int timer)
{
	if(!Inited) return;

	char tmpstr[64];
	if (SendDisconnectRequest == false)
		sprintf(tmpstr, gLangMngr.getString("$HUD_ExitingIn"), timer);
	else
		strcpy(tmpstr, gLangMngr.getString("$HUD_respawnfast"));
	gfxHUD.SetVariable("_root.Main.PlayerDead.DeadMsg.BtnRespawnFast.Text.Text.text", tmpstr);

	if (gClientLogic().localPlayer_->CurLoadout.Hardcore == 1)
	{
		r3dMouse::Hide();
		sprintf(tmpstr, gLangMngr.getString("$HUD_ExitingIn2"), timer);
		gfxHUD.SetVariable("_root.Main.PlayerDead.DeadMsg.BtnExit.Text.Text.text", tmpstr);
		gfxHUD.SetVariable("_root.Main.PlayerDead.DeadMsg.BtnRespawnFast.visible", false);
		gfxHUD.SetVariable("_root.Main.PlayerDead.DeadMsg.Checkbox1a.visible", false);
		gfxHUD.SetVariable("_root.Main.PlayerDead.DeadMsg.Checkbox1b.visible", false);
		gfxHUD.SetVariable("_root.Main.PlayerDead.DeadMsg.Checkbox2a.visible", false);
		gfxHUD.SetVariable("_root.Main.PlayerDead.DeadMsg.Checkbox2b.visible", false);
		gfxHUD.SetVariable("_root.Main.PlayerDead.DeadMsg.Checkbox3a.visible", false);
		gfxHUD.SetVariable("_root.Main.PlayerDead.DeadMsg.Checkbox3b.visible", false);
		gfxHUD.SetVariable("_root.Main.PlayerDead.DeadMsg.RsSText.visible", false);
		gfxHUD.SetVariable("_root.Main.PlayerDead.DeadMsg.RsPText.visible", false);
		gfxHUD.SetVariable("_root.Main.PlayerDead.DeadMsg.RsGText.visible", false);
	}
	else {
		r3dMouse::Show();
	}

	if(timer < 1 && !SendDisconnectRequest)
	{
		SendDisconnectRequest = true;

		if (gClientLogic().localPlayer_->CurLoadout.Hardcore == 1)
		{
			PKT_C2S_DisconnectReq_s n;
			p2pSendToHost(gClientLogic().localPlayer_, &n, sizeof(n), true);
		}
	}
}

void HUDDisplay::showSafeZoneWarning(bool flag)
{
	if(!Inited) return;

	if(SafeZoneWarningVisible != flag)
	{
		SafeZoneWarningVisible = flag;
		gfxHUD.Invoke("_root.Main.Condition.gotoAndStop", flag?"safe":"regular");
	}
}

const char* getReputationIconName(int reputation);
void HUDDisplay::addCharTag(const char* name, bool isSameClan, int rep, Scaleform::GFx::Value& result)
{
	if(!Inited) return;
	r3d_assert(result.IsUndefined());

	const char* repS = getReputationIconName(rep);

	Scaleform::GFx::Value var[3];
	var[0].SetString(name);
	var[1].SetBoolean(isSameClan);
	var[2].SetString(repS);
	gfxHUD.Invoke("_root.api.addCharTag", &result, var, 3);
}

void HUDDisplay::removeUserIcon(Scaleform::GFx::Value& icon)
{
	if(!Inited) return;
	r3d_assert(!icon.IsUndefined());

	Scaleform::GFx::Value var[1];
	var[0] = icon;
	gfxHUD.Invoke("_root.api.removeUserIcon", var, 1);

	icon.SetUndefined();
}

// optimized version
void HUDDisplay::moveUserIcon(Scaleform::GFx::Value& icon, const r3dPoint3D& pos, bool alwaysShow, bool force_invisible /* = false */, bool pos_in_screen_space/* =false */)
{
	if(!Inited)
		return;
	r3d_assert(!icon.IsUndefined());

	r3dPoint3D scrCoord;
	float x, y;
	int isVisible = 1;
	if(!pos_in_screen_space)
	{
		if(alwaysShow)
			isVisible = r3dProjectToScreenAlways(pos, &scrCoord, 20, 20);
		else
			isVisible = r3dProjectToScreen(pos, &scrCoord);
	}
	else
		scrCoord = pos;

	// convert screens into UI space
	float mulX = 1920.0f/r3dRenderer->ScreenW;
	float mulY = 1080.0f/r3dRenderer->ScreenH;
	x = scrCoord.x * mulX;
	y = scrCoord.y * mulY;

	Scaleform::GFx::Value::DisplayInfo displayInfo;
	icon.GetDisplayInfo(&displayInfo);
	displayInfo.SetVisible(isVisible && !force_invisible);
	displayInfo.SetX(x);
	displayInfo.SetY(y);
	icon.SetDisplayInfo(displayInfo);
}

void HUDDisplay::setCharTagTextVisible(Scaleform::GFx::Value& icon, bool isVisible, bool isSameGroup, bool isVoipTalking)
{
	if(!Inited) return;
	r3d_assert(!icon.IsUndefined());

	Scaleform::GFx::Value var[4];
	var[0] = icon;
	var[1].SetBoolean(isVisible);
	var[2].SetBoolean(isSameGroup);
	var[3].SetBoolean(isVoipTalking);
	gfxHUD.Invoke("_root.api.setCharTagTextVisible", var, 4);
}

void HUDDisplay::setTPSReticleVisibility(int set)
{
	if(!Inited) return;

	if(!(gClientLogic().m_gameInfo.flags & GBGameInfo::SFLAGS_CrossHair))
		set = 0;

	if(set == TPSReticleVisible) return;

	TPSReticleVisible = set;
	gfxHUD.SetVariable("_root.Main.reticle.visible", set);
}

void HUDDisplay::addPlayerToGroupList(const char* name, bool isLeader, bool isLeaving)
{
	if(!Inited) return;
	Scaleform::GFx::Value var[3];

	char tmpStr[128] = {0};
	const ClientGameLogic& CGL = gClientLogic();
	CGL.localPlayer_->GetUserName(tmpStr);
	if(stricmp(tmpStr, name)==0)
	{
		localPlayer_groupLeader = isLeader;
		localPlayer_inGroup = true;
	}

	var[0].SetString(name);
	var[1].SetBoolean(isLeader);
	var[2].SetBoolean(isLeaving);
	gfxHUD.Invoke("_root.api.addPlayerToGroup", var, 3);
}

void HUDDisplay::removePlayerFromGroupList(const char* name)
{
	if(!Inited) return;

	char tmpStr[128] = {0};
	const ClientGameLogic& CGL = gClientLogic();
	CGL.localPlayer_->GetUserName(tmpStr);
	if(stricmp(tmpStr, name)==0)
	{
		localPlayer_groupLeader = false;
		localPlayer_inGroup = false;
	}

	gfxHUD.Invoke("_root.api.removePlayerFromGroup", name);
}

void HUDDisplay::aboutToLeavePlayerFromGroup(const char* name)
{
	if(!Inited) return;

	gfxHUD.Invoke("_root.api.aboutToLeavePlayerFromGroup", name);
}

void HUDDisplay::addPlayerToVoipList(const char* name)
{
	if(!Inited) return;
	gfxHUD.Invoke("_root.api.addPlayerToVoipList", name);
}

void HUDDisplay::removePlayerFromVoipList(const char* name)
{
	if(!Inited) return;
	gfxHUD.Invoke("_root.api.removePlayerFromVoipList", name);
}


void HUDDisplay::setCarInfo(int durability, int speed, int speedText, int gas, int rpm)
{
	if(!Inited) return;
	Scaleform::GFx::Value var[5];
	var[0].SetInt(durability);
	var[1].SetInt(speed);
	var[2].SetInt(speedText);
	var[3].SetInt(gas);
	var[4].SetInt(rpm);
	gfxHUD.Invoke("_root.api.setCarInfo", var, 5);
}

void HUDDisplay::showCarInfo(bool visible)
{
	if(!Inited) return;
	gfxHUD.Invoke("_root.api.setCarInfoVisibility", visible);
}

void HUDDisplay::setCarTypeInfo(const char* type)
{
	if(!Inited) return;
	Scaleform::GFx::Value var[1];
	var[0].SetString(type);
	gfxHUD.Invoke("_root.api.setCarTypeInfo", var, 1);
}

void HUDDisplay::setCarSeatInfo(int seatID, const char* type)
{
	if(!Inited) return;
	Scaleform::GFx::Value var[2];
	var[0].SetInt(seatID);
	var[1].SetString(type);
	gfxHUD.Invoke("_root.api.setCarSeatInfo", var, 2);
}
