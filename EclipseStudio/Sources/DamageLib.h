#pragma once

struct DamageLibEntry
{
	FixedString MeshName;
	FixedString ParticleName;
	FixedString SoundName;
	FixedString AnimName;

	int			UseDestructionMesh;

	int			HitPoints;
	int			HasParicles;
	int			HasSound;
	int			HasAnimation;
	int			DestroyedMeshCollision;

	DamageLibEntry();
};


class DamageLib
{
	// types
public:

	typedef DamageLibEntry Entry ;

	typedef r3dSTLString string;
	typedef r3dgameMap( string, Entry ) Data ;
	typedef r3dgameVector(r3dSTLString) stringlist_t;

	// 
public:
	DamageLib();
	~DamageLib();

	// access
public:
	Entry*			GetEntry( const string& key ) ;
	Entry*			GetEntry( const string& category, const string& meshName );

	int				Load();
	void			Save() const;

	void			RemoveEntry( const string& key );
	void			RemoveEntry( const string& category, const string& meshName );

	void			AddEntry( const string& key ) ;
	void			AddEntry( const string& category, const string& meshName );

	bool			Exists( const string& key ) ;
	bool			Exists( const string& category, const string& meshName );

	void			GetEntryNames( stringlist_t* oNames ) const;

	static string	ComposeKey( const string& category, const string& meshName );
	static bool		DecomposeKey( const string& key, string* oCat, string* oMeshName );

	// data
private:
	Data mData ;

} extern * g_DamageLib;

//------------------------------------------------------------------------

inline
void
DamageLib::AddEntry( const string& category, const string& meshName )
{
	AddEntry( ComposeKey( category, meshName ) );
}

//------------------------------------------------------------------------

inline
void
DamageLib::RemoveEntry( const string& category, const string& meshName )
{
	RemoveEntry( ComposeKey( category, meshName ) );
}

//------------------------------------------------------------------------

