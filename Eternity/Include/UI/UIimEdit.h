#pragma once

#include "Tsg_stl\HashTable.h"
#include "Tsg_stl\TString.h"
#include "r3dSpline.h"

typedef r3dgameVector(r3dSTLString) stringlist_t;
struct ColorStringList
{
	r3dSTLString str;
	r3dColor color;
};
typedef r3dgameVector(ColorStringList) clrstringlist_t;

class r3dTimeGradient2;
class r3dBezierGradient;

extern 	float		imgui_mx;
extern 	float		imgui_my;
extern 	float		imgui_mmx;
extern 	float		imgui_mmy;
extern 	int			imgui_lbp;
extern 	int			imgui_rbp;
extern 	int			imgui_lbr;
extern 	int			imgui_rbr;
extern	int			imgui_mzMotion;

extern bool			imgui_disable_gui;

extern	r3dColor	imgui_bkgDlg;


void imgui_Update();
void imgui_SetFixMouseCoords( bool fixCoords );
bool imgui_IsCursorOver2d();
bool imgui_WasCursorOver2d();


bool	Edit_Value_YN(float x, float y, const char* name, int* edit_val);
bool	Edit_Value_List(float x, float y, const char* name, int* edit_val, const char** list, int numlist);
bool	Edit_Value_Static(float x, float y, const char* name, const char* value);



float	imgui_DrawHSlider(float x, float y, float width, float minval, float maxval, float* edit_val, char *fmt, void* tag, bool bUseDesktop = true);
float	imgui_DrawHSliderI(float x, float y, float width, float minval, float maxval, int* edit_val, bool bUseDesktop = true);

int		imgui_Static(float x, float y,const char* name,  const int Width = 360, bool bUseDesktop = true, const int Height = 25, bool top_centered_text = false );
int		imgui_Print(float x, float y,const char* name,  bool bUseDesktop = true );
int		imgui_Checkbox(float x, float y, int wid, int ht, const char* name, int* edit_val, const DWORD flag, bool * pInFocus = NULL, bool bUseDesktop = true );
int		imgui_Checkbox(float x, float y, const char* name, int* edit_val, const DWORD flag, bool * pInFocus = NULL, bool bUseDesktop = true );
bool	imgui_Button(float sx, float sy, float sw, float sh, const char* name, int selected = false, bool bUseDesktop = true );
bool	imgui_ColorButton(float sx, float sy, float sw, float sh, r3dColor color, int selected = false, bool bUseDesktop = true );

int		imgui_Toolbar(float sx, float sy, float sw, float sh, int *edit_val, int MinIdx, const char* list[], int numlist, bool bUseDesktop = true );

bool	imgui_DrawList(float sx, float sy, float sw, float sh, const stringlist_t& names, float* arr_offset, int* edit_val, bool bUseDesktop = true, bool bChangedVal= false, bool bLeftAlign = false, int* pDragIndex = NULL );

bool	imgui_DrawList(float sx, float sy, float sw, float sh, const stringlist_t& names, float* arr_offset, r3dTL::TArray<int>* edit_values, bool bUseDesktop = true, bool bChangedVal= false, bool bLeftAlign = false );

bool	imgui_DrawListColored(float sx, float sy, float sw, float sh, const clrstringlist_t& names, float* arr_offset, int* edit_val, bool bUseDesktop = true, bool bLeftAlign = false );
bool	imgui_FileList(float sx, float sy, float sw, float sh, const char *Dir, char *edit_val, float *pDirFilesOffset , bool bUseDesktop = true, bool bChangedVal= false, int* pDragIndex = NULL, const char* szExtNames = NULL, int* outSelIdx = NULL );
bool	imgui_DirList(float sx, float sy, float sw, float sh, const char *Dir, const char *Extensions, char *edit_val, float *pDirFilesOffset, int *pCurSel, bool bUseDesktop = true, bool bUseCheck = false, bool bChangedVal= false, int* pDragIndex = NULL, int allowCurrent = 0 );


float	imgui_DrawPanel ( float x, float y, float w, float h, bool bUseDesktop = true );
float	imgui_DrawVSlider(float x, float y, float height, float minval, float maxval, float* edit_val, bool bUseDesktop = true);
int		imgui_Value_Slider(float x, float y, const char* name, float* edit_val, float minval, float maxval, const char* fmt, int doslider = true, bool bUseDesktop = true, int optWidth=0 );
int		imgui_Value_SliderI(float x, float y, const char* name, int* edit_val, float minval, float maxval, const char* fmt, int doslider = true, bool bUseDesktop = true, int optWidth=0 );

int		imgui_DrawColorPicker(float x, float y, const char* name, r3dColor* edit_val, float width, bool withAlpha, bool bUseDesktop = true, bool bHSV = false ) ;
int		imgui_DrawColorGradient(float x, float y, const char* name, r3dTimeGradient2* edit_val, float width, float currentTime = -1.f, bool bUseDesktop = true );

int		imgui_DrawFloatGradient(float in_x, float in_y, const char* name, r3dTimeGradient2* edit_val, float in_w, float in_h, 
								float minVal, float maxVal, int x_steps = 10, int y_steps = 10, int x_precision = 2, int y_precision = 2, float currentTime = -1.0f, bool bUseDesktop = true );

int		imgui_DrawBezierGradient(float in_x, float in_y, const char* name, r3dBezierGradient* edit_val, float in_w, float in_h, 
								int x_steps = 10, int y_steps = 10, int x_precision = 2, int y_precision = 2, bool bUseDesktop = true );

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////
class Desktop_c
{
	float		m_fPosX;
	float		m_fPosY;

	float		m_fViewPosX;
	float		m_fViewPosY;

	float		m_fViewWidth;
	float		m_fViewHeight;
	
	float		m_fVirtualWidth;
	float		m_fVirtualHeight;

	float		m_fScrollPosY;
	float		m_fScrollPosX;

	bool		m_bHorizontal;
	bool		m_bVertical;

	bool		m_bActive;

	FixedString	m_sName; // for debug

public:

	bool		IsActive			() const	{ return m_bActive; }

	void		Activate			();
	void		Deactivate			();

	void		Draw				();

	void		Enable				( bool bHorizontal, bool bVertical );

	bool		IsWindowVisible 	( float fX1, float fY1, float fX2, float fY2 );

	float		GetX				() const						{ return m_fPosX; }
	float		GetY				() const						{ return m_fPosY; }

	void		SetViewPosition		( float fX, float fY );
	void		SetViewSize			( float fWidth, float fHeight );
	void		SetVirtualSize		( float fWidth, float fHeight );
	

	Desktop_c( const char * szName );
	virtual ~Desktop_c();
};


struct DesktopHashFunc_T
{
	inline uint32_t operator () ( const char * szKey )
	{
		return r3dHash::MakeHash( szKey );
	}
};



class DesktopManager_c
{
	Desktop_c		m_tDefaultDesktop;

	HashTableDynamic< Desktop_c *, FixedString, DesktopHashFunc_T, 256 > m_dDesctops;
	r3dTL::TArray< Desktop_c * >	m_dDesktopNeedUpdate;

	Desktop_c *		m_pActiveDesktop;	

public:

	Desktop_c &		GetActiveDesctop	() { assert( m_pActiveDesktop ); return *m_pActiveDesktop; }

	void			Init				();
	void			Release				();

	void			Draw				();

	Desktop_c *		AddDesktop			( const char * szName );

	void			Begin				( const char * szName );
	void			End					();

	DesktopManager_c();
	virtual ~DesktopManager_c();
};


extern DesktopManager_c * g_pDesktopManager;

void InitDesktopSystem();
void ReleaseDesktopSystem();


//--------------------------------------------------------------------------------------------------------
inline Desktop_c & Desktop()
{
	return g_pDesktopManager->GetActiveDesctop();
}

extern int g_imgui_LockRbr;