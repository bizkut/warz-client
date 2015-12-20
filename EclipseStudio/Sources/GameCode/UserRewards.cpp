#include "r3dPCH.h"
#include "r3d.h"
#include "shellapi.h"

#include "UserRewards.h"
#include "backend/WOBackendAPI.h"

	CGameRewards*	g_GameRewards = NULL;

CGameRewards::CGameRewards()
{
	loaded_ = false;
	InitDefaultRewards();
}

CGameRewards::~CGameRewards()
{
}

void CGameRewards::SetReward(int id, const char* name, int v1, int v2, int v3, int v4)
{
	r3d_assert(id >= 0 && id < RWD_MAX_REWARD_ID);
	rwd_s& rwd  = rewards_[id];
	rwd.Name    = name;
	rwd.GD_SOFT = v1;
	rwd.XP_SOFT = v2;
	rwd.GD_HARD = v3;
	rwd.XP_HARD = v4;
	rwd.IsSet   = true;
}

void CGameRewards::InitDefaultRewards()
{
	// HERE is COPY-PASTED CODE FROM ExportDefaultRewards()
	// USED FOR SERVER

	SetReward(1, "ZombieKill", 1, 1, 2, 2);
	SetReward(2, "Super ZombieKill", 20, 20, 50, 50);
	SetReward(3, "PlayerKill", 0, 15, 0, 15);
}

void CGameRewards::ExportDefaultRewards()
{
	const char* fname = "@rewards.txt";
	FILE* f = fopen_for_write(fname, "wt");
	for(int i=0; i<RWD_MAX_REWARD_ID; i++)
	{
		const rwd_s& rwd = GetRewardData(i);
		if(!rwd.IsSet) continue;
		fprintf(f, "\tSetReward(%d, \"%s\", %d, %d, %d, %d);\n",
			i, 
			rwd.Name.c_str(), 
			rwd.GD_SOFT, rwd.XP_SOFT,
			rwd.GD_HARD, rwd.XP_HARD);
	}
	fclose(f);

	ShellExecute(NULL, "open", "@rewards.txt", "", "", SW_SHOW);
	MessageBox(NULL, "ExportDefaultRewards success", "ExportDefaultRewards", MB_OK);
	TerminateProcess(r3d_CurrentProcess, 0);
}


int CGameRewards::ApiGetDataGameRewards()
{
	CWOBackendReq req("api_GetDataGameRewards.aspx");
	// HACK_FIX: without this line we'll get HTTP 411 on live IIS by some reasons
	// description:
	//  The Web server (running the Web site) thinks that the HTTP data stream sent by the client (e.g. your Web browser or our CheckUpDown robot) should include a 'Content-Length' specification
	req.AddParam("411", "1");

	if(!req.Issue())
	{
		r3dOutToLog("ApiGetDataGameRewards FAILED, code: %d\n", req.resultCode_);
		return req.resultCode_;
	}

	pugi::xml_document xmlFile;
	req.ParseXML(xmlFile);
	pugi::xml_node xmlRwd = xmlFile.child("rewards");
	
	// parse all rewards
	xmlRwd = xmlRwd.first_child();
	while(!xmlRwd.empty())
	{
		int RewardID = xmlRwd.attribute("ID").as_int();
		if(RewardID < 0 || RewardID >= RWD_MAX_REWARD_ID) {
		#ifndef FINAL_BUILD
			r3dError("received invalid reward %d", RewardID);
		#endif
			xmlRwd = xmlRwd.next_sibling();
			continue;
		}

		rwd_s& rwd = rewards_[RewardID];
		rwd.Name    = xmlRwd.attribute("Name").value();
		rwd.GD_SOFT = xmlRwd.attribute("GD_SOFT").as_int();
		rwd.XP_SOFT = xmlRwd.attribute("XP_SOFT").as_int();
		rwd.GD_HARD = xmlRwd.attribute("GD_HARD").as_int();
		rwd.XP_HARD = xmlRwd.attribute("XP_HARD").as_int();
		rwd.IsSet   = true;

		xmlRwd = xmlRwd.next_sibling();
	}
	
	loaded_ = true;
	
	//ExportDefaultRewards();
		
	return 0;
}

