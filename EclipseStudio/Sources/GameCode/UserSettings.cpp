#include "r3dPCH.h"
#include "r3d.h"

#include "UserSettings.h"

UserSettings gUserSettings;

#define USERSETTINGS_FILE "userSettings.xml"

UserSettings::UserSettings()
{

}

UserSettings::~UserSettings()
{

}

bool UserSettings::createFullPath( char* dest, bool old )
{
	bool res = old ? CreateWorkPath(dest) : CreateConfigPath(dest);
	if(res)
		strcat( dest, USERSETTINGS_FILE );
	return res;
}

void UserSettings::loadSettings()
{
	char FullIniPath[ MAX_PATH * 2 ];
	bool createdPath = createFullPath( FullIniPath, false );
	if( createdPath && r3d_access( FullIniPath, 4 ) == 0 )
	{
		r3dOutToLog( "userSettings: found file at %s\n", FullIniPath );
		loadXML(FullIniPath);
	}
	else
	{
		createdPath = createFullPath( FullIniPath, true );

		if( createdPath && r3d_access( FullIniPath, 4 ) == 0 )
		{
			r3dOutToLog( "userSettings: found file at %s\n", FullIniPath );
			loadXML(FullIniPath);
		}
		else
		{
			if( !createdPath )
			{
				r3dOutToLog( "userSettings: Error: couldn't get local app path! Using defaults!\n" );
			}
			else
			{
				r3dOutToLog( "userSettings: couldn't open both %s and %s! Using defaults.\n", USERSETTINGS_FILE, FullIniPath );
			}
		}
	}
}

void UserSettings::saveSettings()
{
	char fullPath[ MAX_PATH * 2 ];

	if( createFullPath( fullPath, false ) )
	{
		r3dOutToLog( "userSettings: Saving settings to %s\n", fullPath );
		saveXML(fullPath);
	}
	else
	{
		r3dOutToLog( "userSettings: couldn't create path to %s\n", fullPath );
	}
}

static const int userSettings_file_version = 2;
void UserSettings::saveXML(const char* file)
{
	// save into xml
	pugi::xml_document xmlFile;
	xmlFile.append_child(pugi::node_comment).set_value("User Settings");

	pugi::xml_node xmlGlobal = xmlFile.append_child();
	xmlGlobal.set_name("Global");

	xmlGlobal.append_attribute("version") = userSettings_file_version;

	pugi::xml_node xmlBrowseFilters = xmlFile.append_child();
	xmlBrowseFilters.set_name("BrowseGamesFilter");

	xmlBrowseFilters.append_attribute("hideempty") = BrowseGames_Filter.hideempty;
	xmlBrowseFilters.append_attribute("hidefull") = BrowseGames_Filter.hidefull;

	xmlBrowseFilters.append_attribute("tracers2") = BrowseGames_Filter.tracers2;
	xmlBrowseFilters.append_attribute("nameplates2") = BrowseGames_Filter.nameplates2;
	xmlBrowseFilters.append_attribute("crosshair2") = BrowseGames_Filter.crosshair2;
	xmlBrowseFilters.append_attribute("enable_options") = BrowseGames_Filter.enable_options;
	xmlBrowseFilters.append_attribute("password") = BrowseGames_Filter.password;

	xmlBrowseFilters.append_attribute("region_us") = BrowseGames_Filter.region_us;
	xmlBrowseFilters.append_attribute("region_eu") = BrowseGames_Filter.region_eu;
	xmlBrowseFilters.append_attribute("region_ru") = BrowseGames_Filter.region_ru;
	xmlBrowseFilters.append_attribute("region_sa") = BrowseGames_Filter.region_sa;

	pugi::xml_node xmlRecentGames = xmlFile.append_child();
	xmlRecentGames.set_name("RecentGames1");
	saveGames(RecentGames, xmlRecentGames);

	pugi::xml_node xmlFavGames = xmlFile.append_child();
	xmlFavGames.set_name("FavoriteGames1");
	saveGames(FavoriteGames, xmlFavGames);

	xmlFile.save_file(file);
}

void UserSettings::loadXML(const char* file)
{
	r3dFile* f = r3d_open(file, "rb");
	if ( !f )
	{
		r3dOutToLog("Failed to open user settings xml file: %s\n", file);
		return;
	}

	char* fileBuffer = game_new char[f->size + 1];
	fread(fileBuffer, f->size, 1, f);
	fileBuffer[f->size] = 0;

	pugi::xml_document xmlFile;
	pugi::xml_parse_result parseResult = xmlFile.load_buffer_inplace(fileBuffer, f->size);
	fclose(f);
	if(!parseResult)
		r3dError("Failed to parse XML, error: %s", parseResult.description());

	// check for version
	pugi::xml_node xmlGlobal = xmlFile.child("Global");
	if(xmlGlobal.attribute("version").as_int() != userSettings_file_version)
	{
		r3dOutToLog("userSettings: old version file. reverting to default\n");
		delete [] fileBuffer;
		return;
	}


	pugi::xml_node xmlBrowseFilters = xmlFile.child("BrowseGamesFilter");

	BrowseGames_Filter.hideempty = xmlBrowseFilters.attribute("hideempty").as_bool();
	BrowseGames_Filter.hidefull = xmlBrowseFilters.attribute("hidefull").as_bool();

	if(!xmlBrowseFilters.attribute("tracers2").empty())
	{
		BrowseGames_Filter.tracers2 = xmlBrowseFilters.attribute("tracers2").as_bool();
		BrowseGames_Filter.nameplates2 = xmlBrowseFilters.attribute("nameplates2").as_bool();
		BrowseGames_Filter.crosshair2 = xmlBrowseFilters.attribute("crosshair2").as_bool();
	}
	if(!xmlBrowseFilters.attribute("enable_options").empty())
	{
		BrowseGames_Filter.enable_options = xmlBrowseFilters.attribute("enable_options").as_bool();
		BrowseGames_Filter.password = xmlBrowseFilters.attribute("password").as_bool();
	}

	BrowseGames_Filter.region_us = xmlBrowseFilters.attribute("region_us").as_bool();
	BrowseGames_Filter.region_eu = xmlBrowseFilters.attribute("region_eu").as_bool();
	BrowseGames_Filter.region_ru = xmlBrowseFilters.attribute("region_ru").as_bool();
	if(!xmlBrowseFilters.attribute("region_sa").empty())
		BrowseGames_Filter.region_sa = xmlBrowseFilters.attribute("region_sa").as_bool();

	loadGames(RecentGames, xmlFile.child("RecentGames1"));
	loadGames(FavoriteGames, xmlFile.child("FavoriteGames1"));

	// delete only after we are done parsing xml!
	delete [] fileBuffer;
}

void UserSettings::loadGames(TGameList& list, const pugi::xml_node& xmlGames)
{
		pugi::xml_node xmlGame = xmlGames.child("Game");
		while(!xmlGame.empty())
		{
			GBGameInfo ginfo;
			ginfo.gameServerId = xmlGame.attribute("id").as_uint();
			ginfo.mapId        = xmlGame.attribute("map").as_int();
			ginfo.maxPlayers   = xmlGame.attribute("plr").as_int();
			ginfo.flags        = xmlGame.attribute("flags").as_int();
			strcpy_s(ginfo.name, xmlGame.attribute("name").value());
			
			list.push_back(ginfo);
			xmlGame = xmlGame.next_sibling();
		}
}

void UserSettings::saveGames(const TGameList& list, pugi::xml_node& xmlGames)
{
	for(TGameList::const_iterator it = list.begin(); it != list.end(); ++it)
	{
		pugi::xml_node xmlGame = xmlGames.append_child();
		xmlGame.set_name("Game");

		xmlGame.append_attribute("id")    = (unsigned int)it->gameServerId;
		xmlGame.append_attribute("map")   = it->mapId;
		xmlGame.append_attribute("plr")   = it->maxPlayers;
		xmlGame.append_attribute("flags") = (unsigned int)it->flags;
		xmlGame.append_attribute("name")  = it->name;
	}
}

void UserSettings::addGameToRecent(const GBGameInfo& ginfo)
{
	for(TGameList::iterator it = RecentGames.begin(); it != RecentGames.end(); ++it)
	{
		if(it->gameServerId == ginfo.gameServerId)
		{
			RecentGames.erase(it);
			break;
		}
	}

	RecentGames.push_front(ginfo);
	while(RecentGames.size() > 13) // limit to one screen in UI
		RecentGames.pop_back();
}
void UserSettings::addGameToFavorite(const GBGameInfo& ginfo)
{
	for(TGameList::iterator it = FavoriteGames.begin(); it != FavoriteGames.end(); ++it)
	{
		if(it->gameServerId == ginfo.gameServerId)
		{
			FavoriteGames.erase(it);
			return;
		}
	}
	FavoriteGames.push_back(ginfo);
}
bool UserSettings::isInRecentGamesList(DWORD gameID)
{
	for(TGameList::iterator it = RecentGames.begin(); it != RecentGames.end(); ++it)
		if(it->gameServerId == gameID)
			return true;
			
	return false;
}
bool UserSettings::isInFavoriteGamesList(DWORD gameID)
{
	for(TGameList::iterator it = FavoriteGames.begin(); it != FavoriteGames.end(); ++it)
		if(it->gameServerId == gameID)
			return true;
			
	return false;
}
