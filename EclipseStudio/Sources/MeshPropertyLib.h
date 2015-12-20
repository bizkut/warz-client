#pragma once

struct MeshPropertyLibEntry
{
	enum
	{
		LOD_COUNT = 3
	};

	float DrawDistance;

	int IsGlobal;

	r3dTL::TFixedArray< float, LOD_COUNT > LODDistances;

	MeshPropertyLibEntry();
};


class MeshPropertyLib
{
	// types
public:

	typedef MeshPropertyLibEntry Entry ;

	typedef r3dSTLString string;
	typedef r3dgameMap( string, Entry ) Data ;
	typedef r3dgameVector(r3dSTLString) stringlist_t;

	// 
public:
	MeshPropertyLib();
	~MeshPropertyLib();

	// access
public:
	Entry*			GetEntry( const string& key );
	Entry*			GetGlobalEntry( const string& key );

	int				Load( const char* levelFolder, bool isGlobal );
	void			Unload();

	void			AppendXMLNode( pugi::xml_node node, const string& key, const Entry* entry );
	void			Save( const char* levelFolder, bool isGlobal );

	void			RemoveEntry( const string& key );
	void			RemoveEntry( const string& category, const string& meshName );

	void			AddEntry( const string& key );
	void			AddEntry( const string& category, const string& meshName );

	void			AddGlobalEntry( const string& key );

	bool			Exists( const string& key );

	void			GetEntryNames( stringlist_t* oNames ) const;

	static string	ComposeKey( const string& category, const string& meshName );
	static bool		DecomposeKey( const string& key, string* oCat, string* oMeshName );

	// data
private:
	Data mData;
	Data mGlobalData;

} extern * g_MeshPropertyLib;

//------------------------------------------------------------------------


