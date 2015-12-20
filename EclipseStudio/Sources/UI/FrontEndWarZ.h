#pragma once

#include "UIMenu.h"
#include "../../ServerNetPackets/NetPacketsGameBrowser.h"

#include "CkHttp.h"

#include "FrontEndShared.h"
#include "GameCode\UserProfile.h"
#include "GameCode\UserClans.h"
#include "GameCode\UserServerRental.h"

class FrontendWarZ : public UIMenu, public r3dIResource
{
public:
	virtual bool 	Unload();
private:
	Scaleform::Render::D3D9::Texture* RTScaleformTexture;
	bool		needReInitScaleformTexture;
	int			frontendStage; // 0 - login, 1-frontend

	EGameResult	prevGameResult;

	void drawPlayer();

//	void	eventFriendPopupClosed(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	
public:
	FrontendWarZ(const char * movieName);
	virtual ~FrontendWarZ();

	virtual bool Initialize();

	virtual int Update();

	void postLoginStepInit(EGameResult gameResult);
	void initLoginStep(const char* loginErrorMsg);

	//	r3dIResource, callbacks
	//	Need to rebind texture to scaleform after device lost
	virtual	void D3DCreateResource();
	virtual	void D3DReleaseResource(){};

	void bindRTsToScaleForm();

	void SetLoadedThings(obj_Player* plr)
	{
		r3d_assert(m_Player == NULL);
		m_Player = plr;
	}

	// login part of frontend
private:
	friend void FrontendWarZ_LoginProcessThread(void* in_data);
	friend class callbackEnterPassword;
	static unsigned int WINAPI LoginProcessThread(void* in_data);
	static unsigned int WINAPI LoginAuthThread(void* in_data);
	HANDLE	loginThread;
	enum {
		ANS_Unactive,
		ANS_Processing,
		ANS_Timeout,
		ANS_Error,

		ANS_Logged,
		ANS_BadPassword,
		ANS_Frozen,
	};
	volatile DWORD loginAnswerCode;
	bool	DecodeAuthParams();
	void	LoginCheckAnswerCode();
	float	loginProcessStartTime;
	bool	loginMsgBoxOK_Exit;

private:
	void eventPlayGame(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventCancelQuickGameSearch(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventQuitGame(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventCreateCharacter(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventRenameCharacter(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventDeleteChar(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventShowSurvivorsMap(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventReviveChar(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventReviveCharMoney(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventBuyItem(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventBackpackFromInventory(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventBackpackToInventory(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventBackpackGridSwap(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventSetSelectedChar(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventOptionsReset(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventOptionsApply(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventOptionsLanguageSelection(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventOptionsControlsRequestKeyRemap(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventOptionsControlsReset(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventOptionsControlsApply(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventOptionsVoipApply(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventOptionsVoipReset(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventCreateChangeCharacter(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventCreateCancel(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventRequestPlayerRender(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventMsgBoxCallback(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventOpenBackpackSelector(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventChangeBackpack(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventLearnSkill(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventChangeOutfit(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventSetCurrentBrowseChannel(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventTrialRequestUpgrade(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventTrialUpgradeAccount(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventBuyPremiumAccount(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventMarketplaceActive(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventOpenURL(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);

	void eventBrowseGamesRequestFilterStatus(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventBrowseGamesSetFilter(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventBrowseGamesJoin(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventBrowseGamesOnAddToFavorites(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventBrowseGamesRequestList(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);

	void eventRequestMyClanInfo(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventRequestClanList(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventCreateClan(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventClanAdminDonateGC(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventClanAdminAction(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventClanLeaveClan(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventClanDonateGCToClan(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventRequestClanApplications(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventClanApplicationAction(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventClanInviteToClan(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventClanRespondToInvite(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventClanBuySlots(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventClanApplyToJoin(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);

	void eventRequestGCTransactionData(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);

	void eventStorePurchaseGPCallback(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventStorePurchaseGP(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventStorePurchaseGPRequest(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);

	void eventStorePurchaseGDCallback(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventStorePurchaseGD(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	int	m_buyGDForGC;
	static	unsigned int WINAPI as_BuyGDForGCThread(void* in_data);
	void		OnBuyGDForGCSuccess();

	void eventDonateGCtoServerCallback(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventDonateGCtoServer(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventRequestShowDonateGCtoServer(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);


	void eventRequestLeaderboardData(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);

	void eventRequestMyServerList(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventRequestMyServerInfo(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventMyServerKickPlayer(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventRentServerUpdatePrice(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventRentServer(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventMyServerJoinServer(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventMyServerUpdateSettings(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventRenewServerUpdatePrice(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);
	void eventRenewServer(r3dScaleformMovie* pMovie, const Scaleform::GFx::Value* args, unsigned argCount);

	static unsigned int WINAPI as_RequestClanListThread(void* in_data);
	void		OnRequestClanListSuccess();
	int			clanListRequest_SortID;
	int			clanListRequest_StartPos;
	int			clanListRequest_ClanListSize;

	static unsigned int WINAPI as_RequestGCTransactionThread(void* in_data);
	void		OnRequestGCTransactionSuccess();

	static unsigned int WINAPI as_RequestLeaderboardThread(void* in_data);
	void		OnRequestLeaderboardSuccess();
	int			leaderboardSize;
	int			leaderboard_requestTableID;
	int			leaderboard_requestStartPos;
	int			leaderboard_BoardSelected;

	char		CharRename_PreviousName[128];

	void checkForInviteFromClan();
	void setClanInfo();
	CUserClans::CreateParams_s clanCreateParams;
	r3dSTLString					   clanCreateParam_Desc;
	static unsigned int WINAPI as_CreateClanThread(void* in_data);
	void		OnCreateClanSuccess();
	void		refreshClanUIMemberList();
	typedef r3dgameList(CUserClans::ClanInfo_s) ClanList;
	ClanList cachedClanList;
	bool processClanError(int api);

	static unsigned int WINAPI as_BrowseGamesThread(void* in_data);
	void		OnGameListReceived();
	void		processNewGameList();

	void		ProcessSupervisorPings();
	int		 GetSupervisorPing(DWORD ip);

	void initFrontend();
	void initVoipOptions();
	void initItems();
	void updateInventoryAndSkillItems();
	void addStore();

	void addClientSurvivor(const wiCharDataFull& slot, int slotIndex);
	void addBackpackItems(const wiCharDataFull& slot, int slotIndex);
	void updateClientSurvivor(const wiCharDataFull& slot);

	bool		exitRequested_;
	bool		needExitByGameJoin_;
	bool		needReturnFromQuickJoin;

	//
	// Async Function Calls
	//
	typedef void (FrontendWarZ::*fn_finish)();
	typedef unsigned int (WINAPI *fn_thread)(void*);

	float		lastServerReqTime_;
	void		DelayServerRequest();
	float		masterConnectTime_;
	bool		ConnectToMasterServer();
	bool		WaitForGameJoinAnswer();
	bool		ParseGameJoinAnswer();
	volatile bool CancelQuickJoinRequest;

	uint32_t		CurrentBrowseChannel;

	fn_finish	asyncFinish_;
	HANDLE		asyncThread_;
	char		asyncErr_[512];

	void		StartAsyncOperation(fn_thread threadFn, fn_finish finishFn = NULL);
	void		SetAsyncError(int apiCode, const char* msg);
	void		ProcessAsyncOperation();

	static unsigned int WINAPI as_CreateCharThread(void* in_data);
	void		OnCreateCharSuccess();
	static unsigned int WINAPI as_DeleteCharThread(void* in_data);
	void		OnDeleteCharSuccess();
	static unsigned int WINAPI as_BuyItemThread(void* in_data);
	void		OnBuyItemSuccess();
	void		OnBuyPremiumSuccess();
	static	unsigned int WINAPI as_BackpackFromInventoryThread(void* in_data);
	static	unsigned int WINAPI as_BackpackFromInventorySwapThread(void* in_data);
	void		OnBackpackFromInventorySuccess();
	static	unsigned int WINAPI as_ReviveCharThread(void* in_data);
	static	unsigned int WINAPI as_ReviveCharThread2(void* in_data);
	int			ReviveCharSuccessfull; // 0 -failed, 1-success, 2-show money needed
	void		OnReviveCharSuccess();
	static	unsigned int WINAPI as_BackpackToInventoryThread(void* in_data);
	void		OnBackpackToInventorySuccess();
	static	unsigned int WINAPI as_BackpackGridSwapThread(void* in_data);
	void		OnBackpackGridSwapSuccess();
	static	unsigned int WINAPI as_BackpackChangeThread(void* in_data);
	void		OnBackpackChangeSuccess();
	
	CUserServerRental::rentParams_s m_rentServerParams;
	int	m_rentServerPrice;
	static	unsigned int WINAPI as_RentServerThread(void* in_data);
	void		OnRentServerSuccess();
	static	unsigned int WINAPI as_RenewServerThread(void* in_data);
	void		OnRenewServerSuccess();
	static	unsigned int WINAPI as_DonateToServerThread(void* in_data);
	void		OnDonateToServerSuccess();

	static	unsigned int WINAPI as_MyServerListThread(void* in_data);
	void		OnMyServerListSuccess();

	DWORD	m_myGameServerId;
	DWORD	m_myGameServerAdminKey;
	static	unsigned int WINAPI as_MyServerInfoThread(void* in_data);
	void		OnMyServerInfoSuccess();

	char	m_myServerPwd[16];
	int	m_myServerFlags;
	uint32_t	 m_myServerGameTimeLimit;
	static	unsigned int WINAPI as_MyServerSetSettingsThread(void* in_data);
	void		OnMyServerSetSettingsSuccess();

	DWORD	m_myKickCharID;
	static	unsigned int WINAPI as_MyServerKickPlayerThread(void* in_data);
	void		OnMyServerKickPlayerSuccess();
	
	int	m_buyGpPriceCents;
	static	unsigned int WINAPI as_SteamBuyGPThread(void* in_data);
	void		OnSteamBuyGPSuccess();
	

	uint32_t	 m_SkillID;
	static	unsigned int WINAPI as_LearnSkillThread(void* in_data);
	void		OnLearnSkillSuccess();

	uint32_t	m_ChangeOutfit_newHead;
	uint32_t	m_ChangeOutfit_newBody;
	uint32_t	m_ChangeOutfit_newLegs;
	static	unsigned int WINAPI as_ChangeOutfitThread(void* in_data);
	void		OnChangeOutfitSuccess();

	static	unsigned int WINAPI as_CharRenameThread(void* in_data);
	void		OnCharRenameSuccess();

	static unsigned int WINAPI as_PlayGameThread(void* in_data);
	static unsigned int WINAPI as_JoinGameThread(void* in_data);

	bool	UpdateInventoryWithBoughtItem();
	int		StoreDetectBuyIdx();
	void	SyncGraphicsUI();
	void	AddSettingsChangeFlag( DWORD flag );
	void	SetNeedUpdateSettings();
	void	UpdateSettings();
	void	updateSurvivorTotalWeight(int survivor);

// char create
	uint32_t		m_itemID;

	uint32_t		mStore_BuyItemID;
	int		mStore_BuyPrice;
	int		mStore_BuyPriceGD;
	__int64	m_inventoryID;
	int		m_gridTo;
	int		m_gridFrom;
	int		m_Amount;
	int		m_Amount2;
	__int64	mChangeBackpack_inventoryID;

	wiCharDataFull	PlayerCreateLoadout; // save loadout when creating player, so that user can see rendered player
	uint32_t		m_CreateHeroID;
	int		m_CreateBodyIdx;
	int		m_CreateHeadIdx;
	int		m_CreateLegsIdx;
	char		m_CreateGamerTag[64];
	int		m_CreateGameMode;
	
	DWORD		m_joinGameServerId;
	char		m_joinGamePwd[32];

	bool	m_ReloadProfile;
	float	m_ReloadTimer;

	DWORD	settingsChangeFlags_;
	bool	needUpdateSettings_;
	int		m_waitingForKeyRemap;

	class obj_Player* m_Player;
	int m_needPlayerRenderingRequest;

	int m_browseGamesMode; // 0 - browse, 1-recent, 2-favorites
	char m_browseGamesNameFilter[64];
	int m_browseGamesSortMode; // 0-name, 1-map, 2-mode, 3-ping
	int m_browseGamesSortOrder;
	int m_browseGamesCurrentCur;
	int m_browseGamesRequestedOper;
};
