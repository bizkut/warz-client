
#ifndef __PWAR_OBJECT_WATER_PLANE_H
#define __PWAR_OBJECT_WATER_PLANE_H

#ifndef WO_SERVER
#include "XPSObject.h"
#endif // WO_SERVER
#include "water.h"
#include "../../../../GameEngine/gameobjects/GameObj.h"
#ifndef WO_SERVER
#include "building.h"
#endif // WO_SERVER


class obj_WaterPlane : public GameObject, public WaterBase
{
	DECLARE_CLASS(obj_WaterPlane, GameObject)

#ifndef WO_SERVER
	friend void InitWaterValueTrackers(obj_WaterPlane *o);
#endif // WO_SERVER
public:

	struct Settings
	{
		float	WaterPlaneHeight;
		float	CellGridSize;

		float	PlaneXSize;
		float	PlaneZSize;

		float	PlaneCentreX;
		float	PlaneCentreZ;

		int		CoastSmoothLevels;

		Settings();
	};

	struct Info
	{
		Info();

		float OffsetX;
		float OffsetZ;

		int CellCountX;
		int CellCountZ;
	};

	typedef r3dTL::T2DArray<uint8_t> GridVecType;

#ifndef WO_SERVER
	r3dXPSObject<R3D_WATER_PLANE_VERTEX>*	m_Plane;
#endif // WO_SERVER

	obj_WaterPlane();
	~obj_WaterPlane();
	virtual	BOOL		OnCreate();
	virtual	BOOL		OnDestroy();
	virtual	BOOL		Update();
	virtual	void		ReadSerializedData(pugi::xml_node& node);
#ifndef WO_SERVER
	virtual void		WriteSerializedData(pugi::xml_node& node);

	void				DoDraw( const r3dCamera& Cam );
	void				DoDrawDepth( const r3dCamera& Cam );


	virtual void		AppendRenderables( RenderArray ( & render_arrays  )[ rsCount ], const r3dCamera& Cam ) OVERRIDE;

#ifndef FINAL_BUILD
	virtual	float		DrawPropertyEditor(float scrx, float scry, float scrw, float scrh, const AClass* startClass, const GameObjects& selected) OVERRIDE{return 0.0f;};
#endif
#endif // WO_SERVER

public:
	bool				FillWaterGrid ( uint32_t iW, uint32_t iH, bool bUpdateImmediately = true );
	bool				EraseWaterGrid ( uint32_t iW, uint32_t iH, bool bUpdateImmediately = true );
	uint8_t				GetWaterGridValue(uint32_t iW, uint32_t iH);
	void				UpdateWaterPlane ();
	void				UpdateGridOnCellCountChange ();

	void				LoadWaterPlaneGrid(const char*szName);
#ifndef WO_SERVER
	void				SaveWaterPlaneGrid(const char*szName);

	bool WaterSplash(const r3dPoint3D& waterSplashPos, float height, float size, float amount, int texIdx);
#endif // WO_SERVER

	const GridVecType&	GetGrid() const { return m_dGrid; }
	void				SetGrid(const GridVecType &v);

	const Settings&		GetSettings() const;
	void				SetSettings( const Settings& settings );

	const Info&			GetInfo() const;

	void				GetCellIndexes( const r3dPoint3D& pos, int* oCellX, int* oCellZ ) const;

private:

	void				ComputeCoastSmoothInfo (r3dTL::T2DArray<uint8_t> & tInfo);
	void				ComputeWaterVertexesPos (r3dTL::T2DArray<r3dVector> & tInfo, float offX, float offZ );
	void				ResetResizeInfo ();

private:
	GridVecType			m_dGrid;
	float				m_meshHeight;

	// for resizing operation
	float				m_fOldCellGridSize;
	float				m_fOldOffsetX;
	float				m_fOldOffsetZ;

	GridVecType			m_dOldGrid;
	bool				m_bResizeInfoExists;

	Settings			m_Settings;
	Info				m_Info;

public:

	static r3dTL::TArray<obj_WaterPlane*> m_dWaterPlanes;
};


#endif	// __PWAR_OBJECT_WATER_PLANE_H
