#pragma once

#define R3D_ALLOW_SECTOR_LOADING 1

struct Sector
{
	typedef r3dTL::TArray< r3dMesh* > MeshArr;

	Sector();

	MeshArr	Meshes;
	int		Loaded;
};

class SectorMaster
{
public:
	typedef r3dTL::T2DArray< Sector > SectorGrid;

	struct Settings
	{
		Settings();

		float KeepAliveRadius;
		float SectorSize;

		float MapStartX;
		float MapStartZ;
		float MapSizeX;
		float MapSizeZ;
	};

public:
	SectorMaster();
	~SectorMaster();

	void Load( const char* path );
	void Save( const char* path );

	void Reset();
	void Init( float mapStartX, float MapStartZ, float MapSizeX, float MapSizeZ );

	void AddObject( class GameObject* obj );

	void InitSectors( bool allLoaded );

	void Update( const r3dCamera& cam );

	int UnloadSector( Sector& s );
	int LoadSector( Sector& s );

	void LoadAllSectors();

	R3D_FORCEINLINE r3dPoint3D GetSectorPosition( int x, int z ) const;

	const Settings&	GetSettings() const;
	void			SetSettings( const Settings& sts );

	static void ExtractMeshes( Sector::MeshArr* targArr, GameObject* obj );

	float GetEffectiveKeepAliveRadius() const;

private:
	Settings	m_Settings;
	SectorGrid	m_Sectors;

} extern * g_pSectorMaster;

//------------------------------------------------------------------------

R3D_FORCEINLINE r3dPoint3D SectorMaster::GetSectorPosition( int x, int z ) const
{
	r3dPoint3D res;

	res.x = m_Settings.MapStartX + x * m_Settings.SectorSize;
	res.y = 0.0f;
	res.z = m_Settings.MapStartZ + z * m_Settings.SectorSize;

	return res;
}

//------------------------------------------------------------------------
