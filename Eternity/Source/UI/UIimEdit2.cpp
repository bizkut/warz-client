#include "r3dPCH.h"
#include "r3d.h"

#include "UI\r3dMenu.h"
#include "UI\UIimEdit.h"
#include "UI\UICurveEditor.h"

#include "UI\UIimEdit2.h"

#pragma warning(disable: 4244)

static int  g_bCursorIsOver2d = false;
static float g_fSizeThreshold = 0.0001f;

extern int g_imgui_InDrag;

#define IMGUI2_ZONEOFFSET   2
#define IMGUI2_INDENTSIZE   5
#define IMGUI2_VALUENAMELEN 100

        struct imgui_State
        {
          float         ax, ay;
          float         aw, ah;           // current area position
          
          float         x, y, w;
          
          bool          border;
        };
        imgui_State     imgui2_stat = {0};

	const void*	imgui2_val = NULL;		// current editable value. == pointer to FOCUS
	float		imgui2_mx;
	float		imgui2_my;
	float		imgui2_mmx;
	float		imgui2_mmy;
	float		imgui2_mzMotion;

	int		imgui2_lbp = 0;
	int		imgui2_rbp = 0;
	int		imgui2_lbr = 0;
	int		imgui2_rbr = 0;
	bool		imgui2_disabled = false;
	
	float		imgui2_th  = 0;		// text height based on current font
	float		imgui2_dy  = 24;        // height between controls
	float		imgui2_ch  = 24;        // control height
	float           imgui2_tdy = 6;         // text offset for aligning within control

static	bool		imgui2_textIsNumbers = false;
static	bool		imgui2_textEditActive = false;
static	bool		imgui2_textDirty = false;
static	char		imgui2_editText[100]  = "";

        r3dColor	imgui2_bkgDlg    = r3dColor(80,80,80,120);

static	r3dColor	imgui2_borderCol = r3dColor(60,60,60);
static	r3dColor	imgui2_bkgCol    = r3dColor(128,128,128);
static	r3dColor	imgui2_nameCol   = r3dColor(255,255,155);
static	r3dColor	imgui2_nameColH  = r3dColor(255,255,210);
static	r3dColor	imgui2_valCol    = r3dColor(255,255,255);

class imguiAddControl
{
  public:
        float h;

        imguiAddControl(int _h = imgui2_dy) {
          if(imgui2_stat.aw == 0) r3dError("imgui area isn't set");
          h = _h;
        }
        ~imguiAddControl() {
          imgui2_stat.y += h;
        }
};

static const int CIRCLE_VERTS = 8*4;
static float r3d_circleVerts[CIRCLE_VERTS*2];

bool imgui2_Init()
{
  for(unsigned i = 0; i < CIRCLE_VERTS; ++i) {
    float a = (float)i/(float)CIRCLE_VERTS * (float)R3D_PI*2;
    r3d_circleVerts[i*2+0] = cosf(a);
    r3d_circleVerts[i*2+1] = sinf(a);
  }
  
  return true;
}
static bool imgui2_inited = imgui2_Init();

static void r3dDrawRoundBox2D(float x, float y, float w, float h, float r, r3dColor clr)
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

bool imgui2_IsCursorOver2d()
{
	return g_bCursorIsOver2d?true:false;
}

void imgui2_Update()
{
	if( imgui2_textEditActive )
	{
		void r3dProcessCopyPaste( char*, int, bool );
		r3dProcessCopyPaste( imgui2_editText, sizeof imgui2_editText, false );
	}

	 g_bCursorIsOver2d = 0;

  if(imgui2_th == 0) {
    SIZE sz1;
    MenuFont_Editor->GetTextExtent("xxx", &sz1);
    imgui2_th = float(sz1.cy);
  }
    
  int mx, my;

  Mouse->GetXY(mx, my);

  imgui2_mx = float(mx);
  imgui2_my = float(my);

  mx = Mouse->m_MouseMoveX;
  my = Mouse->m_MouseMoveY;

  imgui2_mmx = float(mx);
  imgui2_mmy = float(my);

  // note that button releases is one-time event, but button presses is every-frame flag
  imgui2_lbr = Mouse->WasReleased(r3dMouse::mLeftButton);
  imgui2_rbr = Mouse->WasReleased(r3dMouse::mRightButton);
  imgui2_lbp = Mouse->IsPressed(r3dMouse::mLeftButton);
  imgui2_rbp = Mouse->IsPressed(r3dMouse::mRightButton);

  if(!(Mouse->IsPressed(r3dMouse::mLeftButton)) && !imgui2_textEditActive) 
  {
    imgui2_val = NULL;
  }
  
  return;
}

static void imgui2_DrawBorder(float x, float y, float w, float h)
{
  if(imgui2_stat.aw == 0 || !imgui2_stat.border)
    r3dDrawBox2D(x, y, w, h, imgui2_bkgDlg);
}


void imgui2_StartArea(const char* name, float x, float y, float w, float h, bool drawBorder)
{
  if(imgui2_stat.aw != 0) 
    r3dError("imgui2_StartArea already started");
    
  imgui2_stat.ax = x + IMGUI2_ZONEOFFSET;
  imgui2_stat.ay = y + IMGUI2_ZONEOFFSET;
  imgui2_stat.aw = w - IMGUI2_ZONEOFFSET * 2;
  imgui2_stat.ah = h - IMGUI2_ZONEOFFSET * 2;
  imgui2_stat.border = drawBorder;

  imgui2_stat.x  = x + IMGUI2_ZONEOFFSET;
  imgui2_stat.y  = y + IMGUI2_ZONEOFFSET;
  imgui2_stat.w  = imgui2_stat.aw;

  if(drawBorder)
    r3dDrawRoundBox2D(x, y, w, h, 8, imgui2_bkgDlg);
    
  return;
}

void imgui2_EndArea()
{
  imgui2_stat.ax = 0;
  imgui2_stat.ay = 0;
  imgui2_stat.aw = 0;
  imgui2_stat.ah = 0;
}

void imgui2_Indent(float idx)
{
  imgui2_stat.x  = imgui2_stat.ax + IMGUI2_INDENTSIZE * idx;
  imgui2_stat.w  = imgui2_stat.aw - IMGUI2_INDENTSIZE * idx;
}



static  char            imgui2_CurrentDir[256];
static  stringlist_t    imgui2_DirFiles;
static  float           imgui2_DirFilesOffset = 0;

enum FillFilesType
{
	FILLFILES_FILES,
	FILLFILES_DIRS,
	FILLFILES_FILES_AND_DIRS
};

static void imgui2_FillFiles(const char* Dir, FillFilesType fillFileType )
{
  if(!strcmp(imgui2_CurrentDir, Dir)) return;
  r3dscpy(imgui2_CurrentDir, Dir);

  WIN32_FIND_DATA ffblk;
  HANDLE h;

  imgui2_DirFiles.clear();
  imgui2_DirFilesOffset = 0;
    
  h = FindFirstFile(Dir, &ffblk);
  if(h != INVALID_HANDLE_VALUE) {
    do {
      if (ffblk.cFileName[0] != '.')
	  {
		  int isDir = ( ffblk.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY );

		  switch( fillFileType )
		  {
		  case FILLFILES_FILES:
			  if( !isDir )
				imgui2_DirFiles.push_back( ffblk.cFileName );
			  break;
		  case FILLFILES_DIRS:
			  if( isDir )
				imgui2_DirFiles.push_back( ffblk.cFileName );
			  break;
		  case FILLFILES_FILES_AND_DIRS:
			    imgui2_DirFiles.push_back(ffblk.cFileName);
			  break;
		  }
	  }
    } while(FindNextFile(h, &ffblk) != 0);
    FindClose(h);
  }
}


bool imgui2_FileList(float sw, float sh, const char* Dir, char *edit_val, bool bUseDesktop)
{
  imgui2_FillFiles(Dir, FILLFILES_FILES);

  static int CurSel = -1;
  int idx = -1;

  imguiAddControl c( sh );

  if(imgui2_DrawList( imgui2_stat.x, imgui2_stat.y, sw, sh, imgui2_DirFiles, CurSel, &imgui2_DirFilesOffset, &idx, bUseDesktop) == false) 
    return 0;

  // something was selected
  sprintf(edit_val, "%s", imgui2_DirFiles.at(idx).c_str());
  CurSel = idx;

  return 1;
}

bool imgui2_DirList(float sw, float sh, const char* Dir, char* edit_val, bool bUseDesktop /*= true*/ )
{
	imgui2_FillFiles( Dir, FILLFILES_DIRS );

	static int CurSel = -1;
	int idx = -1;

	imguiAddControl c( sh );

	if(imgui2_DrawList( imgui2_stat.x, imgui2_stat.y, sw, sh, imgui2_DirFiles, CurSel, &imgui2_DirFilesOffset, &idx, bUseDesktop) == false) 
		return 0;

	// something was selected
	sprintf(edit_val, "%s", imgui2_DirFiles.at(idx).c_str());
	CurSel = idx;

	return 1;
}

bool imgui2_DrawList( float sx, float sy, float sw, float sh, const stringlist_t& names, int CurSelection, float* arr_offset, int* edit_val, bool bUseDesktop)
{
	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( sx, sy, sx + sw, sy + sh ) )
			return false;
		sx -= Desktop().GetX();
		sy -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > sx-g_fSizeThreshold && imgui_mx < sx+sw+g_fSizeThreshold && imgui_my > sy-g_fSizeThreshold && imgui_my < sy+sh+g_fSizeThreshold);

  const int entries     = int(names.size());
  const int linesInWnd  = int(sh / (imgui2_th + 2)) - 1;
  
  imgui2_DrawBorder(sx, sy, sw, sh);

  float x = sx + 5;
  float y = sy + 5;
  float w = sw - 10;

  int onNum = -1;
	
  int CurSelected = CurSelection;


  r3dRenderer->SetRenderingMode(R3D_BLEND_ALPHA);
  imgui2_DrawBorder(sx, sy, sw, sh);

  if(entries > linesInWnd) {
    imgui2_DrawVSlider(sx+sw-16-5, y, sh-7, 0.0f, (float)(entries-linesInWnd), arr_offset);
    w = sw - 10 -5 - 16;
  }
  
  //pt: disabled, as it's not working properly (locking scroll list from scrolling)
  /*int lastKey = 0;
  if(win::kbhit()) {
    lastKey = win::getch();
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
  
  if(!g_imgui_InDrag && imgui2_mx > sx && imgui2_mx < sx+sw && imgui2_my > sy && imgui2_my < sy+sh) {

    if(Keyboard->WasPressed(kbsDown)) {
      *arr_offset += 1.0f;
    }

    if(Keyboard->WasPressed(kbsUp)) {
      *arr_offset -= 1.0f;
    }
    
    *arr_offset -= float(imgui2_mzMotion / 120) * 4.0f;
  }
  
  if(*arr_offset > float(entries-linesInWnd)) 
    *arr_offset = float(entries-linesInWnd);
  if(*arr_offset < 0)
    *arr_offset = 0;

  int offset = int(*arr_offset);
	
  for(int i=0; i<linesInWnd; i++) 
  {
    r3dColor24 TCol = imgui2_valCol;

    int idx = i + offset;
    if(idx >= entries)
      break;

    r3dDrawBox2D(x, y, w, imgui2_th, r3dColor24(100,100,100,200));

    if(imgui2_val == NULL) {
      if(!g_imgui_InDrag && imgui2_mx >= x && imgui2_mx <= x + w && imgui2_my >= y && imgui2_my <= y+imgui2_th) {
        onNum = idx;

        TCol = r3dColor24(255,168,0); 
        r3dDrawBox2D(x, y, w, imgui2_th, r3dColor24(100,100,255,150));
      }
    }

    if (i == CurSelected)
    {
      TCol = r3dColor24(255,0,0); 
      r3dDrawBox2D(x-1, y-1, w+2, imgui2_th+2, r3dColor24(255,0,0));
      r3dDrawBox2D(x, y, w, imgui2_th, r3dColor24(150,150,155));
    }

    const char* name = names[idx].c_str();
    float tx = floorf(x + w/2);
    float ty = floorf(y+imgui2_th/2);
    MenuFont_Editor->DrawScaledText(tx-1, ty-1, 1,1,1, r3dColor(0,0,0), name, D3DFONT_CENTERED);
    MenuFont_Editor->DrawScaledText(tx, ty, 1,1,1, TCol, name, D3DFONT_CENTERED);
    
    y += imgui2_th + 2;
  }

  //r3dDrawBox2D(sx+5,sy+5, sw-10, imgui2_th, r3dColor24(100,100,100,200));
  //MenuFont_Editor->PrintF(sx+6, sy+5+1, r3dColor::white, "%d/%d", onNum + offset + 1, entries);

  if(!g_imgui_InDrag && imgui2_val == NULL && imgui2_lbr && onNum != -1) {
    imgui2_val = edit_val;
    *edit_val = onNum;
    return true;
  }
  
  return false;
}

bool imgui2_DrawHSliderEx2(bool isFloat, float x, float y, float width, float minval, float maxval, void* edit_val, bool bUseDesktop)
{
	const float fHeight = 16.0f;

	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( x, y, x + width, y + fHeight ) )
			return false;
		x -= Desktop().GetX();
		y -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > x-g_fSizeThreshold && imgui_mx < x+width+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y+fHeight+g_fSizeThreshold);


  float value = isFloat ? *(float*)edit_val : *(int*)edit_val;

  r3dDrawBox2D(x,y,width,fHeight,imgui2_borderCol);
  r3dDrawBox2D(x+2,y+2,width-4,fHeight-4,imgui2_bkgCol);

  if(imgui2_val == NULL && imgui2_lbp) {
    if(imgui2_mx >= x && imgui2_mx <= x+width && imgui2_my >= y && imgui2_my <= y+16)
      imgui2_val = edit_val;
  }
  
  if(edit_val == imgui2_val && imgui2_lbp) {
    value  = imgui2_mx - x;
    value /= width;
    value *= (maxval-minval);
    value += minval;
	g_imgui_InDrag = 2;
  }

  value = R3D_CLAMP(value, minval, maxval);

  float curindex = width-4-4; // total screenspace
  curindex /= (maxval-minval); // amount per unit
  curindex *= (value-minval); // ration screensapce times units past min
	
  r3dDrawRoundBox2D(x+2+curindex,y+2,4,16-4,2,r3dColor(228,228,228));

  if(isFloat) *(float*)edit_val = value;
  else        *(int*)edit_val = int(value);  

  return false;
}

bool imgui2_DrawHSlider(float x, float y, float width, float minval, float maxval, float* edit_val, bool bUseDesktop) 
{
  return imgui2_DrawHSliderEx2(true, x, y, width, minval, maxval, edit_val, bUseDesktop);
}

bool imgui2_DrawVSlider(float x, float y, float height, float minval, float maxval, float* edit_val, bool bUseDesktop)
{
	const float fWidth = 16;
	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( x, y, x + fWidth, y + height ) )
			return false;
		x -= Desktop().GetX();
		y -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > x-g_fSizeThreshold && imgui_mx < x+fWidth+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y+height+g_fSizeThreshold);


  float value = *edit_val;

  r3dDrawBox2D(x-2,y-2, fWidth, height+4, imgui2_borderCol);
  r3dDrawBox2D(x, y, fWidth - 4, height, imgui2_bkgCol);

  if(imgui2_val == NULL && imgui2_lbp) {
    if(imgui2_mx >= x && imgui2_mx <= x+16 && imgui2_my >= y && imgui2_my <= y+height)
      imgui2_val = edit_val;
  }

  if(edit_val == imgui2_val && imgui2_lbp) {
    value  = imgui2_my - y;
    value /= height;
    value *= (maxval-minval);
    value += minval;
	g_imgui_InDrag = 2;
  }

  value = R3D_CLAMP(value, minval, maxval);

  float curindex = height-10; // total screenspace
  curindex /= (maxval-minval); // amount per unit
  curindex *= (value-minval); // ration screensapce times units past min
    
  r3dDrawBox2D(x-2,y+curindex,16,10,r3dColor(228,228,228));

  *edit_val = value;
  return false;
}

void imgui2_StartTextEdit(const char* valtext, void* edit_val)
{
  assert(!imgui2_textEditActive);

  void imgui_MarkTextEditActive ( bool bActive, void * pDat );
  imgui_MarkTextEditActive ( true, edit_val );

  imgui2_textEditActive = true;
  imgui2_textDirty      = true;
  imgui2_textIsNumbers = false;
  imgui2_val = edit_val;
  r3dscpy(imgui2_editText, valtext);
  
  Keyboard->ClearPressed();
  win::input_Flush();
}

void imgui2_EndTextEdit()
{
	void imgui_MarkTextEditActive ( bool bActive, void * pDat );
	imgui_MarkTextEditActive ( false, NULL );
	imgui2_textEditActive = false;
	imgui2_val = NULL;
}

static bool Do_Text_Edit(float x, float y, bool bInFocus)
{
	assert(imgui2_textEditActive);

	char TempS[128];
	sprintf(TempS, "%s_", imgui2_editText);
	MenuFont_Editor->PrintF(x, y, r3dColor(255, 155, 15), TempS);

	if( (!bInFocus && imgui_lbp) || Keyboard->WasPressed(kbsEnter) || Keyboard->WasPressed(kbsNumericEnter)) {
		imgui2_EndTextEdit ();
		return true;
	}
	if(Keyboard->WasPressed(kbsEsc)) {
		imgui2_EndTextEdit ();
		return false;
	}
	if(Keyboard->WasPressed(kbsBACK)) {
		if(*imgui2_editText!='\0') {
			imgui2_textDirty = false;
			imgui2_editText[strlen(imgui2_editText)-1] = 0;
		}
	}

	if(	Keyboard->WasPressed(kbsDel) 
		)
	{
		imgui2_editText[0] = 0;
	}
	
	int ch = 0;
	if(win::kbhit()) {
	  ch = win::getch();

	  if(imgui2_textIsNumbers) {
	    if(!((ch >= '0' && ch <= '9') || ch == '.' || ch == '-'))
	      ch = 0;
	  }
	  
	  // remove control chars
	  if(ch < 0x20)
	    ch = 0;
	}
	  
	if(ch) {
		if(imgui2_textDirty) {
			r3dscpy(imgui2_editText, "");
			imgui2_textDirty = false;
		}
		sprintf(imgui2_editText + strlen(imgui2_editText), "%c", ch);
	}

	return false;
}

bool imgui2_ValueEx(float x, float y, float w, const char* name, void* edit_val, bool isFloat, float minval, float maxval, const char* fmt, int doslider, bool bUseDesktop)
{
	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( x, y, x + w, y + imgui2_ch ) )
			return false;
		x -= Desktop().GetX();
		y -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > x-g_fSizeThreshold && imgui_mx < x+w+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y+imgui2_ch+g_fSizeThreshold);

  float textx = IMGUI2_VALUENAMELEN + (w - 160);
  if(!doslider) textx = IMGUI2_VALUENAMELEN;

  imgui2_DrawBorder(x, y, w, imgui2_ch);

  MenuFont_Editor->PrintF(x+5,y+imgui2_tdy,imgui2_nameCol, name);

  r3dDrawBox2D(x+textx-4, y+4, w - (x+textx-8), imgui2_ch-8, r3dColor(0,0,0));

  char fmt2[100];
  sprintf(fmt2, "%s", fmt);
  char valtext[100];
  sprintf(valtext, fmt2, isFloat ? *(float*)edit_val : *(int*)edit_val);
  SIZE sz2;
  MenuFont_Editor->GetTextExtent(valtext, &sz2);

  // process text editing here
  if(imgui2_textEditActive && imgui2_val == edit_val) 
  {
	bool inFocus = false;
	if(!g_imgui_InDrag && imgui2_mx > x+textx && imgui2_mx < x+textx+sz2.cx && imgui2_my > y && imgui2_my < y + imgui2_ch) 
	    inFocus = true;
    if(Do_Text_Edit(x+textx, y+imgui2_tdy, inFocus)) {
      if(isFloat) *(float*)edit_val = atof(imgui2_editText);
      else        *(int*)edit_val   = atoi(imgui2_editText);
    }
    return false;
  }

  if(doslider) {
    imgui2_DrawHSliderEx2(isFloat, x+IMGUI2_VALUENAMELEN, y+4, w-170, minval, maxval, edit_val, false);
  }

  bool inFocus = false;
  if(!g_imgui_InDrag && imgui2_val == NULL && imgui2_mx > x+textx && imgui2_mx < x+textx+sz2.cx && imgui2_my > y && imgui2_my < y + imgui2_ch) 
    inFocus = true;
    
  if(inFocus) {
//    MenuFont_Editor->PrintF(x+textx,y+1,imgui2_valCol, valtext);
	r3dDrawBox2D ( x+textx-2,y+imgui2_tdy-2, sz2.cx+4, sz2.cy+4, r3dColor(120,0,0));
  }

  MenuFont_Editor->PrintF(x+textx,y+imgui2_tdy,imgui2_valCol, valtext);

  if(inFocus && imgui2_lbp && !imgui2_textEditActive) {
    imgui2_StartTextEdit(valtext, edit_val);
    imgui2_textIsNumbers = true;
  }
    
  return false;
}

bool imgui2_Value(const char* name, float* edit_val, float minval, float maxval, const char* fmt, int doslider, bool bUseDesktop) {
  imguiAddControl c;
  return imgui2_ValueEx(imgui2_stat.x, imgui2_stat.y, imgui2_stat.w, name, edit_val, true, minval, maxval, fmt, doslider, bUseDesktop);
}

bool imgui2_Value(const char* name, int* edit_val, float minval, float maxval, const char* fmt, int doslider, bool bUseDesktop) {
  imguiAddControl c;
  return imgui2_ValueEx(imgui2_stat.x, imgui2_stat.y, imgui2_stat.w, name, edit_val, false, minval, maxval, fmt, doslider, bUseDesktop);
}

bool imgui2_ListValue(const char* name, int* edit_val, const char* list[], int numlist, bool bUseDesktop) {
  imguiAddControl c;
  return imgui2_ListValueEx(imgui2_stat.x, imgui2_stat.y, imgui2_stat.w, name, edit_val, list, numlist, bUseDesktop);
}

bool imgui2_ListValueEx(float x, float y, float w, const char* name, int* edit_val, const char* list[], int numlist, bool bUseDesktop)
{
	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( x, y, x + w, y + imgui2_ch ) )
			return false;
		x -= Desktop().GetX();
		y -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > x-g_fSizeThreshold && imgui_mx < x+w+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y+imgui2_ch+g_fSizeThreshold);

  imgui2_DrawBorder(x, y, w, imgui2_ch);
  if(numlist == 0 )
	  return false;

  *edit_val = R3D_CLAMP(*edit_val, 0, numlist-1);
  
  SIZE sz1;
  MenuFont_Editor->GetTextExtent(name, &sz1);
  SIZE sz2;
  MenuFont_Editor->GetTextExtent(list[*edit_val], &sz2);

  bool inFocus = false;
  if(!g_imgui_InDrag && imgui2_val == NULL && imgui2_mx > x && imgui2_mx < x + w && imgui2_my > y && imgui2_my < y + imgui2_ch) 
    inFocus = true;

  if(inFocus) MenuFont_Editor->PrintF(x+5+1,y+imgui2_tdy+1, imgui2_nameColH, "%s", name);
  MenuFont_Editor->PrintF(x+5, y+imgui2_tdy, imgui2_nameCol, "%s", name);
  MenuFont_Editor->PrintF(x+IMGUI2_VALUENAMELEN, y+imgui2_tdy, imgui2_valCol, "%s", list[*edit_val]);
  
  if(imgui2_lbp && inFocus) {
    imgui2_val = edit_val;

    int v = *edit_val + 1;
    if(v >= numlist) {
      v = 0;
    }
    *edit_val = v;
    return true;
  }
  
  return false;
}


/*
bool Edit_Value_YN(float x, float y, const char* name, int* edit_val)
{
  const static char* list[2] = {
    "NO",
    "YES"
  };
  
  if(*edit_val != 0) {
    *edit_val = 1;
  }
  
  return Edit_Value_List(x, y, name, edit_val, list, 2);
}
*/


bool imgui2_Checkbox(const char* name, int* edit_val, const DWORD flag, bool bUseDesktop) {
  imguiAddControl c;
  return imgui2_CheckboxEx(imgui2_stat.x, imgui2_stat.y, imgui2_stat.w, name, edit_val, flag, bUseDesktop);
}

bool imgui2_CheckboxEx(float x, float y, float w, const char* name, int* edit_val, const DWORD flag, bool bUseDesktop)
{
	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( x, y, x + w, y + imgui2_ch ) )
			return false;
		x -= Desktop().GetX();
		y -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > x-g_fSizeThreshold && imgui_mx < x+w+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y+imgui2_ch+g_fSizeThreshold);

  imgui2_DrawBorder(x, y, w, imgui2_ch);

  // checkbox coords
  float cw = imgui2_ch - 10;
  float cx = x + 5; // w-2 - cw;
  float cy = y + imgui2_ch/2 - cw/2;

  MenuFont_Editor->PrintF(x+5+cw+5, y+imgui2_tdy, imgui2_nameCol, "%s", name);

  int checked = (*edit_val & flag) ? 1 : 0;

  bool inFocus = false;
  if(!g_imgui_InDrag && imgui2_val == NULL && imgui2_mx > x && imgui2_mx < (x + w) && imgui2_my > y && imgui2_my < y + imgui2_ch) 
    inFocus = true;

  if(imgui2_lbr && inFocus)
    checked =  1 - checked;

  if(inFocus)
      r3dDrawBox2D(cx, cy, cw, cw, r3dColor(255,0,0));
  else
      r3dDrawBox2D(cx, cy, cw, cw, r3dColor(255,255,255));

  if(checked)
     r3dDrawBox2D(cx+1, cy+1, cw-2, cw-2, r3dColor(0,255,0));
  else
     r3dDrawBox2D(cx+1, cy+1, cw-2, cw-2, r3dColor(0,15,0));

  *edit_val &= ~flag;
  if(checked) *edit_val |= flag;

  return false;
}

int imgui2_Checkbox_Small(float x, float y,int wid, const char* name, int* edit_val, const DWORD flag)
{
  int ht = imgui2_ch;

  imgui2_DrawBorder(x, y, wid, ht);

  SIZE sz1;
  MenuFont_Editor->GetTextExtent(name, &sz1);
  MenuFont_Editor->PrintF(x+2, y+ht/2-sz1.cy/2, imgui2_nameCol, "%s", name);


  int temp_val = (*edit_val & flag) ? 1 : 0;

  bool inFocus = false;
  if(!g_imgui_InDrag && imgui2_val == NULL && imgui2_mx > x && imgui2_mx < (x + wid) && imgui2_my > y && imgui2_my < y + ht) 
    inFocus = true;

  if(imgui2_lbr && inFocus)
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


bool imgui2_DrawColorPicker(float x, float y, const char* name, r3dColor* edit_val, float width, bool withAlpha, bool bUseDesktop) 
{
	const float ht = (25+17+17+17+17+4);

	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( x, y, x + width, y + ht ) )
			return false;
		x -= Desktop().GetX();
		y -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > x-g_fSizeThreshold && imgui_mx < x+width+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y+ht+g_fSizeThreshold);

  static float      saved_fc[4];  // current float data of color
  static const void* saved_inst = NULL; // pointer to r3dColor variable

  imguiAddControl c(ht);
 
  float fc[4];
  fc[0] = float(edit_val->R);
  fc[1] = float(edit_val->G);
  fc[2] = float(edit_val->B);
  fc[3] = float(edit_val->A);
 
  imgui2_DrawBorder(x, y, width, ht);
  
  const float dy = 25;
  MenuFont_Editor->PrintF(x+5, y+2, imgui2_nameCol, "%s (%d %d %d %d)", name, edit_val->R, edit_val->G, edit_val->B, edit_val->A);
  y += dy;
  r3dDrawBox2D(x+5, y, 50, 50, r3dColor(fc[0], fc[1], fc[2], 255.0f));
 
  if(imgui2_val == NULL) 
    saved_inst = NULL;
 
  if(saved_inst && saved_inst != edit_val) {
    // we have some other picker active - just draw sliders here
    assert(imgui2_val);
    
    imgui2_DrawHSlider(x + 60, y, width-65, 0.0f, 255.0f, &fc[0]);
    y += 17;
    imgui2_DrawHSlider(x + 60, y, width-65, 0.0f, 255.0f, &fc[1]);
    y += 17;
    imgui2_DrawHSlider(x + 60, y, width-65, 0.0f, 255.0f, &fc[2]);
    y += 17;
    if(withAlpha) {
    imgui2_DrawHSlider(x + 60, y, width-65, 0.0f, 255.0f, &fc[3]);
    y += 17;
    }
    
    return false;
  }
  
  if(saved_inst == NULL) {
    saved_fc[0] = fc[0];
    saved_fc[1] = fc[1];
    saved_fc[2] = fc[2];
    saved_fc[3] = fc[3];
  }
 
  imgui2_DrawHSlider(x + 60, y, width-65, 0.0f, 255.0f, &saved_fc[0]); if(imgui2_val == &saved_fc[0]) saved_inst = edit_val;
  y += 17;
  imgui2_DrawHSlider(x + 60, y, width-65, 0.0f, 255.0f, &saved_fc[1]); if(imgui2_val == &saved_fc[1]) saved_inst = edit_val;
  y += 17;
  imgui2_DrawHSlider(x + 60, y, width-65, 0.0f, 255.0f, &saved_fc[2]); if(imgui2_val == &saved_fc[2]) saved_inst = edit_val;
  y += 17;
  if(withAlpha) {
  imgui2_DrawHSlider(x + 60, y, width-65, 0.0f, 255.0f, &saved_fc[3]); if(imgui2_val == &saved_fc[3]) saved_inst = edit_val;
  y += 17;
  }
 
  if(saved_inst) {
    assert(saved_inst == edit_val);
    
    edit_val->R = BYTE(saved_fc[0]);
    edit_val->G = BYTE(saved_fc[1]);
    edit_val->B = BYTE(saved_fc[2]);
    edit_val->A = BYTE(saved_fc[3]);
  }
  
  return false;
}

bool imgui2_DrawColorGradient(const char* name, r3dTimeGradient2* edit_val, bool bUseDesktop)
{
  imguiAddControl c(0);
  return imgui2_DrawColorGradientEx(imgui2_stat.x, imgui2_stat.y, name, edit_val, imgui2_stat.w, bUseDesktop);
}

bool imgui2_DrawColorGradientEx(float x, float y, const char* name, r3dTimeGradient2* edit_val, float width, bool bUseDesktop)
{
	const float gradH = 24;
	const float Ht = gradH + 10 +17*3+2;

	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( x, y, x + width, y + Ht ) )
			return false;
		x -= Desktop().GetX();
		y -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > x-g_fSizeThreshold && imgui_mx < x+width+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y+Ht+g_fSizeThreshold);

  static const void* saved_inst = NULL;
  static int         saved_idx  = -1;
  static float	     dragStartTime = 999999;

  imguiAddControl c(Ht);

  imgui2_DrawBorder(x, y, width, Ht);
  
  r3dDrawBox2D(x+2, y+2, width-4, gradH, imgui2_borderCol);
  
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

    if(imgui2_mx >= (gx-4) && imgui2_mx <= (gx+4) && imgui2_my >= gy && imgui2_my <= (gy+gradH)) {
      onIdx = i;
    }
    
    if(imgui2_val == NULL && onIdx == i) {
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

  if(!g_imgui_InDrag && imgui2_mx >= x && imgui2_mx <= x+width && imgui2_my >= y && imgui2_my <= (y+gradH)) {
    inFocus = true;
  }

  if(imgui2_val == NULL && inFocus && onIdx == -1) 
    r3dDrawBox2D(imgui2_mx, y-2, 2, gradH+4, r3dColor::white);


 // draw RGB color pickers
  y += gradH + 10;

  if(saved_inst == edit_val && saved_idx != -1) {
  
    float* fc = gr.Values[saved_idx].val;

    r3dDrawBox2D(x+10, y, 17, 47, r3dColor(fc[0], fc[1], fc[2], 255.0f));

    imgui2_DrawHSlider(x + 30, y, width-30-2, 0.0f, 255.0f, &fc[0], false);
    y += 17;
    imgui2_DrawHSlider(x + 30, y, width-30-2, 0.0f, 255.0f, &fc[1], false);
    y += 17;
    imgui2_DrawHSlider(x + 30, y, width-30-2, 0.0f, 255.0f, &fc[2], false);
    y += 17;
  }

  int actmod = Keyboard->IsPressed(kbsLeftControl);
  int act1   = !g_imgui_InDrag && imgui_lbp && !actmod;
  int act2   = !g_imgui_InDrag && imgui_lbp && actmod;

 // do dragging
  if(imgui2_val == edit_val && saved_idx != -1) {
    if(!imgui2_lbp) {
      imgui2_val = NULL;
    } else if(act1 && r3dGetTime() > dragStartTime + 0.2f) { 
      // start dragging after 0.2f sec
      float time = (imgui2_mx - x) / width;
      time = R3D_CLAMP(time, 0.05f, 0.95f);
      gr.Values[saved_idx].time = time;
      gr.ResortAfterChange(&saved_idx);
    }
    
    return false;
  }
  
  if(imgui2_val == NULL) 
  {
    if(onIdx != -1 && act2) {
      // remove previously selected point
      if(onIdx != 0 && onIdx != gr.NumValues-1) {
        gr.NumValues--;
        memmove(&gr.Values[onIdx], &gr.Values[onIdx+1], sizeof(gr.Values[0]) * (gr.NumValues - onIdx));
      
        saved_inst = NULL;
        saved_idx  = -1;
        return false;
      }
    }
  
    if((imgui2_lbr && !actmod) && inFocus && onIdx == -1) {
      // add new point
      float time = (imgui2_mx - x) / width;
      
      saved_inst = edit_val;
      saved_idx  = gr.AddValue3f(time, 255, 255, 255);
    }

    if(act1 && onIdx != -1) {
      // select point
      saved_inst = edit_val;
      saved_idx  = onIdx;

      // lock this var for dragging (except corner vars)
      if(onIdx != 0 && onIdx != gr.NumValues-1) {
        imgui2_val     = edit_val;
        dragStartTime = r3dGetTime();
      }
    }

  } //if(imgui2_val == NULL) 

  return false;
}

bool imgui2_DrawFloatGradient(const char* name, r3dTimeGradient2* edit_val, float* pVertScale, float minVal, float maxVal, bool bUseDesktop ) {
  imguiAddControl c(0);
  return imgui2_DrawFloatGradientEx(imgui2_stat.x, imgui2_stat.y, name, edit_val, pVertScale, imgui2_stat.w, 200, minVal, maxVal, bUseDesktop);
}

bool imgui2_DrawFloatGradientEx(float x, float y, const char* name, r3dTimeGradient2* edit_val, float* pVertScale, float w, float h, float minVal, float maxVal, bool bUseDesktop )
{
  imguiAddControl c(h);
  static UICurveEditor curveEditor;

  g_bCursorIsOver2d |= (imgui_mx > x-g_fSizeThreshold && imgui_mx < x+w+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y+h+g_fSizeThreshold);

  curveEditor.DrawIM(x, y, w, h, name, edit_val, pVertScale,  minVal, maxVal, 10, 10, 2, 2, -1.0f, bUseDesktop);
  return false;
}

bool imgui2_Toolbar(int* edit_val, int MinIdx, const char* list[], int numlist, bool bUseDesktop) {
  float h = imgui2_dy + 10;
  imguiAddControl c(h + (imgui2_dy - imgui2_ch));
  return imgui2_ToolbarEx(imgui2_stat.x, imgui2_stat.y, imgui2_stat.w, h, edit_val, MinIdx, list, numlist, bUseDesktop);
}

bool imgui2_ToolbarEx(float sx, float sy, float sw, float sh, int *edit_val, int MinIdx, const char* list[], int numlist, bool bUseDesktop)
{
	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( sx, sy, sx + sw, sy + sh ) )
			return false;
		sx -= Desktop().GetX();
		sy -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > sx-g_fSizeThreshold && imgui_mx < sx+sw+g_fSizeThreshold && imgui_my > sy-g_fSizeThreshold && imgui_my < sy+sh+g_fSizeThreshold);


  imgui2_DrawBorder(sx, sy, sw, sh);

  SIZE sz;
  MenuFont_Editor->GetTextExtent(list[0], &sz);

  int Bw = (sw-10) / numlist-10;

  if (Bw >200) Bw = 200;

  for (int i=0;i<numlist;i++)
  {
	  int xx = sx + 5 +(Bw+10)*i;
	  int yy = sy +sh/2 - sz.cy/2-5;

	  int hover = 0;
	  if(!g_imgui_InDrag && imgui2_mx > xx && imgui2_mx < xx+Bw && imgui2_my > yy && imgui2_my < yy+sz.cy+10) {
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

	  if(imgui2_lbr && hover) {
	        *edit_val = MinIdx +i;
          }
  }

  return false;
}


bool imgui2_Button(const char* name, int selected, bool bUseDesktop) {
  imguiAddControl c;
  return imgui2_ButtonEx(imgui2_stat.x, imgui2_stat.y, imgui2_stat.w * 0.7f, imgui2_ch, name, selected, bUseDesktop);
}

bool imgui2_ButtonEx(float sx, float sy, float sw, float sh, const char* name, int selected, bool bUseDesktop)
{
	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( sx, sy, sx + sw, sy + sh ) )
			return false;
		sx -= Desktop().GetX();
		sy -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > sx-g_fSizeThreshold && imgui_mx < sx+sw+g_fSizeThreshold && imgui_my > sy-g_fSizeThreshold && imgui_my < sy+sh+g_fSizeThreshold);

  int hover = 0;
  if(!g_imgui_InDrag && imgui2_mx > sx && imgui2_mx < sx+sw && imgui2_my > sy && imgui2_my < sy+sh) {
    hover = 1;
  }
    
  r3dDrawBox2D(sx, sy, sw, sh, (imgui2_val == NULL && hover) ? r3dColor(255, 255, 255) : r3dColor24(0,0,0));

  if(selected) 
    r3dDrawBox2D(sx+2,sy+2, sw-4, sh-4, (imgui2_val == NULL && hover) ? r3dColor(200, 200, 200) : r3dColor24(160, 160, 160));
  else
    r3dDrawBox2D(sx+2,sy+2, sw-4, sh-4, (imgui2_val == NULL && hover) ? r3dColor24(100, 100, 100) : imgui2_bkgDlg);

  SIZE sz1;
  MenuFont_Editor->GetTextExtent(name, &sz1);

  //MenuFont_Editor->DrawScaledText(sx + sw/2, sy+sh/2, 1,1,1, imgui2_nameCol, name, D3DFONT_CENTERED);
  MenuFont_Editor->PrintF(floorf(sx + sw/2 - sz1.cx/2), floorf(sy+sh/2-sz1.cy/2), imgui2_nameCol, name);

  if(imgui2_val == NULL && imgui2_lbr && hover) {
    //imgui2_val = name;
    return true;
  }
  
  return false;
}

bool imgui2_StringValue(const char* name, char* value, bool allowEdit, bool bUseDesktop ) {
  imguiAddControl c;
  return imgui2_StringValueEx(imgui2_stat.x, imgui2_stat.y, imgui2_stat.w, name, value, allowEdit, bUseDesktop );
}

bool imgui2_StringValueEx(float x, float y, float w, const char* name, char* edit_val, bool allowEdit, bool bUseDesktop )
{
	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( x, y, x + w, y + imgui2_ch ) )
			return false;
		x -= Desktop().GetX();
		y -= Desktop().GetY();
	}

	g_bCursorIsOver2d |= (imgui_mx > x-g_fSizeThreshold && imgui_mx < x+w+g_fSizeThreshold && imgui_my > y-g_fSizeThreshold && imgui_my < y+imgui2_ch+g_fSizeThreshold);

  imgui2_DrawBorder(x, y, w, imgui2_ch);

  SIZE sz1;
  MenuFont_Editor->GetTextExtent(name, &sz1);
  SIZE sz2;
  MenuFont_Editor->GetTextExtent(edit_val, &sz2);
  
  bool inFocusPure = !g_imgui_InDrag && ( imgui2_mx > x && imgui2_mx < x + w && imgui2_my > y && imgui2_my < y + imgui2_ch);
  bool inFocus = imgui2_val == NULL && inFocusPure;
  
  if(inFocus) MenuFont_Editor->PrintF(x+5+1, y+imgui2_tdy+1, imgui2_nameColH, "%s", name);
  MenuFont_Editor->PrintF(x+5, y+imgui2_tdy, imgui2_nameCol, "%s", name);

  if(imgui2_textEditActive && imgui2_val == edit_val) {
    // process text editing here
    if(Do_Text_Edit(x+IMGUI2_VALUENAMELEN, y+imgui2_tdy, inFocusPure)) {
      r3dscpy(edit_val, imgui2_editText);
      return true;
    }
    return false;
  } else {
    // printf value
    MenuFont_Editor->PrintF(x+IMGUI2_VALUENAMELEN, y+imgui2_tdy, imgui2_valCol, "%s", edit_val);
  }

  if(inFocus && imgui2_lbp) {
    if(allowEdit && !imgui2_textEditActive) {
      imgui2_StartTextEdit(edit_val, edit_val);
      return false;
    }
    imgui2_val = name;
    return true;
  }
  
  return false;
}


bool imgui2_Static(const char* name, ...) {
  char buf[1024];
  
  va_list va;
  va_start(va, name);
  vsprintf(buf, name, va);
  va_end(va);

  imguiAddControl c;
  return imgui2_StaticEx(imgui2_stat.x, imgui2_stat.y, imgui2_stat.w, false, true, buf);
}

bool imgui2_Static_Checked(const char* name, ...) {
	char buf[1024];

	va_list va;
	va_start(va, name);
	vsprintf(buf, name, va);
	va_end(va);

	imguiAddControl c;
	return imgui2_StaticEx(imgui2_stat.x, imgui2_stat.y, imgui2_stat.w, true, true, buf);
}

bool imgui2_StaticEx(float x, float y, float w, bool bChecked, bool bUseDesktop, const char* name, ...)
{
	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( x, y, x + w, y + imgui2_ch ) )
			return false;
		x -= Desktop().GetX();
		y -= Desktop().GetY();
	}
	
	g_bCursorIsOver2d |= (imgui2_mx > x-g_fSizeThreshold && imgui2_mx < x+w+g_fSizeThreshold && imgui2_my > y-g_fSizeThreshold && imgui2_my < y+imgui2_ch+g_fSizeThreshold);

	int hover = 0;
	if(!g_imgui_InDrag && imgui2_mx > x && imgui2_mx < x+w && imgui2_my > y && imgui2_my < y+imgui2_ch) {
		hover = 1;
	}
	
  char buf[1024];
  
  va_list va;
  va_start(va, name);
  vsprintf(buf, name, va);
  va_end(va);

  imgui2_DrawBorder(x, y, w, imgui2_ch);

  if(hover&&bChecked) MenuFont_Editor->PrintF(x+5+1, y+imgui2_tdy+1, imgui2_nameColH, "%s", buf);
  MenuFont_Editor->PrintF(x+5, y+imgui2_tdy, imgui2_nameCol, "%s", buf);

  if(imgui2_val == NULL && imgui2_lbr && hover) {
	  return true;
  }

  return false;
}
