#pragma once

// DO NOT CHANGE ID OF REWARD!!!
enum EPlayerRewardID
{
	RWD_ZombieKill		= 1,	//Standard
	RWD_SuperZombieKill = 2, 

	// Why the fuck this shit is here? :( It shouldn't be part of PlayerRewards to begin with. FUCK!
#ifdef MISSIONS
	RWD_ObjectiveComplete=510,
	RWD_MissionComplete=511,
#endif
		
	RWD_MAX_REWARD_ID	= 512,
};

class CGameRewards
{
  public:
	struct rwd_s
	{
	  bool		IsSet;
	  r3dSTLString	Name;
	
	  int		GD_SOFT;	// in softcore mode
	  int		XP_SOFT;
	  int		GD_HARD;	// in hardcore mode
	  int		XP_HARD;
	  
	  rwd_s()
	  {
		IsSet = false;
	  }
	};
	
	bool		loaded_;
	rwd_s		rewards_[RWD_MAX_REWARD_ID];

	void		InitDefaultRewards();
	void		  SetReward(int id, const char* name, int v1, int v2, int v3, int v4);
	void		ExportDefaultRewards();
	
  public:
	CGameRewards();
	~CGameRewards();
	
	int		ApiGetDataGameRewards();

	const rwd_s&	GetRewardData(int rewardID)
	{
		r3d_assert(rewardID >= 0 && rewardID < RWD_MAX_REWARD_ID);
		return rewards_[rewardID];
	}
};

extern CGameRewards*	g_GameRewards;
