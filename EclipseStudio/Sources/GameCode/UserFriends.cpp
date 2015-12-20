#include "r3dPCH.h"
#include "r3d.h"

#include "UserFriends.h"

CUserFriends::CUserFriends()
{
	InitializeCriticalSection(&csFriends_);
	gotNewData = false;
}

CUserFriends::~CUserFriends()
{
	DeleteCriticalSection(&csFriends_);
}

CUserFriends::Friend_s CUserFriends::GetFriendData(DWORD friendId)
{
	r3dCSHolder cs1(csFriends_);

	Friend_s* fr = internalGetFriendData(friendId);
	if(fr) {
		return *fr;
	}

	Friend_s empty;
	empty.friendID = 0;
	return empty;
}

CUserFriends::Friend_s* CUserFriends::internalGetFriendData(DWORD friendId)
{
	for(size_t i = 0, size = friendsCur_.size(); i < size; i++) {
		if(friendsCur_[i].friendID == friendId) {
			return &friendsCur_[i];
		}
	}
	
	return NULL;
}

wiStats CUserFriends::GetFriendStats(DWORD friendId)
{
	r3dCSHolder cs1(csFriends_);
	
	wiStats* stats = internalGetFriendStats(friendId);
	if(stats) return *stats;

	return wiStats();
}

wiStats* CUserFriends::internalGetFriendStats(DWORD friendId)
{
	for(size_t i = 0, size = friendStats_.size(); i < size; i++) {
		if(friendStats_[i].friendID == friendId) {
			return &friendStats_[i].stats;
		}
	}
	
	return NULL;
}

const CUserFriends::Friend_s* CUserFriends::GetPrevFriendData(DWORD friendId)
{
	for(size_t i = 0, size = friendsPrev_.size(); i < size; i++) {
		if(friendsPrev_[i].friendID == friendId) {
			return &friendsPrev_[i];
		}
	}
	
	return NULL;
}

void CUserFriends::ClearNewDataFlag()
{
	r3dCSHolder cs1(csFriends_);
	gotNewData = false;
}

void CUserFriends::SetCurrentData(pugi::xml_node& xmlNode)
{
	r3dCSHolder cs1(csFriends_);

	pugi::xml_node xmlFriends = xmlNode.child("friends");

	ParseFriendStatus(xmlFriends.child("frstatus"));
	ParseAddReqs(xmlFriends.child("frreq"));
	gotNewData = true;
}

void CUserFriends::SetCurrentStats(pugi::xml_document& xmlFile)
{
	r3dCSHolder cs1(csFriends_);

	pugi::xml_node xmlFriends = xmlFile.child("friends");

	pugi::xml_node xmlItem = xmlFriends.first_child();
	while(!xmlItem.empty())
	{
		uint32_t ID  = xmlItem.attribute("ID").as_uint();
		wiStats* stats = internalGetFriendStats(ID);
		
		// do not update stats that we already have
		if(stats) {
			xmlItem = xmlItem.next_sibling();
			continue;
		}
			
		Stats_s st;
		st.friendID = ID;
		st.stats.XP          = xmlItem.attribute("XP").as_uint();
//		st.stats.Kills       = xmlItem.attribute("k").as_uint();
		st.stats.Deaths      = xmlItem.attribute("d").as_uint();
		st.stats.TimePlayed  = xmlItem.attribute("t").as_uint();
		friendStats_.push_back(st);

		xmlItem = xmlItem.next_sibling();
	}
}

void CUserFriends::ParseFriendStatus(pugi::xml_node xmlFriends)
{
	friendsCur_.clear();

	pugi::xml_node xmlItem = xmlFriends.first_child();
	while(!xmlItem.empty())
	{
		uint32_t ID    = xmlItem.attribute("ID").as_uint();
		const char* gsid = xmlItem.attribute("gsid").value();
		const char* GT = xmlItem.attribute("GT").value();
		bool isOnline  = xmlItem.attribute("on").as_bool();
		const char* clanName = xmlItem.attribute("clname").value();
		
		Friend_s fr;
		fr.friendID = ID;
		r3dscpy(fr.gamerTag, GT);
		r3dscpy(fr.clanName, clanName);
		fr.isOnline = isOnline;
		sscanf(gsid, "%I64d", &fr.gameSessionId);
			
		friendsCur_.push_back(fr);
		
		xmlItem = xmlItem.next_sibling();
	}
}

void CUserFriends::ParseAddReqs(pugi::xml_node xmlPending)
{
	addReqs_.clear();

	pugi::xml_node xmlItem = xmlPending.first_child();
	while(!xmlItem.empty())
	{
		uint32_t ID    = xmlItem.attribute("ID").as_uint();
		uint32_t XP    = xmlItem.attribute("XP").as_uint();
		const char* GT = xmlItem.attribute("GT").value();
		const char* clanName = xmlItem.attribute("clname").value();

		AddReq_s ar;
		ar.friendID    = ID;
		ar.HonorPoints = XP;
		r3dscpy(ar.gamerTag, GT);
		r3dscpy(ar.clanName, clanName);
			
		addReqs_.push_back(ar);
		
		xmlItem = xmlItem.next_sibling();
	}
}

CUserFriends::AddReq_s CUserFriends::GetPendingFriendReq()
{
	AddReq_s empty;
	empty.friendID = 0;
	
	r3dCSHolder cs1(csFriends_);
	if(addReqs_.size() == 0) {
		return empty;
	}
		
	for(r3dgameVector(AddReq_s)::iterator it = addReqs_.begin();
		it != addReqs_.end();
		++it)
	{
		const AddReq_s& req = *it;
		
		// check if we already returned it
		bool found = false;
		for(size_t i=0; i<savedAddReqs_.size(); ++i)
		{
			if(savedAddReqs_[i] == req.friendID) {
				found = true;
				break;
			}
		}
		
		if(found)
			continue;
			
		// found not processed req
		savedAddReqs_.push_back(req.friendID);
		return req;
	}
	
	// nothing new found
	return empty;
}

void CUserFriends::ClearSavedFriendReq(DWORD friendId)
{
	r3dCSHolder cs1(csFriends_);
		
	for(r3dgameVector(DWORD)::iterator it = savedAddReqs_.begin();
		it != savedAddReqs_.end();
		++it)
	{
		if((*it) == friendId) {
			savedAddReqs_.erase(it);
			return;
		}
	}
}