#include "r3dPCH.h"
#include "r3d.h"

#include "UI\r3dMenu.h"
#include "UI\UIimEdit.h"
#include "UI\UICurveEditor.h"

#pragma warning(disable: 4244)

bool			imgui_disable_gui = false;

int g_imgui_InDrag = 0;
int g_imgui_LockRbr = 0;

DesktopManager_c * g_pDesktopManager = NULL;

static void Start_Text_Edit(const char* valtext, void* edit_val);
static bool Do_Text_Edit(float x, float y, bool bInFocus);

//--------------------------------------------------------------------------------------------------------
void InitDesktopSystem()
{
	r3d_assert( ! g_pDesktopManager );
	g_pDesktopManager = game_new DesktopManager_c;
	g_pDesktopManager->Init();

}

//--------------------------------------------------------------------------------------------------------
void ReleaseDesktopSystem()
{
	r3d_assert( g_pDesktopManager );
	g_pDesktopManager->Release();
	SAFE_DELETE( g_pDesktopManager );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------------------
DesktopManager_c::DesktopManager_c()
	: m_pActiveDesktop( NULL )
	, m_tDefaultDesktop( "default" )
{
}

//--------------------------------------------------------------------------------------------------------
DesktopManager_c::~DesktopManager_c()
{
	r3d_assert( ! m_dDesctops.Size() );
}

//--------------------------------------------------------------------------------------------------------
void DesktopManager_c::Init()
{
	m_pActiveDesktop = &m_tDefaultDesktop;
	m_pActiveDesktop->SetViewPosition( 0, 0 );
	m_pActiveDesktop->SetViewSize( r3dRenderer->ScreenW, r3dRenderer->ScreenH );
	m_pActiveDesktop->Enable( false, false );
	m_pActiveDesktop->Activate();
}

//--------------------------------------------------------------------------------------------------------
void DesktopManager_c::Release()
{
	m_dDesctops.IterateStart();

	while ( m_dDesctops.IterateNext() )
	{
		Desktop_c * pDesc = m_dDesctops.IterateGet();
		SAFE_DELETE( pDesc );
	} 	

	m_dDesctops.Clear();
	m_dDesktopNeedUpdate.Clear();
}

//--------------------------------------------------------------------------------------------------------
Desktop_c * DesktopManager_c::AddDesktop( const char * szName )
{
	Desktop_c * pDesc = game_new Desktop_c( szName );
	r3d_assert( pDesc );
	m_dDesctops.Add( szName, pDesc );
	return pDesc;
}

//--------------------------------------------------------------------------------------------------------
void DesktopManager_c::Begin( const char * szName )
{
	r3d_assert( m_pActiveDesktop );
	r3d_assert( !m_pActiveDesktop->IsActive() || m_pActiveDesktop == &m_tDefaultDesktop );

	Desktop_c * pDesc = m_dDesctops.GetObject( szName );
	if ( ! pDesc )
		return;

	m_pActiveDesktop->Deactivate();

	m_pActiveDesktop = pDesc ? pDesc : &m_tDefaultDesktop;
	m_pActiveDesktop->Activate();

	if ( m_dDesktopNeedUpdate.FindItemIndex( m_pActiveDesktop ) == INVALID_INDEX )
		m_dDesktopNeedUpdate.PushBack( m_pActiveDesktop );
}

//--------------------------------------------------------------------------------------------------------
void DesktopManager_c::End()
{
	r3d_assert( m_pActiveDesktop );
	m_pActiveDesktop->Deactivate();

	m_pActiveDesktop = &m_tDefaultDesktop;
	m_pActiveDesktop->Activate();
}

//--------------------------------------------------------------------------------------------------------
void DesktopManager_c::Draw()
{
	for ( uint32_t i = 0; i < m_dDesktopNeedUpdate.Count(); i++ )
	{
		m_dDesktopNeedUpdate[ i ]->Draw();
	}	

	m_dDesktopNeedUpdate.Clear();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------------------------------------
Desktop_c::Desktop_c( const char * szName )
	: m_fPosX( 0 )
	, m_fPosY( 0 )
	, m_fViewPosX( 0 )
	, m_fViewPosY( 0 )
	, m_fViewWidth( 1280 )
	, m_fViewHeight( 768 )
	, m_fVirtualWidth( 0 )
	, m_fVirtualHeight( 0 )
	, m_fScrollPosY( 0 )
	, m_fScrollPosX( 0 )
	, m_bHorizontal( true )
	, m_bVertical( true )
	, m_bActive( false )
	, m_sName( szName )
{

}

//--------------------------------------------------------------------------------------------------------
Desktop_c::~Desktop_c()
{
}

//--------------------------------------------------------------------------------------------------------
void Desktop_c::SetViewSize( float fWidth, float fHeight )
{
	m_fViewWidth = fWidth;
	m_fViewHeight = fHeight;
}


//--------------------------------------------------------------------------------------------------------
void Desktop_c::SetVirtualSize( float fWidth, float fHeight )
{
	m_fVirtualWidth = fWidth;
	m_fVirtualHeight = fHeight;
}

//--------------------------------------------------------------------------------------------------------
void Desktop_c::SetViewPosition( float fX, float fY )
{
	m_fViewPosX = fX;
	m_fViewPosY = fY;
}

//--------------------------------------------------------------------------------------------------------
bool Desktop_c::IsWindowVisible( float fX1, float fY1, float fX2, float fY2 ) 
{
	m_fVirtualWidth = R3D_MAX( fX2 - m_fViewPosX, m_fVirtualWidth );
	m_fVirtualHeight = R3D_MAX( fY2 - m_fViewPosY, m_fVirtualHeight );

	if ( fX2 - m_fViewPosX < m_fPosX || fY2 - m_fViewPosY < m_fPosY || fX1 - m_fViewPosX > m_fPosX + m_fViewWidth || fY1 - m_fViewPosY > m_fPosY + m_fViewHeight )
		return false;

	return true;
}

//--------------------------------------------------------------------------------------------------------
void Desktop_c::Draw()
{
	int nVal = imgui_mzMotion;

	if( imgui_mx > m_fViewPosX && imgui_mx < m_fViewPosX + m_fViewWidth + 14 && imgui_my > m_fViewPosY && imgui_my < m_fViewPosY + m_fViewHeight)
		m_fScrollPosY -= nVal * ( 0.5f / m_fVirtualHeight );

	if ( m_bVertical && m_fVirtualHeight > m_fViewHeight )
	{
		imgui_DrawVSlider( m_fViewPosX + m_fViewWidth, m_fViewPosY, m_fViewHeight, 0, 1, &m_fScrollPosY, false );
		m_fPosY = m_fScrollPosY * R3D_MAX( 0.0f, m_fVirtualHeight - m_fViewHeight );
	}
	else
	{
		m_fPosY = 0;
	}

	if ( m_bHorizontal && m_fVirtualWidth > m_fViewWidth )
	{
		imgui_DrawHSlider( m_fViewPosX, m_fViewPosY + m_fViewHeight - 10, m_fViewWidth, 0, 1, &m_fScrollPosX, NULL, NULL, false );
		m_fPosX = m_fScrollPosX * R3D_MAX( 0.0f, m_fVirtualWidth - m_fViewWidth );
	}
	else
	{
		m_fPosX = 0;
	}

	m_fVirtualWidth = m_fViewWidth;
	m_fVirtualHeight = m_fViewHeight;
}

//--------------------------------------------------------------------------------------------------------
void Desktop_c::Enable( bool bHorizontal, bool bVertical )
{
	m_bHorizontal = bHorizontal;
	m_bVertical = bVertical;
}

//--------------------------------------------------------------------------------------------------------
void Desktop_c::Activate()
{
	r3d_assert( ! m_bActive );

	RECT rc;

	rc.left = m_fViewPosX;
	rc.top = m_fViewPosY;
	rc.right = m_fViewPosX + m_fViewWidth;
	rc.bottom = m_fViewPosY + m_fViewHeight;

	r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, true );
	HRESULT hr = r3dRenderer->pd3ddev->SetScissorRect( &rc );
	r3d_assert( SUCCEEDED( hr ) );
	m_bActive = true;
}

//--------------------------------------------------------------------------------------------------------
void Desktop_c::Deactivate()
{
	r3d_assert( m_bActive );
	r3dRenderer->pd3ddev->SetRenderState( D3DRS_SCISSORTESTENABLE, false );
	m_bActive = false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	>	
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

static	r3dColor	bkgColor(80, 80, 80);

	const void*	imgui_val = NULL;		// current editable value. == pointer to FOCUS
	float		imgui_mx;
	float		imgui_my;
	float		imgui_mmx;
	float		imgui_mmy;

	int		imgui_lbp = 0;
	int		imgui_rbp = 0;
	int		imgui_lbr = 0;
	int		imgui_rbr = 0;
	int		imgui_mzMotion;
	bool		imgui_disabled = false;
	
	float		imgui_th  = 20;		// text height (for clicking)
	float		imgui_dy  = 25;

static	bool		imgui_textEditActive = false;
static	bool		imgui_textDirty = false;
static	char		imgui_editText[100]  = "";

		r3dColor	imgui_bkgDlg    = r3dColor(80,80,80,220);

static	r3dColor	imgui_borderCol = r3dColor(60,60,60);
static	r3dColor	imgui_bkgCol    = r3dColor(128,128,128);
static	r3dColor	imgui_nameCol   = r3dColor(255,255,155);
static	r3dColor	imgui_nameColH  = r3dColor(255,255,210);
static	r3dColor	imgui_valCol    = r3dColor(255,255,255);

static const int CIRCLE_VERTS = 8*4;
static float r3d_circleVerts[CIRCLE_VERTS*2];
static int  g_bCursorIsOver2d = 0;
static int g_bLastCursorIsOver2d = 0;
static float g_fSizeThreshold = 2.f;

bool imgui_Init()
{
  for(unsigned i = 0; i < CIRCLE_VERTS; ++i) {
    float a = (float)i/(float)CIRCLE_VERTS * (float)R3D_PI*2;
    r3d_circleVerts[i*2+0] = cosf(a);
    r3d_circleVerts[i*2+1] = sinf(a);
  }
  
  return true;
}
static bool imgui_inited = imgui_Init();

void r3dDrawRoundBox2D(float x, float y, float w, float h, float r, r3dColor clr)
{
  const unsigned n = CIRCLE_VERTS/4;
  R3D_SCREEN_VERTEX verts[(n+1)*4];
  R3D_SCREEN_VERTEX* v = verts;
  const float* cverts = r3d_circleVerts;
  
  x = floorf(x) + 0.5f;
  y = floorf(y) + 0.5f;
  w = floorf(w);
  h = floorf(h);

  for(int i=0; i<(n+1)*4; ++i) {
    v[i].color = clr.GetPacked();
    v[i].z     = r3dRenderer->GetNearPlaneZValue();    //Z;
    v[i].rwh   = 1.0f; //Z;//1.0f / Z;
    v[i].tu    = 0; 
    v[i].tv    = 0; 
  }
	
  for(int i = 0; i <= n; ++i, ++v) {
    v->x = x+w-r + cverts[i*2]*r;
    v->y = y+h-r + cverts[i*2+1]*r;
  }
	
  for(int i = n; i <= n*2; ++i, ++v) {
    v->x = x+r + cverts[i*2]*r;
    v->y = y+h-r + cverts[i*2+1]*r;
  }
	
  for(int i = n*2; i <= n*3; ++i, ++v) {
    v->x = x+r + cverts[i*2]*r;
    v->y = y+r + cverts[i*2+1]*r;
  }
	
  for(int i = n*3; i < n*4; ++i, ++v) {
    v->x = x+w-r + cverts[i*2]*r;
    v->y = y+r + cverts[i*2+1]*r;
  }
  v->x = x+w-r + cverts[0]*r;
  v->y = y+r + cverts[1]*r;

  r3dRenderer->SetTex(NULL);
  r3dRenderer->Render2DPolygon((n+1)*4, verts);
  r3dRenderer->Flush();
}

// change all normal box draws to our round one
static void r3dDrawBox_R_2D(float x, float y, float w, float h, const r3dColor24& clr)
{
	if(w<8) r3dDrawBox2D(x, y, w, h, clr);
	else r3dDrawRoundBox2D(x, y, w, h, 4, clr);
}
#define r3dDrawBox2D r3dDrawBox_R_2D

void imgui_MarkTextEditActive ( bool bActive, void * pDat )
{
	imgui_textEditActive = bActive;
	imgui_val = bActive ? (void *)pDat : (void *)0;
}

static bool gFixCoords = false;

void imgui_SetFixMouseCoords( bool fixCoords )
{
	gFixCoords = fixCoords ;
}

void r3dProcessCopyPaste( char* inOutString, int bufferLength, bool noConsoleLock )
{

	if(
		( ( Keyboard->IsPressed( kbsLeftControl, noConsoleLock )
				||
			Keyboard->IsPressed( kbsRightControl, noConsoleLock ) )
				&&
			Keyboard->IsPressed( kbsV, noConsoleLock ) )
				||
		( (	Keyboard->IsPressed( kbsLeftShift, noConsoleLock )
				||
			Keyboard->IsPressed( kbsRightShift, noConsoleLock ) )
				&&
			Keyboard->IsPressed( kbsIns, noConsoleLock ) )
		)
	{
		if( OpenClipboard( r3dRenderer->HLibWin ) )
		{
			HANDLE handle = GetClipboardData( CF_TEXT );

			if( handle )
			{
				char * pszText = static_cast<char*>( GlobalLock( handle ) );

				if( pszText )
				{
					strncpy( inOutString, pszText, bufferLength - 1 );
				}

				GlobalUnlock( handle );
			}

			CloseClipboard();
		}
	}

	if( (	(	Keyboard->IsPressed( kbsLeftControl, noConsoleLock )
					||
				Keyboard->IsPressed( kbsRightControl, noConsoleLock ) )
					&&
				Keyboard->IsPressed( kbsIns, noConsoleLock )
			)
				||
		(	(	Keyboard->IsPressed( kbsLeftControl, noConsoleLock )
					||
				Keyboard->IsPressed( kbsRightControl, noConsoleLock ) )
					&&
				Keyboard->IsPressed( kbsC, noConsoleLock )
		) )
	{
		if( OpenClipboard( r3dRenderer->HLibWin ) )
		{
			HANDLE alloc = GlobalAlloc( GHND, strlen( inOutString ) + 1 );
			if( alloc )
			{
				if( char* str = (char*)GlobalLock( alloc ) )
				{
					EmptyClipboard();

					strcpy( str, inOutString );

					GlobalUnlock( alloc );

					SetClipboardData( CF_TEXT, alloc );
				}

				CloseClipboard();
			}
		}
	}
}

void imgui_Update()
{
	if( imgui_textEditActive )
	{
		r3dProcessCopyPaste( imgui_editText, sizeof imgui_editText, false );
	}


  g_bLastCursorIsOver2d = g_bCursorIsOver2d;

  g_bCursorIsOver2d = 0;
  int mx, my;

  Mouse->GetXY(mx, my);

  int mmx=Mouse->m_MouseMoveX, mmy=Mouse->m_MouseMoveY;

  imgui_mmx = float(mmx);
  imgui_mmy = float(mmy);

  if( gFixCoords )
  {
    float vx, vy, vw, vh;
    r3dRenderer->GetBackBufferViewport( &vx, &vy, &vw, &vh );

	mx = int( float(mx) - vx );
	my = int( float(my) - vy );
  }
  imgui_mx = float(mx);
  imgui_my = float(my);

  // note that button releases is one-time event, but button presses is every-frame flag
  imgui_lbr = Mouse->WasReleased(r3dMouse::mLeftButton);
  imgui_rbr = Mouse->WasReleased(r3dMouse::mRightButton);
  imgui_lbp = Mouse->IsPressed(r3dMouse::mLeftButton);
  imgui_rbp = Mouse->IsPressed(r3dMouse::mRightButton);
  
  if(!(Mouse->IsPressed(r3dMouse::mLeftButton)) && !imgui_textEditActive) {
    imgui_val = NULL;
  }

  imgui_mzMotion = Mouse->m_WheelScroll;

  g_imgui_InDrag --;

  if( g_imgui_InDrag < 0 )
    g_imgui_InDrag = 0;

  g_imgui_LockRbr --;

  if( g_imgui_LockRbr < 0 )
    g_imgui_LockRbr = 0;
  
  return;
}

bool imgui_IsCursorOver2d()
{
	return g_bCursorIsOver2d?true:false;
}

bool imgui_WasCursorOver2d()
{
	return g_bLastCursorIsOver2d?true:false;
}

typedef r3dgameVector(r3dSTLString) stringlist_t;

char imgui_CurrentDir[256];
stringlist_t imgui_DirFiles;

r3dgameSet( r3dSTLString ) imgui_ListExtensions;

char imgui_CurrentTreeDir[256];
char imgui_CurrentTreeExt[128];
stringlist_t imgui_TreeDirectories;
stringlist_t imgui_TreeDirFullPath;

void imgui_FileListReset ()
{
	imgui_CurrentDir [0] = 0;
}

void FillExt( const char* szExt )
{
	imgui_ListExtensions.clear();
	if ( !szExt )
		return;
	char buff[ MAX_PATH ];
	r3dscpy( buff, szExt );
	char* pStr = buff;
	char* p;
	while ( ( p = strchr(pStr, '|' ) ) != 0 )
	{
		*p = 0;
		imgui_ListExtensions.insert( pStr );
		pStr = p + 1;
	}
	imgui_ListExtensions.insert( pStr );
}

void imgui_FillFiles(const char *Dir, const char* szExtNames )
{
	WIN32_FIND_DATA ffblk;
	HANDLE h;
	char szName[ MAX_PATH ];

	FillExt( szExtNames );
	
	h = FindFirstFile(Dir, &ffblk);
	if(h == INVALID_HANDLE_VALUE) {
		imgui_DirFiles.clear();
		return;
	}

	static stringlist_t DirFiles; // local copy of dir files
	DirFiles.clear();
	
	do {
		if (ffblk.cFileName[0] != '.') 
		{
			FixFileName( ffblk.cFileName, szName );
			if ( szExtNames )
			{
				char ext[ 16 ];
				_splitpath ( szName, NULL, NULL, NULL, ext );
				if ( imgui_ListExtensions.find( ext ) == imgui_ListExtensions.end() )
					continue;
			}
			DirFiles.push_back( szName );
		}
	} while(FindNextFile(h, &ffblk) != 0);
	FindClose(h);

	// check if files was changed
	if(strcmp(imgui_CurrentDir, Dir) == 0)
	{
		if(imgui_DirFiles.size() == DirFiles.size())
			return;
	}

	r3dscpy(imgui_CurrentDir, Dir);
	imgui_DirFiles = DirFiles;
}

int FillTree( r3dSTLString sDir, r3dSTLString sSpaces, int allowCurrent )
{
	WIN32_FIND_DATA ffblk;
	int nNumFiles = 0;
	HANDLE h = FindFirstFile( ( sDir + "*" ).c_str(), &ffblk);
	if(h == INVALID_HANDLE_VALUE)
		return 0;

	char szName[ MAX_PATH ];
	do
	{
		int isCurrent = 0;

		if ( ffblk.cFileName[ 0 ] == '.' )
		{
			if( ffblk.cFileName[ 1 ] == '.' )
				continue;

			if( !allowCurrent )
				continue;
			else
				isCurrent = 1;
		}

		FixFileName( ffblk.cFileName, szName );
		if ( ffblk.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			imgui_TreeDirectories.push_back( sSpaces + szName );

			if( isCurrent )
				imgui_TreeDirFullPath.push_back( sDir );
			else
				imgui_TreeDirFullPath.push_back( sDir + szName );

			nNumFiles++;
			if ( !isCurrent && !FillTree( sDir + szName + "/", "    " + sSpaces, 0 ) )
			{
				if ( stricmp( szName, "particles" ) != 0 )
				{
					imgui_TreeDirectories.pop_back();
					imgui_TreeDirFullPath.pop_back();
					nNumFiles--;
				}
			}
		}
		else
		{
			char ext[ 16 ];
			_splitpath ( szName, NULL, NULL, NULL, ext );
			if ( imgui_ListExtensions.find( ext ) == imgui_ListExtensions.end() )
				continue;
			nNumFiles++;
		}
    } while( FindNextFile( h, &ffblk ) != 0 );

	FindClose(h);

	return nNumFiles;
}

void imgui_FillDirTree(const char* Dir, const char* Ext, int allowCurrent )
{
	if ( !strcmp( imgui_CurrentTreeDir, Dir ) && !strcmp( imgui_CurrentTreeExt, Ext ) )
		return;
	r3dscpy(imgui_CurrentTreeDir, Dir);
	r3dscpy(imgui_CurrentTreeExt, Ext);

	imgui_ListExtensions.clear();

	FillExt( Ext );

	imgui_TreeDirectories.clear();
	imgui_TreeDirFullPath.clear();

	FillTree( Dir, "", allowCurrent );
}

bool imgui_FileList(float sx, float sy, float sw, float sh, const char *Dir, char *edit_val, float *pDirFilesOffset, bool bUseDesktop, bool bChangedVal, int* pDragIndex, const char* szExtNames, int* outSelIdx )
{
	if(imgui_disable_gui)
		return 0;
	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( sx, sy, sx + sw, sy + sh ) )
			return false;

		sx -= Desktop().GetX();
		sy -= Desktop().GetY();
	}

	r3d_assert( pDirFilesOffset ) ;

	g_bCursorIsOver2d |= (imgui_mx > sx-g_fSizeThreshold && imgui_mx < sx+sw+g_fSizeThreshold && imgui_my > sy-g_fSizeThreshold && imgui_my < sy+sh+g_fSizeThreshold);

	imgui_FillFiles(Dir, szExtNames);

  int idx = -1 ;

  for ( unsigned int i = 0; i < imgui_DirFiles.size(); i++ )
  {
	  if ( imgui_DirFiles[i] == edit_val )
	  {
		  idx = i;
		  break;
	  }
  }

  bool res = imgui_DrawList(sx, sy, sw, sh, imgui_DirFiles, pDirFilesOffset, &idx, false, bChangedVal, false, pDragIndex ) ; 

  // something was selected
  if( res )
  {
    if( (int)imgui_DirFiles.size() > idx )
    {
      sprintf(edit_val, "%s", imgui_DirFiles.at(idx).c_str());
    }
  }

  if( outSelIdx )
  {
	  *outSelIdx = idx ;
  }

  return res ;
}

bool imgui_DirList( float sx, float sy, float sw, float sh, const char *Dir, const char *Extensions, char* full_path, float *pDirFilesOffset, int *pCurSel, bool bUseDesktop, bool bUseCheck, bool bChangedVal, int* pDragIndex, int allowCurrent )
{
	if(imgui_disable_gui)
		return 0;
	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( sx, sy, sx + sw, sy + sh ) )
		return false;

		sx -= Desktop().GetX();
		sy -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > sx-g_fSizeThreshold && imgui_mx < sx+sw+g_fSizeThreshold && imgui_my > sy-g_fSizeThreshold && imgui_my < sy+sh+g_fSizeThreshold);

	imgui_FillDirTree( Dir, Extensions, allowCurrent );

	static int CurSel = -1;
	int idx;
	if ( pCurSel )
		idx = *pCurSel;
	else
		idx = CurSel;

	for( unsigned int i = 0; i < imgui_TreeDirFullPath.size(); i++ )
	{
		char path0[ 512 ], path1[ 512 ];

		GetFullPathName( imgui_TreeDirFullPath[ i ].c_str(), sizeof path0, path0, NULL );
		GetFullPathName( full_path, sizeof path1, path1, NULL );

		if( path0[ strlen( path0 ) - 1 ] != '\\' )
			strcat( path0, "\\" );

		if( path1[ strlen( path1 ) - 1 ] != '\\' )
			strcat( path1, "\\" );

		if( !stricmp( path0, path1 ) )
		{
			idx = i;
			break;
		}
	}

	if(!imgui_DrawList(sx, sy, sw, sh, imgui_TreeDirectories, pDirFilesOffset, &idx, false, bChangedVal, true, pDragIndex ))
	{
		if ( !( bUseCheck && !pCurSel && CurSel < 0 ) )
			return 0;
		idx = 0;
	}

	// something was selected
	sprintf(full_path, "%s", imgui_TreeDirFullPath.at(idx).c_str() );

	if( !strcmp( full_path, "." ) )
	{
		strcpy( full_path, Dir );
	}

	if ( pCurSel )
		*pCurSel = idx;
	CurSel = idx;

	return 1;
}





bool imgui_DrawList(float sx, float sy, float sw, float sh, const stringlist_t& names, float* arr_offset, int* edit_val, bool bUseDesktop, bool bChangedVal, bool bLeftAlign, int* pDragIndex )
{
	if(imgui_disable_gui)
		return 0;

	int CurSelection = *edit_val - int( *arr_offset ) ;

	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( sx, sy, sx + sw, sy + sh ) )
			return false;
		sx -= Desktop().GetX();
		sy -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > sx-g_fSizeThreshold && imgui_mx < sx+sw+g_fSizeThreshold && imgui_my > sy-g_fSizeThreshold && imgui_my < sy+sh+g_fSizeThreshold);

  const float font_height = 14;//MenuFont_Editor->getFontHeight();
	
  const int entries     = int(names.size());
  const int linesInWnd  = int(sh / (font_height + 3)) - 1;

  float x = sx + 5;
  float y = sy + 5; //font_height+7;
  float w = sw - 10;

  int onNum = -1;
	
  int CurSelected = CurSelection >= 0 ? CurSelection : 0;
  
  if ( arr_offset && (int)(CurSelected + *arr_offset + 0.5) >= 0 && bChangedVal )
  {
	  if ( CurSelected < *arr_offset || CurSelected > linesInWnd )
		  *arr_offset = *arr_offset + CurSelected;
  }

  r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);
  r3dDrawBox2D(sx,sy, sw, sh, imgui_bkgDlg);

  if(entries > linesInWnd) {
    imgui_DrawVSlider(sx+sw-16-5, y, sh-5-font_height-7, 0.0f, (float)(entries-linesInWnd), arr_offset, false );
	w = sw - 10 -5 - 16;
  }
  
  //pt: disabled, as it's not working properly (locking scroll list from scrolling)
  /*int lastKey = 0;
  if(win::kbhit()) {
    lastKey = win::getch(false);
  }
  
  if((lastKey >= '0' && lastKey <= '9') || (lastKey >= 'a' && lastKey <= 'z') || (lastKey >= 'A' && lastKey <= 'Z')) 
  {
    int c2 = ::toupper(lastKey);
    for(int i=0; i<entries; i++) {
      int c1 = ::toupper(names[i][0]);
      if(c1 == c2) {
        *arr_offset = (float)i;
        break;
      }
    }
  }*/
  
  if(imgui_mx > sx && imgui_mx < sx+sw && imgui_my > sy && imgui_my < sy+sh) {

    if(Keyboard->IsPressed(kbsDown)) {
      *arr_offset += 1.0f;
    }

    if(Keyboard->IsPressed(kbsUp)) {
      *arr_offset -= 1.0f;
    }
    
    *arr_offset -= float(imgui_mzMotion / 120) * 4.0f;
	imgui_mzMotion = 0.f;
  }
  
  if(*arr_offset > float(entries-linesInWnd)) 
    *arr_offset = float(entries-linesInWnd);
  if(*arr_offset < 0)
    *arr_offset = 0;

  int offset = int(*arr_offset);
	
  for(int i=0; i<linesInWnd; i++) {

    r3dColor24 TCol = imgui_valCol;

    int idx = i + offset;
    if(idx >= entries)
      break;

	r3dDrawBox2D(x, y, w, font_height, r3dColor24(100,100,100,200));

    if(imgui_val == NULL) {
      if(imgui_mx >= x && imgui_mx <= x + w && imgui_my >= y && imgui_my <= y+font_height) {
        onNum = idx;

        TCol = r3dColor24(255,168,0); 
			r3dDrawBox2D(x, y, w, font_height, r3dColor24(100,100,255,150));
      }
    }

	if (i == CurSelection)
	{
      TCol = r3dColor24(255,0,0); 
  	  r3dDrawBox2D(x-1, y-1, w+2, font_height+2, r3dColor24(255,0,0));
      r3dDrawBox2D(x, y, w, font_height, r3dColor24(150,150,155));
	}

    const char* name = names[idx].c_str();

	float fPosCorrection;
	if ( bLeftAlign )
	{
		SIZE sz1;
		MenuFont_Editor->GetTextExtent(name, &sz1);
		fPosCorrection = float( sz1.cx ) / 2;
	}
	else
		fPosCorrection = w / 2;

   MenuFont_Editor->DrawScaledText(x-1 + fPosCorrection, y-1+font_height/2, 1,1,1, r3dColor(0,0,0), name, D3DFONT_CENTERED);
    MenuFont_Editor->DrawScaledText(x + fPosCorrection, y+font_height/2, 1,1,1, TCol, name, D3DFONT_CENTERED);
    
    y += font_height + 3;
  }

  //r3dDrawBox2D(sx+5,sy+5, sw-10, font_height, r3dColor24(100,100,100,200));
  //MenuFont_Editor->PrintF(sx+6, sy+5+1, r3dColor::white, "%d/%d", onNum + offset + 1, entries);

	bool bResult = false;

	if ( pDragIndex )
	{
		static bool bDragInProgress = false;
		static bool bDragStart = false;
		static int nStartNum = -1;
		static r3dPoint2D ptDragCoord( 0, 0 );
		*pDragIndex = -1;
		if ( imgui_lbp )
		{
			if ( !bDragInProgress && !bDragStart )
			{
				bDragStart = true;
				ptDragCoord.x = imgui_mx;
				ptDragCoord.y = imgui_my;
				nStartNum = onNum;
			}
			if ( bDragStart && ( abs( ptDragCoord.x - imgui_mx ) > 1 || abs( ptDragCoord.y - imgui_my ) > 1 ) && nStartNum >= 0 )
			{
				bDragInProgress = true;
				bResult = *edit_val != nStartNum;
				*edit_val = nStartNum;
				bDragStart = false;
			}
			if ( bDragInProgress )
				*pDragIndex = *edit_val;
		}

		if ( imgui_lbr || Keyboard->WasPressed(kbsEsc) )
		{
			bDragInProgress = false;
			bDragStart = false;
		}
	}

	if(imgui_val == NULL && imgui_lbr && onNum != -1 && !g_imgui_InDrag) {
		imgui_val = edit_val;
		*edit_val = onNum;
		return true;
	}

  return bResult;
}

bool imgui_DrawList(float sx, float sy, float sw, float sh, const stringlist_t& names, float* arr_offset, r3dTL::TArray<int>* edit_values, bool bUseDesktop /*= true*/, bool bChangedVal /*= false*/, bool bLeftAlign /*= false*/ )
{
	if(imgui_disable_gui)
		return 0;

	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( sx, sy, sx + sw, sy + sh ) )
			return false;
		sx -= Desktop().GetX();
		sy -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > sx-g_fSizeThreshold && imgui_mx < sx+sw+g_fSizeThreshold && imgui_my > sy-g_fSizeThreshold && imgui_my < sy+sh+g_fSizeThreshold);

	const float font_height = 14;

	const int entries     = int(names.size());
	const int linesInWnd  = int(sh / (font_height + 3)) - 1;

	float x = sx + 5;
	float y = sy + 5;
	float w = sw - 10;

	int onNum = -1;

	r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);
	r3dDrawBox2D(sx,sy, sw, sh, imgui_bkgDlg);

	if(entries > linesInWnd)
	{
		imgui_DrawVSlider(sx+sw-16-5, y, sh-5-font_height-7, 0.0f, (float)(entries-linesInWnd), arr_offset, false );
		w = sw - 10 -5 - 16;
	}

	if(imgui_mx > sx && imgui_mx < sx+sw && imgui_my > sy && imgui_my < sy+sh) {

		if(Keyboard->IsPressed(kbsDown)) {
			*arr_offset += 1.0f;
		}

		if(Keyboard->IsPressed(kbsUp)) {
			*arr_offset -= 1.0f;
		}

		*arr_offset -= float(imgui_mzMotion / 120) * 4.0f;
		imgui_mzMotion = 0.f;
	}

	if(*arr_offset > float(entries-linesInWnd)) 
		*arr_offset = float(entries-linesInWnd);
	if(*arr_offset < 0)
		*arr_offset = 0;

	int offset = int(*arr_offset);

	for(int i=0; i<linesInWnd; i++) {

		r3dColor24 TCol = imgui_valCol;

		int idx = i + offset;
		if(idx >= entries)
			break;

		r3dDrawBox2D(x, y, w, font_height, r3dColor24(100,100,100,200));

		if(imgui_mx >= x && imgui_mx <= x + w && imgui_my >= y && imgui_my <= y+font_height) {
			onNum = idx;

			TCol = r3dColor24(255,168,0); 
			r3dDrawBox2D(x, y, w, font_height, r3dColor24(100,100,255,150));
		}

		int selected = 0;

		for( int ii = 0, e = (int)edit_values->Count(); ii < e; ii ++ )
		{
			int offseted = (*edit_values)[ ii ] - offset;

			if( offseted == i )
			{
				selected = 1;
				break;
			}
		}

		if( selected )
		{
			TCol = r3dColor24(255,0,0); 
			r3dDrawBox2D(x-1, y-1, w+2, font_height+2, r3dColor24(255,0,0));
			r3dDrawBox2D(x, y, w, font_height, r3dColor24(150,150,155));
		}

		const char* name = names[idx].c_str();

		float fPosCorrection;
		if ( bLeftAlign )
		{
			SIZE sz1;
			MenuFont_Editor->GetTextExtent(name, &sz1);
			fPosCorrection = float( sz1.cx ) / 2;
		}
		else
			fPosCorrection = w / 2;

		MenuFont_Editor->DrawScaledText(x-1 + fPosCorrection, y-1+font_height/2, 1,1,1, r3dColor(0,0,0), name, D3DFONT_CENTERED);
		MenuFont_Editor->DrawScaledText(x + fPosCorrection, y+font_height/2, 1,1,1, TCol, name, D3DFONT_CENTERED);

		y += font_height + 3;
	}

	bool bResult = false;

	if( imgui_lbr && onNum != -1 && !g_imgui_InDrag)
	{
		int foundIdx = -1;

		for( int i = 0, e = edit_values->Count(); i < e; i ++ )
		{
			if( (*edit_values)[ i ] == onNum )
			{
				foundIdx = i;
				break;
			}
		}

		if( foundIdx < 0 )
		{
			if( Keyboard->IsPressed( kbsLeftControl ) )
			{
				edit_values->PushBack( onNum );
			}
			else
			{
				edit_values->Clear();
				edit_values->PushBack( onNum );
			}
		}
		else
		{
			if( Keyboard->IsPressed( kbsLeftControl ) )
			{
				if( (int)edit_values->Count() > foundIdx )
					edit_values->Erase( foundIdx );
			}
		}

		return true;
	}

	return bResult;
}

bool imgui_DrawListColored(float sx, float sy, float sw, float sh, const clrstringlist_t& names, float* arr_offset, int* edit_val, bool bUseDesktop, bool bLeftAlign )
{
	if(imgui_disable_gui)
		return 0;
	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( sx, sy, sx + sw, sy + sh ) )
			return false;
		sx -= Desktop().GetX();
		sy -= Desktop().GetY();
	}

	int CurSelection = *edit_val - (int)( *arr_offset ) ;

	g_bCursorIsOver2d |= (imgui_mx > sx-g_fSizeThreshold && imgui_mx < sx+sw+g_fSizeThreshold && imgui_my > sy-g_fSizeThreshold && imgui_my < sy+sh+g_fSizeThreshold);

  const float font_height = 14;
	
  const int entries     = int(names.size());
  const int linesInWnd  = int(sh / (font_height + 3)) - 1;

  float x = sx + 5;
  float y = sy + 5; //font_height+7;
  float w = sw - 10;

  int onNum = -1;
	
  int CurSelected = CurSelection >= 0 ? CurSelection : 0;


  r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);
  r3dDrawBox2D(sx,sy, sw, sh, imgui_bkgDlg);

  if(entries > linesInWnd) {
    imgui_DrawVSlider(sx+sw-16-5, y, sh-5-font_height-7, 0.0f, (float)(entries-linesInWnd), arr_offset, false );
	w = sw - 10 -5 - 16;
  }
  
  //pt: disabled, as it's not working properly (locking scroll list from scrolling)
  /*int lastKey = 0;
  if(win::kbhit()) {
    lastKey = win::getch(false);
  }
  
  if((lastKey >= '0' && lastKey <= '9') || (lastKey >= 'a' && lastKey <= 'z') || (lastKey >= 'A' && lastKey <= 'Z')) 
  {
    int c2 = ::toupper(lastKey);
    for(int i=0; i<entries; i++) {
      int c1 = ::toupper(names[i].str[0]);
      if(c1 == c2) {
        *arr_offset = (float)i;
        break;
      }
    }
  }*/
  
  if(imgui_mx > sx && imgui_mx < sx+sw && imgui_my > sy && imgui_my < sy+sh) {

    if(Keyboard->IsPressed(kbsDown)) {
      *arr_offset += 1.0f;
    }

    if(Keyboard->IsPressed(kbsUp)) {
      *arr_offset -= 1.0f;
    }
    
    *arr_offset -= float(imgui_mzMotion / 120) * 4.0f;
	imgui_mzMotion = 0.f;
  }
  
  if(*arr_offset > float(entries-linesInWnd)) 
    *arr_offset = float(entries-linesInWnd);
  if(*arr_offset < 0)
    *arr_offset = 0;

  int offset = int(*arr_offset);
	
  for(int i=0; i<linesInWnd; i++)
  {

    int idx = i + offset;
    if(idx >= entries)
      break;

	r3dColor24 TCol = names[idx].color;

	r3dDrawBox2D(x, y, w, font_height, r3dColor24(100,100,100,200));

    if(imgui_val == NULL) {
      if(imgui_mx >= x && imgui_mx <= x + w && imgui_my >= y && imgui_my <= y+font_height) {
        onNum = idx;

        TCol = r3dColor24(255,168,0); 
			r3dDrawBox2D(x, y, w, font_height, r3dColor24(100,100,255,150));
      }
    }

	if (i == CurSelected)
	{
      TCol = r3dColor24(255,0,0); 
  	  r3dDrawBox2D(x-1, y-1, w+2, font_height+2, r3dColor24(255,0,0));
      r3dDrawBox2D(x, y, w, font_height, r3dColor24(150,150,155));
	}

    const char* name = names[idx].str.c_str();

	float fPosCorrection;
	if ( bLeftAlign )
	{
		SIZE sz1;
		MenuFont_Editor->GetTextExtent(name, &sz1);
		fPosCorrection = float( sz1.cx ) / 2;
	}
	else
		fPosCorrection = w / 2;

	MenuFont_Editor->DrawScaledText(x-1 + fPosCorrection, y-1+font_height/2, 1,1,1, r3dColor(0,0,0), name, D3DFONT_CENTERED);
	MenuFont_Editor->DrawScaledText(x + fPosCorrection, y+font_height/2, 1,1,1, TCol, name, D3DFONT_CENTERED);


    y += font_height + 3;
  }

  //r3dDrawBox2D(sx+5,sy+5, sw-10, font_height, r3dColor24(100,100,100,200));
  //MenuFont_Editor->PrintF(sx+6, sy+5+1, r3dColor::white, "%d/%d", onNum + offset + 1, entries);



  if(imgui_val == NULL && imgui_lbr && onNum != -1 && !g_imgui_InDrag) {
    imgui_val = edit_val;
    *edit_val = onNum;
    return true;
  }
  
  return false;
}

float imgui_DrawHSliderV(bool isFloat, float x, float y, float width, float minval, float maxval, void* edit_val, char* fmt, void* editTag, bool bUseDesktop = true )
{
  if(imgui_disable_gui)
    return 0;

  const float DEF_HEIGHT = 16;
  float value = isFloat ? *(float*)edit_val : *(int*)edit_val;

  float ht = DEF_HEIGHT;

  if ( bUseDesktop )
  {
    if ( ! Desktop().IsWindowVisible( x, y, x + width, y + DEF_HEIGHT ) )
      return value;
    x -= Desktop().GetX();
    y -= Desktop().GetY();
  }

  g_bCursorIsOver2d |= (imgui_mx > x-g_fSizeThreshold && imgui_mx < x+width+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y+DEF_HEIGHT+g_fSizeThreshold);

  bool inText = false;

  if( fmt )
  {
    float TEXT_SIZE = 30.0f;

    char TempS[128];
    if( isFloat )
      sprintf(TempS, fmt, *(float*)edit_val );
    else
      sprintf(TempS, fmt, *(int*)edit_val );

    SIZE sz2;

    int textx = x + width - TEXT_SIZE + 4;

    MenuFont_Editor->GetTextExtent( TempS, &sz2 );
    bool inFocus = (imgui_mx > textx && imgui_mx < textx+sz2.cx && imgui_my > y && imgui_my < y + ht);

    if( imgui_lbp && !imgui_textEditActive )
    {
      if( inFocus )
      {
        Start_Text_Edit(TempS, editTag);
        inText = true;
      }
    }

    if(imgui_textEditActive && imgui_val == editTag)
    {
      inText = true;
      if(Do_Text_Edit(textx, y+2, inFocus))
      {
        if(isFloat)
        {
          value = atof(imgui_editText);
          *(float*)edit_val = value;
        }
        else
        {
          value = atoi(imgui_editText);
          *(int*)edit_val = (int)value;
        }
      }
    }
    else
    {

      MenuFont_Editor->PrintF( textx, y + 2, r3dColor(255, 255, 255), TempS );
    }

    width -= TEXT_SIZE - 2.f;

  }
  
  r3dDrawBox2D(x,y,width,DEF_HEIGHT,imgui_borderCol);
  r3dDrawBox2D(x+2,y+2,width-4,DEF_HEIGHT - 4,imgui_bkgCol);

  if(imgui_val == NULL && imgui_lbp && !inText) {
    if(imgui_mx >= x && imgui_mx <= x+width && imgui_my >= y && imgui_my <= y+DEF_HEIGHT)
      imgui_val = edit_val;
  }
  
  if(edit_val == imgui_val && imgui_lbp && !inText ) {
    value  = imgui_mx - x;
    value /= width;
    value *= (maxval-minval);
    value += minval;
    g_imgui_InDrag = 2;
  }

  value = R3D_CLAMP(value, minval, maxval);

  float curindex = width-4-4; // total screenspace
  curindex /= (maxval-minval); // amount per unit
  curindex *= (value-minval); // ration screensapce times units past min
	
  r3dDrawBox2D(x+2+curindex,y+2,4,DEF_HEIGHT-4,r3dColor(228,228,228));

  if(isFloat) *(float*)edit_val = value;
  else        *(int*)edit_val = int(value);  

  return value;
}

float imgui_DrawHSlider(float x, float y, float width, float minval, float maxval, float* edit_val, char* fmt, void* editTag, bool bUseDesktop ) 
{
	if(imgui_disable_gui)
		return 0;
  return imgui_DrawHSliderV(true, x, y, width, minval, maxval, edit_val, fmt, editTag, bUseDesktop );
}

float imgui_DrawHSliderI(float x, float y, float width, float minval, float maxval, int* edit_val, bool bUseDesktop) 
{
	if(imgui_disable_gui)
		return 0;
  return imgui_DrawHSliderV(false, x, y, width, minval, maxval, edit_val, NULL, edit_val, bUseDesktop);
}


float imgui_DrawVSlider(float x, float y, float height, float minval, float maxval, float* edit_val, bool bUseDesktop )
{ 
	if(imgui_disable_gui)
		return 0;
	const float DEF_WIDTH = 16;
	float value = *edit_val;

	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( x - 2, y - 2, x + DEF_WIDTH, y + height + 2 ) )
			return value;
		x -= Desktop().GetX();
		y -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > x-g_fSizeThreshold && imgui_mx < x+DEF_WIDTH+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y+height+g_fSizeThreshold);

  r3dDrawBox2D(x-2,y-2, DEF_WIDTH, height+4, imgui_borderCol);
  r3dDrawBox2D(x, y, DEF_WIDTH - 4, height, imgui_bkgCol);

  if(imgui_val == NULL && imgui_lbp) {
    if(imgui_mx >= x && imgui_mx <= x+DEF_WIDTH && imgui_my >= y && imgui_my <= y+height)
      imgui_val = edit_val;
  }

  float knobHeight = 10.0f;

  if(edit_val == imgui_val && imgui_lbp) {
    value  = imgui_my - y;
    value /= height;
    value *= (maxval-minval);
    value += minval;
    g_imgui_InDrag = 2;
  }

  value = R3D_CLAMP(value, minval, maxval);

  float curindex = height; // total screenspace
  curindex /= (maxval-minval); // amount per unit
  curindex *= (value-minval); // ration screensapce times units past min
  curindex -= knobHeight * 0.5f;

  curindex = R3D_CLAMP(curindex, 0.0f, height - knobHeight);
    
  r3dDrawBox2D(x-2,y+curindex,DEF_WIDTH,knobHeight,r3dColor(228,228,228));

  *edit_val = value;
  return value;
}

static void Start_Text_Edit(const char* valtext, void* edit_val)
{
  r3d_assert(!imgui_textEditActive);

  imgui_textEditActive = true;
  imgui_textDirty      = true;
  imgui_val = edit_val;
  r3dscpy(imgui_editText, valtext);
  
  Keyboard->ClearPressed();
}

static bool Do_Text_Edit(float x, float y, bool bInFocus)
{
	r3d_assert(imgui_textEditActive);

	char TempS[128];
	sprintf(TempS, "%s_", imgui_editText);
	MenuFont_Editor->PrintF(x, y, r3dColor(255, 155, 15), TempS);

	if((!bInFocus && imgui_lbp) || Keyboard->WasPressed(kbsEnter) || Keyboard->WasPressed(kbsNumericEnter)) {
		imgui_val = NULL;
		imgui_textEditActive = false;
		return true;
	}
	if(Keyboard->WasPressed(kbsEsc)) {
		imgui_val = NULL;
		imgui_textEditActive = false;
		return false;
	}
	if(Keyboard->WasPressed(kbsBACK)) {
		if(*imgui_editText != '\0') {
			imgui_textDirty = false;
			imgui_editText[strlen(imgui_editText)-1] = 0;
		}
	}

	if( Keyboard->WasPressed(kbsDel) 
		) 
	{
		imgui_editText[0] = 0;
	}


	int v = -1;
	if(Keyboard->WasPressed(kbs1)) {
		v = 1;
	}
	if(Keyboard->WasPressed(kbs2)) {
		v = 2;
	}
	if(Keyboard->WasPressed(kbs3)) {
		v = 3;
	}
	if(Keyboard->WasPressed(kbs4)) {
		v = 4;
	}
	if(Keyboard->WasPressed(kbs5)) {
		v = 5;
	}
	if(Keyboard->WasPressed(kbs6)) {
		v = 6;
	}
	if(Keyboard->WasPressed(kbs7)) {
		v = 7;
	}
	if(Keyboard->WasPressed(kbs8)) {
		v = 8;
	}
	if(Keyboard->WasPressed(kbs9)) {
		v = 9;
	}
	if(Keyboard->WasPressed(kbs0)) {
		v = 0;
	}

	if(Keyboard->WasPressed(kbsNumericEnd)) {
		v = 1;
	}
	if(Keyboard->WasPressed(kbsNumericDown)) {
		v = 2;
	}
	if(Keyboard->WasPressed(kbsNumericPgDn)) {
		v = 3;
	}
	if(Keyboard->WasPressed(kbsNumericLeft)) {
		v = 4;
	}
	if(Keyboard->WasPressed(kbsNumericFive)) {
		v = 5;
	}
	if(Keyboard->WasPressed(kbsNumericRight)) {
		v = 6;
	}
	if(Keyboard->WasPressed(kbsNumericHome)) {
		v = 7;
	}
	if(Keyboard->WasPressed(kbsNumericUp)) {
		v = 8;
	}
	if(Keyboard->WasPressed(kbsNumericPgUp)) {
		v = 9;
	}
	if(Keyboard->WasPressed(kbsNumericIns)) {
		v = 0;
	}
	
	if(Keyboard->WasPressed(kbsPeriod) || Keyboard->WasPressed(kbsNumericDel)) 
		v = '.' - '0';
	if(Keyboard->WasPressed(kbsMinus) || Keyboard->WasPressed(kbsGrayMinus))
		v = '-' - '0';

	if(v != -1) {
		if(imgui_textDirty) {
			r3dscpy(imgui_editText, "");
			imgui_textDirty = false;
		}
		sprintf(imgui_editText + strlen(imgui_editText), "%c", v + '0');
	}

	return false;
}

float imgui_DrawPanel ( float x, float y, float w, float h, bool bUseDesktop )
{
	if(imgui_disable_gui)
		return 0;
	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( x, y, x + w, y + h ) )
			return h+1;
		x -= Desktop().GetX();
		y -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > x-g_fSizeThreshold && imgui_mx < x+w+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y+h+g_fSizeThreshold);

	r3dDrawBox2D(x,y,w,h,imgui_bkgDlg);

	return float(h+1);
}

int imgui_Value_SliderV(bool isFloat, float x, float y, const char* name, void* edit_val, float minval, float maxval, const char* fmt, int doslider, bool bUseDesktop, int width )
{
	if(imgui_disable_gui)
		return 0;

  SIZE sz1, sz2;
  MenuFont_Editor->GetTextExtent(name, &sz1);
  float tw = (float)sz1.cx;

  int wid = doslider ? 360 : 200;
  if(width > 0)
	  wid = width;

  int ht = 25;
  int textx = 100+(wid-160);

	float f = wid - (x+textx-8);

	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( x, y, x + wid, y + ht ) )
			return ht+1;
		x -= Desktop().GetX();
		y -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > x-g_fSizeThreshold && imgui_mx < x+wid+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y+ht+g_fSizeThreshold);

  r3dDrawBox2D(x,y,wid,ht,imgui_bkgDlg);

 MenuFont_Editor->PrintF(x+5,y+ht/2-sz1.cy/2,imgui_nameCol, name);

  //r3dDrawBox2D ( x+textx-4, y+4, wid - (x+textx-8), ht-8, r3dColor(0,0,0));

  r3dDrawBox2D ( x+textx-4, y+4, f, ht-8, r3dColor(0,0,0));

  char fmt2[100];
  sprintf(fmt2, "%s", fmt);
  char valtext[100];

  if( isFloat )
	  sprintf( valtext, fmt2, *(float*)edit_val );
  else
	  sprintf( valtext, fmt2, *(int*)edit_val );

  MenuFont_Editor->GetTextExtent(valtext, &sz2);

  bool inFocusPure = (imgui_mx > x+textx && imgui_mx < x+textx+sz2.cx && imgui_my > y && imgui_my < y + ht);
  bool inFocus = (imgui_val == NULL && inFocusPure );
 
   // process text editing here
  if(imgui_textEditActive && imgui_val == edit_val) {

    if(Do_Text_Edit(x+textx, y+ht/2-sz1.cy/2, inFocusPure)) {
      if(isFloat) *(float*)edit_val = atof(imgui_editText);
      else        *(int*)edit_val   = atoi(imgui_editText);
    }
    return ht+1;
  }

  if(doslider) {
    imgui_DrawHSliderV(isFloat, x+100, y+4, wid-170, minval, maxval, edit_val, NULL, edit_val, false);
  }
    
  if(inFocus) {
//    MenuFont_Editor->PrintF(x+textx,y+1,imgui_valCol, valtext);
	r3dDrawBox2D ( x+textx-2,y+ht/2-sz2.cy/2-2, sz2.cx+4, sz2.cy+4, r3dColor(120,0,0));
  }

  MenuFont_Editor->PrintF(x+textx,y+ht/2-sz2.cy/2,imgui_valCol, valtext);

  if(inFocus && imgui_lbp && !imgui_textEditActive) {
    Start_Text_Edit(valtext, edit_val);
  }
    
  return ht+1;
}

int imgui_Value_Slider(float x, float y, const char* name, float* edit_val, float minval, float maxval, const char* fmt, int doslider, bool bUseDesktop, int optWidth )
{
	if(imgui_disable_gui)
		return 0;
  return imgui_Value_SliderV(true, x, y, name, edit_val, minval, maxval, fmt, doslider, bUseDesktop, optWidth);
}

int imgui_Value_SliderI(float x, float y, const char* name, int* edit_val, float minval, float maxval, const char* fmt, int doslider, bool bUseDesktop, int optWidth )
{
	if(imgui_disable_gui)
		return 0;
  return imgui_Value_SliderV(false, x, y, name, edit_val, minval, maxval, fmt, doslider, bUseDesktop, optWidth);
}


bool Edit_Value_List(float x, float y, const char* name, int* edit_val, const char** list, int numlist)
{
	if(imgui_disable_gui)
		return false;
  *edit_val = R3D_CLAMP(*edit_val, 0, numlist-1);
  
  SIZE sz1;
  MenuFont_Editor->GetTextExtent(name, &sz1);
  SIZE sz2;
  MenuFont_Editor->GetTextExtent(list[*edit_val], &sz2);

  int inFocus = 0;
  if(imgui_val == NULL && imgui_mx > x && imgui_mx < x + float(sz1.cx+sz2.cx) && imgui_my > y && imgui_my < y + imgui_th) 
    inFocus = 1;

  g_bCursorIsOver2d |= imgui_mx > x-g_fSizeThreshold && imgui_mx < x + float(sz1.cx+sz2.cx)+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y + imgui_th+g_fSizeThreshold;

  if(inFocus) MenuFont_Editor->PrintF(x+1,y+1, imgui_nameColH, "%s", name);
  MenuFont_Editor->PrintF(x, y, imgui_nameCol, "%s", name);
  MenuFont_Editor->PrintF(x+float(sz1.cx), y, imgui_valCol, ":%s", list[*edit_val]);
  
  if(imgui_lbp && inFocus) {
    imgui_val = edit_val;

    int v = *edit_val + 1;
    if(v >= numlist) {
      v = 0;
    }
    *edit_val = v;
    return true;
  }
  
  return false;
}


bool Edit_Value_YN(float x, float y, const char* name, int* edit_val)
{
	if(imgui_disable_gui)
		return false;
  const static char* list[2] = {
    "NO",
    "YES"
  };
  
  if(*edit_val != 0) {
    *edit_val = 1;
  }
  
  return Edit_Value_List(x, y, name, edit_val, list, 2);
}



int imgui_Checkbox(float x, float y, int wid, int ht, const char* name, int* edit_val, const DWORD flag, bool * pInFocus/* = NULL*/, bool bUseDesktop );


int imgui_Checkbox(float x, float y, const char* name, int* edit_val, const DWORD flag, bool * pInFocus/* = NULL*/, bool bUseDesktop )
{
	return imgui_Checkbox(x, y, 360, 25, name, edit_val, flag, pInFocus, bUseDesktop );
}

int imgui_Checkbox(float x, float y, int wid, int ht, const char* name, int* edit_val, const DWORD flag, bool * pInFocus/* = NULL*/, bool bUseDesktop )
{
	if(imgui_disable_gui)
		return 0;
  int textx = 100+(wid-160);

  if ( bUseDesktop )
  {
	  if ( ! Desktop().IsWindowVisible( x, y, x + wid, y + ht ) )
		  return ht+1;
	  x -= Desktop().GetX();
	  y -= Desktop().GetY();
  }

  r3dDrawBox2D(x,y,wid,ht,imgui_bkgDlg);

  SIZE sz1;
  MenuFont_Editor->GetTextExtent(name, &sz1);
  MenuFont_Editor->PrintF(x+2, y+ht/2-sz1.cy/2, imgui_nameCol, "%s", name);


  int temp_val = (*edit_val & flag) ? 1 : 0;

  int inFocus = 0;

  int checkBoxX = x+wid-2-20;
  int checkBoxY = y+2;
  int checkBoxW = 20;
  int checkBoxH = 20;

  if( imgui_val == NULL && 
      imgui_mx > checkBoxX && 
      imgui_mx < checkBoxX + checkBoxW && 
      imgui_my > checkBoxY && 
      imgui_my < checkBoxY + checkBoxH &&
      !g_imgui_InDrag )
    inFocus = 1;

  g_bCursorIsOver2d |= imgui_mx > x-g_fSizeThreshold && imgui_mx < (x + wid)+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y + ht+g_fSizeThreshold;

  if(imgui_lbr && inFocus)
    temp_val =  1 - temp_val;

  r3dColor borderClr;
  r3dColor innerClr;

  if(inFocus)
      borderClr = r3dColor(255,0,0);
  else
      borderClr = r3dColor(255,255,255);

  if( temp_val )
      innerClr = r3dColor(0,255,0);
  else
      innerClr = r3dColor(0,15,0);

  r3dDrawBox2D( checkBoxX, checkBoxY, checkBoxW, checkBoxH, borderClr );
  r3dDrawBox2D( checkBoxX+1, checkBoxY+1, checkBoxW-2, checkBoxH-2, innerClr );

   *edit_val &= ~flag;
   if(temp_val) 
	   *edit_val |= flag;
   if(pInFocus) 
	   *pInFocus = inFocus==1?true:false;

  return ht+1;
}

int imgui_Checkbox_Small(float x, float y,int wid, const char* name, int* edit_val, const DWORD flag, bool bUseDesktop )
{
	if(imgui_disable_gui)
		return 0;
  int ht = 25;

  if ( bUseDesktop )
  {
	  if ( ! Desktop().IsWindowVisible( x, y, x + wid, y + ht ) )
		  return ht+1;
	  x -= Desktop().GetX();
	  y -= Desktop().GetY();
  }

  r3dDrawBox2D(x,y,wid,ht,imgui_bkgDlg);

  SIZE sz1;
  MenuFont_Editor->GetTextExtent(name, &sz1);
  MenuFont_Editor->PrintF(x+2, y+ht/2-sz1.cy/2, imgui_nameCol, "%s", name);


  int temp_val = (*edit_val & flag) ? 1 : 0;

  int inFocus = 0;
  if(imgui_val == NULL && imgui_mx > x && imgui_mx < (x + wid) && imgui_my > y && imgui_my < y + ht) 
    inFocus = 1;

  g_bCursorIsOver2d |= imgui_mx > x-g_fSizeThreshold && imgui_mx < (x + wid)+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y + ht+g_fSizeThreshold;

  if(imgui_lbr && inFocus)
    temp_val =  1 - temp_val;

  if(inFocus)
      r3dDrawBox2D(x+wid-2-20,y+2,20,20,r3dColor(255,0,0));
  else
      r3dDrawBox2D(x+wid-2-20,y+2,20,20,r3dColor(255,255,255));

  if (temp_val)
     r3dDrawBox2D(x+wid-2-20+1,y+2+1,20-2,20-2,r3dColor(0,255,0));
  else
     r3dDrawBox2D(x+wid-2-20+1,y+2+1,20-2,20-2,r3dColor(0,15,0));

   *edit_val &= ~flag;
   if(temp_val) *edit_val |= flag;

  return ht+1;
}


bool Edit_Value_Static(float x, float y, const char* name, const char* value)
{
	if(imgui_disable_gui)
		return false;
  SIZE sz1;
  MenuFont_Editor->GetTextExtent(name, &sz1);
  SIZE sz2;
  MenuFont_Editor->GetTextExtent(value, &sz2);
  
  int inFocus = 0;
  if(imgui_val == NULL && imgui_mx > x && imgui_mx < x + float(sz1.cx+sz2.cx) && imgui_my > y && imgui_my < y + imgui_th) 
    inFocus = 1;

  g_bCursorIsOver2d |= imgui_mx > x-g_fSizeThreshold && imgui_mx < x + float(sz1.cx+sz2.cx)+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y + imgui_th+g_fSizeThreshold;
  
  if(inFocus) MenuFont_Editor->PrintF(x+1,y+1, imgui_nameColH, "%s", name);
  MenuFont_Editor->PrintF(x, y, imgui_nameCol, "%s", name);
  MenuFont_Editor->PrintF(x+float(sz1.cx),y,imgui_valCol, ":%s", value);
  
  if(imgui_lbp && inFocus) {
    imgui_val = name;
    return true;
  }
  
  return false;
}


int imgui_DrawColorPicker(float x, float y, const char* name, r3dColor* edit_val, float width, bool withAlpha, bool bUseDesktop, bool bHSV ) 
{
	if(imgui_disable_gui)
		return 0;
  static float      saved_fc[4];  // current float data of color
  static float      saved_fc_hsv[4];  // current float data of color
  static const void* saved_inst = NULL; // pointer to r3dColor variable
  static const void* saved_inst_hsv = NULL; // pointer to r3dColor variable
 
  float fc[4];
  fc[0] = float(edit_val->R);
  fc[1] = float(edit_val->G);
  fc[2] = float(edit_val->B);
  fc[3] = float(edit_val->A);

  float fc_hsv[4];

 
  int ht = (25+17+17+17+4);
  if ( withAlpha )
	  ht += 17;

 
  if ( bHSV )
  {
	  ht += (4+17+17+17);
	
	  r3dVector v = r3dRGBtoHSV( *edit_val );
	  fc_hsv[0] = v.x;
	  fc_hsv[1] = v.y;
	  fc_hsv[2] = v.z;	  
  }


  if ( bUseDesktop )
  {
	  if ( ! Desktop().IsWindowVisible( x, y, x + width, y + ht ) )
		  return ht +1;
	  x -= Desktop().GetX();
	  y -= Desktop().GetY();
  }

  g_bCursorIsOver2d |= (imgui_mx > x-g_fSizeThreshold && imgui_mx < x+width+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y+ht+g_fSizeThreshold);

  r3dDrawBox2D(x, y, width, ht, imgui_bkgDlg);
  
  const float dy = 25;


	if ( withAlpha) 
		MenuFont_Editor->PrintF(x+5, y+2, imgui_nameCol, "%s (%d %d %d %d)", name, edit_val->R, edit_val->G, edit_val->B, edit_val->A);
	else
		MenuFont_Editor->PrintF(x+5, y+2, imgui_nameCol, "%s (%d %d %d)", name, edit_val->R, edit_val->G, edit_val->B );

  y += dy;
  r3dDrawBox2D(x+5, y, 50, 50, r3dColor(fc[0], fc[1], fc[2], 255.0f));
 
  if(imgui_val == NULL) 
  {
    saved_inst = NULL;
	saved_inst_hsv = NULL;
  }
 
	if(saved_inst && saved_inst != edit_val)
	{
		// we have some other picker active - just draw sliders here
		r3d_assert(imgui_val);

		imgui_DrawHSlider(x + 60, y, width-65, 0.0f, 255.0f, &fc[0], "%.0f", &edit_val->R, false);
		y += 17;
		imgui_DrawHSlider(x + 60, y, width-65, 0.0f, 255.0f, &fc[1], "%.0f", &edit_val->G, false);
		y += 17;
		imgui_DrawHSlider(x + 60, y, width-65, 0.0f, 255.0f, &fc[2], "%.0f", &edit_val->B, false);
		y += 17;

		if(withAlpha) 
		{
			imgui_DrawHSlider(x + 60, y, width-65, 0.0f, 255.0f, &fc[3], "%.0f", &edit_val->A, false);
			y += 17;
		}


		if ( bHSV )
		{
			y += 4;
			imgui_DrawHSlider(x + 60, y, width-65, 0.0f, 1, &fc_hsv[0], "%.0f", &edit_val->R, false ); 
			y += 17;
			imgui_DrawHSlider(x +60, y, width-65, 0.0f, 1, &fc_hsv[1], "%.0f", &edit_val->G, false ); 	
			y += 17;
			imgui_DrawHSlider(x + 60, y, width-65, 0.0f, 1, &fc_hsv[2], "%.0f", &edit_val->B, false );
			y += 17;
		}
    
    return ht+1;
  }
  
	if(saved_inst == NULL) 
	{
		saved_fc[0] = fc[0];
		saved_fc[1] = fc[1];
		saved_fc[2] = fc[2];
		saved_fc[3] = fc[3];
	}

	if(saved_inst_hsv == NULL) 
	{
		saved_fc_hsv[0] = fc_hsv[0];
		saved_fc_hsv[1] = fc_hsv[1];
		saved_fc_hsv[2] = fc_hsv[2];	
	}	
 
  imgui_DrawHSlider(x + 60, y, width-65, 0.0f, 255.0f, &saved_fc[0], "%.0f", &saved_fc[0], false ); 
  if(imgui_val == &saved_fc[0]) 
	  saved_inst = edit_val;
  y += 17;
  imgui_DrawHSlider(x + 60, y, width-65, 0.0f, 255.0f, &saved_fc[1], "%.0f", &saved_fc[1], false ); 
  if(imgui_val == &saved_fc[1]) 
	  saved_inst = edit_val;
  y += 17;
  imgui_DrawHSlider(x + 60, y, width-65, 0.0f, 255.0f, &saved_fc[2], "%.0f", &saved_fc[2], false ); 
  if(imgui_val == &saved_fc[2]) 
	  saved_inst = edit_val;
  y += 17;
  if(withAlpha)
  {
	  imgui_DrawHSlider(x + 60, y, width-65, 0.0f, 255.0f, &saved_fc[3], "%.0f", &saved_fc[3], false ); if(imgui_val == &saved_fc[3]) saved_inst = edit_val;
	  y += 17;
  }

  if ( bHSV )
  {
	  y += 4;
	  imgui_DrawHSlider(x + 60, y, width-65, 0.0f, 1, &saved_fc_hsv[0], "%.0f", &saved_fc_hsv[0], false ); 
	  if(imgui_val == &saved_fc_hsv[0]) 
		  saved_inst_hsv = edit_val;
	  MenuFont_Editor->PrintF(x+10, y+2, imgui_nameCol, "H: %.0f", saved_fc_hsv[0]*255.f );
	  y += 17;

	  imgui_DrawHSlider(x +60, y, width-65, 0.0f, 1, &saved_fc_hsv[1], "%.0f", &saved_fc_hsv[1], false ); 
	  if(imgui_val == &saved_fc_hsv[1]) 
		  saved_inst_hsv = edit_val;
	  MenuFont_Editor->PrintF(x+10, y+2, imgui_nameCol, "S: %.0f", saved_fc_hsv[1]*255.f );
	  y += 17;

	  imgui_DrawHSlider(x + 60, y, width-65, 0.0f, 1, &saved_fc_hsv[2], "%.0f", &saved_fc_hsv[2], false );
	  if(imgui_val == &saved_fc_hsv[2])
		  saved_inst_hsv = edit_val;
	  MenuFont_Editor->PrintF(x+10, y+2, imgui_nameCol, "V: %.0f", saved_fc_hsv[2]*255.f );
	  y += 17;
  }
 
  if(saved_inst) 
  {
    r3d_assert(saved_inst == edit_val);
    
    edit_val->R = BYTE(saved_fc[0]);
    edit_val->G = BYTE(saved_fc[1]);
    edit_val->B = BYTE(saved_fc[2]);
    edit_val->A = BYTE(saved_fc[3]);
  }
  else if ( saved_inst_hsv )
  {
	  r3d_assert(saved_inst_hsv == edit_val);
	  *edit_val = r3dHSVtoRGB( r3dVector( saved_fc_hsv[ 0 ], saved_fc_hsv[ 1 ], saved_fc_hsv[ 2 ] ) );
  }


  
  return ht+1;
}


int imgui_DrawColorGradient(float x, float y, const char* name, r3dTimeGradient2* edit_val, float width, float currentTime, bool bUseDesktop )
{
	if(imgui_disable_gui)
		return 0;
  static const void* saved_inst = NULL;
  static int         saved_idx  = -1;
  static float	     dragStartTime = 999999;

  const float gradH = 24;
  
  const float Ht = gradH + 10 +17*3+2;

  if ( bUseDesktop )
  {
	  if ( ! Desktop().IsWindowVisible( x, y, x + width, y + Ht ) )
		  return Ht +1;
	  x -= Desktop().GetX();
	  y -= Desktop().GetY();
  }

  g_bCursorIsOver2d |= (imgui_mx > x-g_fSizeThreshold && imgui_mx < x+width+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y+Ht+g_fSizeThreshold);

  r3dDrawBox2D(x, y, width, Ht, imgui_bkgDlg);
  
  r3dDrawBox2D(x+2, y+2, width-4, gradH, imgui_borderCol);
  
  r3dTimeGradient2& gr = *edit_val;
  
  for(int i=0; i<gr.NumValues-1; i++) {
    const r3dTimeGradient2::val_s& v1 = gr.Values[i];
    const r3dTimeGradient2::val_s& v2 = gr.Values[i+1];
    
    float x1 = (v1.time / 1.0f) * (width-8);
    float x2 = (v2.time / 1.0f) * (width-8);
    
    r3dColor clr1(v1.val[0], v1.val[1], v1.val[2], 255.0f);
    r3dColor clr2(v2.val[0], v2.val[1], v2.val[2], 255.0f);
    r3dDrawBox2DGradientH(x +4+ x1, y+4, (x2-x1), gradH-4, clr1, clr2);
  }

  int onIdx = -1;

  for(int i=0; i<gr.NumValues; i++) {
    const r3dTimeGradient2::val_s& v1 = gr.Values[i];

    float gx = x +4+ (v1.time / 1.0f) * (width-8);
    float gy = y+2;

    if( !g_imgui_InDrag && imgui_mx >= (gx-4) && imgui_mx <= (gx+4) && imgui_my >= gy && imgui_my <= (gy+gradH)) {
      onIdx = i;
    }
    
    if(imgui_val == NULL && onIdx == i) {
      r3dDrawBox2D(gx-1, gy-1, 4, gradH+4, r3dColor::white);
      r3dDrawBox2D(gx,   gy,   2, gradH,   r3dColor::black);
    } else if(saved_inst == edit_val && i == saved_idx) {
      r3dDrawBox2D(gx-1, gy-2, 4, gradH+6, r3dColor::white);
      r3dDrawBox2D(gx,   gy,   2, gradH,   r3dColor::blue);
    } else {
      r3dDrawBox2D(gx-1, gy-1, 5, gradH+4, r3dColor::black);
      r3dDrawBox2D(gx,   gy,   3, gradH+2, r3dColor::white);
    }
  }

  bool inFocus = false;

  if(!g_imgui_InDrag && imgui_mx >= x && imgui_mx <= x+width && imgui_my >= y && imgui_my <= (y+gradH)) {
    inFocus = true;
  }

  if(imgui_val == NULL && inFocus && onIdx == -1) 
    r3dDrawBox2D(imgui_mx, y-2, 2, gradH+4, r3dColor::white);

  if( currentTime >= 0.f && currentTime <= 1.f )
  {
    float lx = x + 4 + (currentTime / 1.0f) * (width-8);
    r3dDrawLine2D( lx, y + 2, lx, y + gradH + 2, 2.0f, r3dColor::green );
  }

 // draw RGB color pickers
  y += gradH + 10;

  if(saved_inst == edit_val && saved_idx != -1) {
  
    float* fc = gr.Values[saved_idx].val;

    r3dDrawBox2D(x+10, y, 17, 47, r3dColor(fc[0], fc[1], fc[2], 255.0f));

    imgui_DrawHSlider(x + 30, y, width-30-2, 0.0f, 255.0f, &fc[0], "%.0f", &fc[0], false);
    y += 17;
    imgui_DrawHSlider(x + 30, y, width-30-2, 0.0f, 255.0f, &fc[1], "%.0f", &fc[1], false);
    y += 17;
    imgui_DrawHSlider(x + 30, y, width-30-2, 0.0f, 255.0f, &fc[2], "%.0f", &fc[2], false);
    y += 17;
  }

 // do dragging
  if(imgui_val == edit_val && saved_idx != -1) {
    if(!imgui_lbp) {
      imgui_val = NULL;
    } else if(r3dGetTime() > dragStartTime + 0.2f) { 
      // start dragging after 0.2f sec
      float time = (imgui_mx - x) / width;
      time = R3D_CLAMP(time, 0.05f, 0.95f);
      gr.Values[saved_idx].time = time;
      gr.ResortAfterChange(&saved_idx);
    }
    
    return Ht+1;
  }
  
  if(imgui_val == NULL) 
  {
    if(onIdx != -1 ) {
	  g_imgui_LockRbr = 2;
	  if( imgui_rbr )
	  {
		  // remove previously selected point
		  if(onIdx != 0 && onIdx != gr.NumValues-1)
		  {
			  gr.NumValues--;
			  memmove(&gr.Values[onIdx], &gr.Values[onIdx+1], sizeof(gr.Values[0]) * (gr.NumValues - onIdx));

			  saved_inst = NULL;
			  saved_idx  = -1;
			  return Ht+1;
		  }
	  }
    }
  
    if(imgui_lbr && inFocus && onIdx == -1) {
      // add new point
      float time = (imgui_mx - x) / width;
      
      saved_inst = edit_val;
      saved_idx  = gr.AddValue3f(time, 255, 255, 255);
    }

    if(!g_imgui_InDrag && imgui_lbp && onIdx != -1) {
      // select point
      saved_inst = edit_val;
      saved_idx  = onIdx;

      // lock this var for dragging (except corner vars)
      if(onIdx != 0 && onIdx != gr.NumValues-1) {
        imgui_val     = edit_val;
        dragStartTime = r3dGetTime();
      }
    }

  } //if(imgui_val == NULL) 

  return Ht+1;
}





int imgui_DrawFloatGradient(float in_x, float in_y, const char* name, r3dTimeGradient2* edit_val, float in_w, float in_h, 
							float minVal, float maxVal, int x_steps /*= 10*/, int y_steps /*= 10*/, int x_precision /*= 2*/, int y_precision /*= 2*/, float currentTime /*= -1.0f*/, bool bUseDesktop )
{
	if(imgui_disable_gui)
		return 0;
	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( in_x, in_y, in_x + in_w, in_y + in_h ) )
			return in_h+1;
		in_x -= Desktop().GetX();
		in_y -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > in_x-g_fSizeThreshold && imgui_mx < in_x+in_w+g_fSizeThreshold && imgui_my > in_y-g_fSizeThreshold && imgui_my < in_y+in_h+g_fSizeThreshold);

  static UICurveEditor curveEditor;
  curveEditor.DrawIM(in_x, in_y, in_w, in_h, name, edit_val, 0, minVal, maxVal, x_steps, y_steps, x_precision, y_precision, currentTime, false );

  // smooth checkbox
  {
      const int CheckBoxDim = 12;
	  const int DISPLACE = int( in_w - 120 );

	  int wid = in_w - DISPLACE - 8;
	  int ht = CheckBoxDim + 4;
	  int textx = 100+(wid-160);

	  float x = in_x + DISPLACE;
	  float y = in_y + in_h - ht - 6;

	  r3dDrawBox2D(x, y, wid, ht, imgui_bkgDlg );

	  SIZE sz1;
	  MenuFont_Editor->GetTextExtent(name, &sz1);
	  MenuFont_Editor->PrintF(x+2, y+ht/2-sz1.cy/2, r3dColor(255,255,155), "%s", "Smooth" );

	  int temp_val = edit_val->Smooth;

	  int inFocus = 0;
	  if( imgui_val == NULL && 
		  imgui_mx > x && imgui_mx < (x + wid) && 
		  imgui_my > y && imgui_my < y + ht
		  && !g_imgui_InDrag )
		    inFocus = 1;

	  g_bCursorIsOver2d |= imgui_mx > x-g_fSizeThreshold && imgui_mx < (x + wid)+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y + ht+g_fSizeThreshold;

	  if(imgui_lbr && inFocus)
		  temp_val =  1 - temp_val;

	  if(inFocus)
		  r3dDrawBox2D(x+wid-2-CheckBoxDim,y+2,CheckBoxDim,CheckBoxDim,r3dColor(255,0,0));
	  else
		  r3dDrawBox2D(x+wid-2-CheckBoxDim,y+2,CheckBoxDim,CheckBoxDim,r3dColor(255,255,255));

	  if (temp_val)
		  r3dDrawBox2D(x+wid-2-CheckBoxDim+1,y+2+1,CheckBoxDim-2,CheckBoxDim-2,r3dColor(0,255,0));
	  else
		  r3dDrawBox2D(x+wid-2-CheckBoxDim+1,y+2+1,CheckBoxDim-2,CheckBoxDim-2,r3dColor(0,15,0));

	  edit_val->Smooth = temp_val ? true : false;
  }

  return in_h+1;
}


int imgui_DrawBezierGradient(	float in_x, float in_y, const char* name, r3dBezierGradient* edit_val, float in_w, float in_h, 
								int x_steps, int y_steps, int x_precision, int y_precision, bool bUseDesktop )
{
	if(imgui_disable_gui)
		return 0;
	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( in_x, in_y, in_x + in_w, in_y + in_h ) )
			return in_h+1;
		in_x -= Desktop().GetX();
		in_y -= Desktop().GetY();
	}

	float minVal = edit_val->ClampMin;
	float maxVal = edit_val->ClampMax;

	g_bCursorIsOver2d |= (imgui_mx > in_x-g_fSizeThreshold && imgui_mx < in_x+in_w+g_fSizeThreshold && imgui_my > in_y-g_fSizeThreshold && imgui_my < in_y+in_h+g_fSizeThreshold);

	static UICurveEditor curveEditor;
	curveEditor.DrawBezier(in_x, in_y, in_w, in_h, name, edit_val, 0, minVal, maxVal, x_steps, y_steps, x_precision, y_precision, false );

	// smooth checkbox
	{
		const int CheckBoxDim = 12;
		const int DISPLACE = int( in_w - 120 );

		int wid = in_w - DISPLACE - 8;
		int ht = CheckBoxDim + 4;
		int textx = 100+(wid-160);

		float x = in_x + DISPLACE;
		float y = in_y + in_h - ht - 6;

		r3dDrawBox2D(x, y, wid, ht, imgui_bkgDlg );

		SIZE sz1;
		MenuFont_Editor->GetTextExtent(name, &sz1);
		MenuFont_Editor->PrintF(x+2, y+ht/2-sz1.cy/2, r3dColor(255,255,155), "%s", "Smooth" );

		//int temp_val = edit_val->Smooth;

		int inFocus = 0;
		if(imgui_val == NULL && imgui_mx > x && imgui_mx < (x + wid) && imgui_my > y && imgui_my < y + ht) 
			inFocus = 1;

		g_bCursorIsOver2d |= imgui_mx > x-g_fSizeThreshold && imgui_mx < (x + wid)+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y + ht+g_fSizeThreshold;

		//if(imgui_lbr && inFocus)
		//	temp_val =  1 - temp_val;

		if(inFocus)
			r3dDrawBox2D(x+wid-2-CheckBoxDim,y+2,CheckBoxDim,CheckBoxDim,r3dColor(255,0,0));
		else
			r3dDrawBox2D(x+wid-2-CheckBoxDim,y+2,CheckBoxDim,CheckBoxDim,r3dColor(255,255,255));

		//if (temp_val)
		r3dDrawBox2D(x+wid-2-CheckBoxDim+1,y+2+1,CheckBoxDim-2,CheckBoxDim-2,r3dColor(0,255,0));
		//else
		//	r3dDrawBox2D(x+wid-2-CheckBoxDim+1,y+2+1,CheckBoxDim-2,CheckBoxDim-2,r3dColor(0,15,0));

		//edit_val->Smooth = temp_val ? true : false;
	}

	return in_h+1;
}

int imgui_Toolbar(float sx, float sy, float sw, float sh, int *edit_val, int MinIdx, const char* list[], int numlist, bool bUseDesktop )
{
	if(imgui_disable_gui)
		return 0;
	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( sx, sy, sx + sw, sy + sh ) )
			return sh + 1;
		sx -= Desktop().GetX();
		sy -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > sx-g_fSizeThreshold && imgui_mx < sx+sw+g_fSizeThreshold && imgui_my > sy-g_fSizeThreshold && imgui_my < sy+sh+g_fSizeThreshold);

  r3dDrawBox2D(sx,sy, sw, sh, imgui_bkgDlg);

  SIZE sz;
  MenuFont_Editor->GetTextExtent(list[0], &sz);

  int Bw = (sw-10) / numlist-10;

  if (Bw >200) Bw = 200;

  for (int i=0;i<numlist;i++)
  {
	  int xx = sx + 5 +(Bw+10)*i;
	  int yy = sy +sh/2 - sz.cy/2-5;

	  int hover = 0;
	  if(imgui_mx > xx && imgui_mx < xx+Bw && imgui_my > yy && imgui_my < yy+sz.cy+10 && !g_imgui_InDrag) {
        hover = 1;
      }
    
	  int selected = (*edit_val-MinIdx) == i;

	  r3dColor BCol = (hover) ? r3dColor(255,255,255) : r3dColor24(100,100,100);

	  if(selected) 
		r3dDrawBox2D(xx,yy, Bw, sz.cy+10, r3dColor24(255, 0, 0));
	  else
		r3dDrawBox2D(xx,yy, Bw, sz.cy+10, BCol);

      r3dDrawBox2D(xx+2, yy+2, Bw-4, sz.cy+10-4, (selected) ? r3dColor(100, 100, 100) : r3dColor24(0, 0, 0));

	  MenuFont_Editor->DrawScaledText(xx+Bw/2, yy+sz.cy/2+5, 1,1,1, r3dColor(255,255,255), list[i], D3DFONT_CENTERED);

	  if(imgui_lbr && hover) {
			*edit_val = MinIdx +i;
	}
  }

  return sh + 1;
}


bool imgui_Button(float sx, float sy, float sw, float sh, const char* name, int selected, bool bUseDesktop )
{
	if( imgui_disable_gui )
		return 0;

	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( sx, sy, sx + sw, sy + sh ) )
			return false;
		sx -= Desktop().GetX();
		sy -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > sx-g_fSizeThreshold && imgui_mx < sx+sw+g_fSizeThreshold && imgui_my > sy-g_fSizeThreshold && imgui_my < sy+sh+g_fSizeThreshold);

  int hover = 0;
  if(imgui_mx > sx && imgui_mx < sx+sw && imgui_my > sy && imgui_my < sy+sh && !g_imgui_InDrag) {
    hover = 1;
  }
    
  r3dDrawBox2D(sx,sy, sw, sh, (imgui_val == NULL && hover) ? r3dColor(255, 255, 255) : r3dColor24(0,0,0));

  if(selected) 
    r3dDrawBox2D(sx+2,sy+2, sw-4, sh-4, (imgui_val == NULL && hover) ? r3dColor(200, 200, 200) : r3dColor24(160, 160, 160));
  else
    r3dDrawBox2D(sx+2,sy+2, sw-4, sh-4, (imgui_val == NULL && hover) ? r3dColor24(100, 100, 100) : bkgColor);

r3dRenderer->Flush();

  if ( name && *name )
    MenuFont_Editor->DrawScaledText(sx + sw/2, sy+sh/2, 1,1,1, imgui_nameCol, name, D3DFONT_CENTERED);

  if(imgui_val == NULL && imgui_lbr && hover) {
    imgui_val = name;
      
    return true;
  }
  
  return false;
}

bool imgui_ColorButton(float sx, float sy, float sw, float sh, r3dColor color, int selected, bool bUseDesktop )
{
	if(imgui_disable_gui)
		return 0;
	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( sx, sy, sx + sw, sy + sh ) )
			return false;
		sx -= Desktop().GetX();
		sy -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > sx-g_fSizeThreshold && imgui_mx < sx+sw+g_fSizeThreshold && imgui_my > sy-g_fSizeThreshold && imgui_my < sy+sh+g_fSizeThreshold);

	r3dColor invert_color;
	invert_color.R = 255-color.R;
	invert_color.G = 255-color.G;
	invert_color.B = 255-color.B;
	invert_color.A = 255;

	int hover = 0;
	if(imgui_mx > sx && imgui_mx < sx+sw && imgui_my > sy && imgui_my < sy+sh) {
		hover = 1;
	}

	if(selected)
		r3dDrawBox2D(sx,sy, sw, sh, (imgui_val == NULL && hover) ? r3dColor(255, 255, 255) : invert_color);
	else
		r3dDrawBox2D(sx,sy, sw, sh, (imgui_val == NULL && hover) ? r3dColor(255, 255, 255) : r3dColor24(0,0,0));

	r3dDrawBox2D(sx+2,sy+2, sw-4, sh-4, color);

	if(imgui_val == NULL && imgui_lbr && hover) {
		imgui_val = &color;

		return true;
	}

	return false;
}

int imgui_Static(float x, float y, const char* name,  const int Width, bool bUseDesktop, const int Height, bool top_centered_text )
{
	if(imgui_disable_gui)
		return 0;
  int wid = Width;
  int ht = Height;
  int textx = 100+(wid-160);

  if ( bUseDesktop )
  {
	  if ( ! Desktop().IsWindowVisible( x, y, x + wid, y + ht ) )
		  return ht +1;
	  x -= Desktop().GetX();
	  y -= Desktop().GetY();
  }

  g_bCursorIsOver2d |= (imgui_mx > x-g_fSizeThreshold && imgui_mx < x+wid+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y+ht+g_fSizeThreshold);

  r3dDrawBox2D(x,y,wid,ht,imgui_bkgDlg);

  SIZE sz1;

  int tx;
  int ty;

  MenuFont_Editor->GetTextExtent(name, &sz1);

  if( top_centered_text )
  {
	  tx = x + wid / 2 - sz1.cx / 2;
	  ty = y + 2;
  }
  else
  {
	  tx = x + 2;
	  ty = y+ht/2-sz1.cy/2;
  }

  MenuFont_Editor->PrintF( tx, ty, imgui_nameCol, "%s", name);

  return ht +1;
}

int	imgui_Print(float x, float y,const char* name,  bool bUseDesktop )
{
	if( imgui_disable_gui )
		return 0;

	SIZE sz1;

	MenuFont_Editor->GetTextExtent( name, &sz1 );

	float wid = (float)sz1.cx;
	float ht = (float)sz1.cy;

	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( x, y, x + wid, y + ht ) )
			return ht +1;
		x -= Desktop().GetX();
		y -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > x-g_fSizeThreshold && imgui_mx < x+wid+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y+ht+g_fSizeThreshold);

	r3dDrawBox2D(x,y,wid,ht,imgui_bkgDlg);

	MenuFont_Editor->PrintF( x, y, imgui_nameCol, "%s", name);

	return ht +1;
}


//bool imgui_Grid(float sx, float sy, float sw, float sh, const char* name, int Xd, int Yd, )
