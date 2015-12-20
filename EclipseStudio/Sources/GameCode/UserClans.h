#pragma once

#include "UserProfile.h"

class CWOBackendReq;

class CUserClans
{
  public:
	CRITICAL_SECTION csClans_;
	
	enum EClanResultCodes
	{
/*	
	20 - not enough slots in clan	
	21 - already joined clan
	22 - no gamertag
	23 - no permission
	24 - already have pending application to specified clan
	25 - have maximum pending applications
	27 - clan name already exist
	28 - clan tag already exist
*/
	};
	static int	GetResultCode2(const char* apiName, CWOBackendReq& req);
	
	enum EClanEvents
	{
	  CLANEvent_Created = 1,

	  CLANEvent_SetRank = 3,
		//Var1 - Who
		//Var2 - member
		//Var3 - rank
		//Text1 - gamertag
		//Text2 - gamertag
   
	  CLANEvent_Joined = 4,
		//Var1 - Who
		//Text1 - gamertag

	  CLANEvent_Left   = 5,
		//Var1 - who
		//Text1 - gamertag

	  CLANEvent_Kick   = 6,
		//Var1 - Who
		//Var2 - member
		//Text1 - gamertag
		//Text2 - gamertag

	  CLANEvent_DonateToClanGP = 10,
		//Var1 - who
		//Var3 - amount
		//Text1 - gamertag

	  CLANEvent_DonateToMemberGP = 11,
		//Var1 - who
		//Var2 - member
		//Var3 - amount
		//Text1 - gamertag
		//Text2 - gamertag

	  CLANEvent_LevelUp = 12,
		//Var1 - level
		
	  ClanEvent_AddMaxMembers = 13,
		//Var1 - who
		//Var2 - added members
		//Text1 - gamertag
	};
	
	// clan log
	struct log_s
	{
	  time_t	EventDate;
	  int		EventType;	// of EClanEvents
	  
	  int		Var1;
	  int		Var2;
	  int		Var3;
	  int		Var4;
	  r3dSTLString	Text1;
	  r3dSTLString	Text2;
	  r3dSTLString	Text3;
	};
	r3dgameList(log_s) log_;
	
  public:
	CUserClans();
	~CUserClans();
	
	//
	// clan creation
	//
	struct CreateParams_s
	{
	  char		ClanName[64*2]; // utf8
	  int		ClanNameColor;
	  char		ClanTag[16]; // utf8
	  int		ClanTagColor;
	  int		ClanEmblemID;
	  int		ClanEmblemColor; // for denis: not needed
	  
	  CreateParams_s()
	  {
	    memset(this, 0, sizeof(*this));
	  }
	};
	int		ApiClanCheckIfCreateNeedMoney(int* out_NeedMoney);
	int		ApiClanCreate(const CreateParams_s& params);
	
	//
	// clan members
	//
	struct ClanMember_s
	{
	  int		CharID;
	  char		gamertag[64]; // utf8
	  int		ClanRank;
	  int		ContributedXP;
	  int		ContributedGP;
	  wiStats	stats;
	};
	typedef r3dgameList(ClanMember_s) TClanMemberList;
	static void	ParseClanMember(pugi::xml_node& xmlNode, ClanMember_s& member);
	
	//
	// clan information
	//
	struct ClanInfo_s
	{
	  int		ClanID;
	  
	  char		OwnerGamertag[32*2]; // utf8
	  char		ClanName[64*2]; // utf8
	  int		ClanNameColor;
	  char		ClanTag[5*2]; // utf8
	  int		ClanTagColor;
	  int		ClanEmblemID;
	  int		ClanEmblemColor;
	  char		ClanLore[512*2]; // utf8
	  
	  int		ClanXP;
	  int		ClanLevel;
	  int		ClanGP;		// clan GP reserve
	  int		NumClanMembers;
	  int		MaxClanMembers;
	  
	  ClanInfo_s()
	  {
	    memset(this, 0, sizeof(*this));
	  }
	};
	static void	ParseClanInfo(pugi::xml_node& xmlNode, ClanInfo_s& clan);

	// our current clan info and members
	ClanInfo_s	clanInfo_;
	TClanMemberList	clanMembers_;

	// retreive clan info. if out_members is set, then members structure will be filled as well
	static int	ApiClanGetInfo(int ClanID, ClanInfo_s* out_info, TClanMemberList* out_members);
	
	// leaderboards - all clan info
	r3dgameList(ClanInfo_s) leaderboard_;
	int		ApiClanGetLeaderboard();
	// sort types - 0: create date, 1: clan name, 2: clan members, 3: clan tag
	int		ApiClanGetLeaderboard2(int SortType, int StartPos, int* out_ClanListSize);
	
	void		RemoveMember(int MemberID);
	ClanMember_s*	GetMember(int MemberID);
	
	//
	// generic clan functions
	//
	int		ApiClanLeave();
	int		ApiClanKick(int MemberID);
	int		ApiClanSetRank(int MemberID, int Rank);
	int		ApiClanSetLore(const char* lore);
	int		ApiClanDonateGPToClan(int GP);
	int		ApiClanDonateGPToMember(int MemberID, int GP);
	int		ApiClanBuyAddMembers(int AddMemberIdx); // AddMemberIdx- index in gUserProfile.ShopClanAddMembers_ 
	
	//
	// clan->player invites
	//
	int		ApiClanSendInvite(const char* gamertag);
	int		ApiClanAnswerInvite(int ClanInviteID, int Accept);
	
	//
	// player->clan applications
	//
	int		ApiClanApplyToJoin(int ClanID, const char* note);
	int		ApiClanApplyAnswer(int ClanApplID, bool Accept);
	
	// retreive current clan status (in clanCurData_) and invites/applications lists
	int		ApiGetClanStatus();
	void		SetCurrentData(pugi::xml_node& xmlNode);

	// current clan data, should be used for join/leave/new members check
	struct ClanCurrentData_s
	{
	  int		ClanID;
	  int		ClanRank;
	  int		MaxClanMembers;
	  int		NumClanMembers;
	};
	ClanCurrentData_s clanCurData_;
	void		ParseClanCurrentData(pugi::xml_node& xmlNode);

	struct ClanApplication_s
	{
	  int		ClanApplID;
	  r3dSTLString	Gamertag;
	  wiStats	stats;
	  r3dSTLString	Note;
	};
	typedef r3dgameList(ClanApplication_s) ClanApplicationList;
	ClanApplicationList clanApplications_;
	void		ParseClanApplications(pugi::xml_node& xmlNode);

	struct ClanInvite_s
	{
	  int		ClanInviteID;
	  r3dSTLString	Gamertag;	// gamertag of who invited you
	  
	  r3dSTLString	ClanName;
	  int		ClanLevel; // FOR DENIS: change to how many members in clan, also add clan description
	  int		ClanEmblemID;
	  int		ClanEmblemColor;
	};
	r3dgameVector(ClanInvite_s) clanInvites_;
	void		ParseClanInvites(pugi::xml_node& xmlNode);
};

