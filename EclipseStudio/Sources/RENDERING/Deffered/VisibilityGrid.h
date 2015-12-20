#pragma once

#include "r3dBitMaskArray.h"

class VisibiltyGrid
{
public:
	struct HeightRange
	{
		float	Min ;
		float	Max ;

		int		CellCount ;
	};

	typedef r3dTL::TArray< HeightRange > HeightRanges ;

	// construction/ destruction
public:
	VisibiltyGrid();
	~VisibiltyGrid();

	// manipulation/ access
public:
	void			Init( int CellCountX, int CellCountY, int CellCountZ, float Width, float Height, float Depth, float DesiredCellHeight, r3dPoint3D Start, bool NeedBuildHeights ) ;

	int				GetCellPairIDX( int cx0, int cy0, int cz0, int cx1, int cy1, int cz1 ) const ;
	int				GetVisibilityAt( int cx0, int cy0, int cz0, int cx1, int cy1, int cz1 ) const ;

	void			GetCellCoords( const r3dPoint3D& pos, int * oCX, int* oCY, int * oCZ ) const ;

	static int		IsVisGridObj( GameObject* obj );

	int				Calculate() ;
	void			Save();
	int				Load();

	void			DebugCells() const;

	int				IsBBoxVisibleFrom( int cellX, int cellY, int cellZ, const r3dBoundBox& bbox ) ;

	int				GetCellCountX ( ) const ;
	int				GetCellCountY ( ) const ;
	int				GetCellCountZ ( ) const ;

	int				IsInited( ) const ;

	// helpers
private:
	void			SetupCell( r3dPoint3D ( &vertices ) [ 8 ], r3dBoundBox* box, const HeightRange* hr, int cx, int cy, int cz ) ;
	void			BuildCellHeights( float DesiredCellHeight ) ;

	// data
private:
	r3dBitMaskArray		m_VisibilityCells ;
	HeightRanges		m_CellHeightRanges ;

	r3dPoint3D			m_Start ;

	float				m_Width ;
	float				m_Height ;
	float				m_Depth ;

	float				m_CellWidth ;
	float				m_CellHeight ;
	float				m_CellDepth ;

	float				m_DesiredCellHeight ;

	int					m_CellCountX ;
	int					m_CellCountY ;
	int					m_CellCountZ ;

	int					m_IsInited ;

} extern * g_pVisibilityGrid ;

