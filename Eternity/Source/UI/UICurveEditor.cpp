#include "r3dPCH.h"
#include "r3d.h"

#include "UI\r3dMenu.h"	// for MenuLayer
#include "UI\UICurveEditor.h"
#include "UI\UIimEdit.h"

#pragma warning (disable: 4244)

extern	const void*	imgui_val;			// current editable value. == pointer to FOCUS
extern	bool		imgui_disabled;
extern	float		imgui_mx;
extern	float		imgui_my;
extern	int		imgui_lbp;
extern	int		imgui_rbp;
extern	int		imgui_lbr;
extern	int		imgui_rbr;
extern	int		g_imgui_InDrag;

void r3dDrawVLine(float x, float y, float length, const r3dColor& clr)
{
  r3dDrawBox2D(x, y, 1, length, clr);
}

void r3dDrawHLine(float x, float y, float length, const r3dColor& clr)
{
  r3dDrawBox2D(x, y, length, 1, clr);
}

void r3dDrawLine2D(float x1, float y1, float x2, float y2, float w, const r3dColor& clr)
{
  // make vector perpendicular to the line
  r3dPoint3D v1 = r3dPoint3D(x2-x1, 0, y2-y1).NormalizeTo();
  r3dPoint3D v2 = v1.Cross(r3dPoint3D(0, 1, 0)) * (w / 2);

  R3D_SCREEN_VERTEX V[4];
  for(int i=0; i<4; i++) {
    V[i].color = clr.GetPacked();
    V[i].z     = r3dRenderer->GetNearPlaneZValue();
    V[i].rwh   = 1.0f;
  }

  V[0].x = x1-v2.x; V[0].y = y1-v2.z;
  V[1].x = x2-v2.x; V[1].y = y2-v2.z;
  V[2].x = x2+v2.x; V[2].y = y2+v2.z;
  V[3].x = x1+v2.x; V[3].y = y1+v2.z;

  r3dRenderer->Render2DPolygon(4, V);
  return;
}


	int		UICurveEditor::draggingPnt = -1;
static	float		dragSavedMin;
static	float		dragSavedMax;

	
UICurveEditor::UICurveEditor()
: vertScale( 1.f )
{
}	

UICurveEditor::~UICurveEditor()
{
}

void UICurveEditor::CalcFromCoord(const float x, const float y, float& xvar, float& yvar)
{
  // [0.0 - 1.0f]
  xvar = R3D_CLAMP((x - fx) / fw, 0.0f, 1.0f) * 1.0f;				
  // [minVal - maxVal]
  if(minMaxLocked)
    yvar = minVal + R3D_CLAMP(1.0f - ((y - fy) / fh), 0.0f, 1.0f) * (maxVal - minVal);	
  else 
    yvar = minVal + (1.0f - ((y - fy) / fh)) * (maxVal - minVal);	

  yvar /= vertScale ;
  
  assert(xvar >= 0.0f && xvar <= 1.0f);
  
  return;
}

void UICurveEditor::CalcFromValue(const r3dBezierGradient::val_s& gv, float& x, float &y)
{
	x = fx + fw * (gv.time / 1.0f);
	y = fy + fh * (1.0f - ((gv.val[0] * vertScale - minVal ) / (maxVal - minVal)) )  ;

	return;
}

void UICurveEditor::CalcFromValue(const r3dTimeGradient2::val_s& gv, float& x, float &y)
{
  x = fx + fw * (gv.time / 1.0f);
  y = fy + fh * (1.0f - ((gv.val[0] * vertScale - minVal) / (maxVal - minVal))  )  ;
  
  return;
}

void UICurveEditor::DrawFloatCurve(const r3dTimeGradient2& grad )
{
	static const r3dColor clr_curve(255, 255, 255);
	static const float    line_w  = 2;

	if( grad.Smooth )
	{
		int subDivCount = grad.NumValues * 16;

		float prevX, prevY;

		r3dTimeGradient2::val_s val;

		val.time	= 0; 
		val.val[0]	= grad.GetFloatValue( val.time );

		CalcFromValue( val, prevX, prevY );

		for( int i = 1 ; i < subDivCount; i++ ) 
		{
			r3dTimeGradient2::val_s val;

			val.time	= float( i ) / ( subDivCount - 1 ); 
			val.val[0]	= grad.GetFloatValue( val.time );

			float x, y;
			CalcFromValue( val, x, y );

			float drawPrevX = prevX ;
			float drawPrevY = prevY ;
			float drawX = x ;
			float drawY = y ;

			if( vertScale > 1.0f )
			{
				r3dError( "Implement proper clipping! ( like in the branch below! ) " ) ;
			}
			
			r3dDrawLine2D(drawPrevX, drawPrevY, drawX, drawY, line_w, clr_curve);

			prevX = x;
			prevY = y;
		}
	}
	else
	{
		// just draw lines from-to control points
		for(int i=0; i<grad.NumValues-1; i++) 
		{
			// get screen coords for current and next gradient value
			float x1, y1;
			CalcFromValue(grad.Values[i], x1, y1 );
			float x2, y2;
			CalcFromValue(grad.Values[i+1], x2, y2);

			float drawX1 = x1 ;
			float drawY1 = y1 ;
			float drawX2 = x2 ;
			float drawY2 = y2 ;

			int numClampedUp = 0, numClampedDown = 0 ;

			// clip

			struct 
			{
				int operator() (  float* drawX, float* drawY, float x1, float x2, float y1, float y2, float fy, float csign )
				{
					if( ( *drawY - fy ) * csign < 0 )
					{
						if( fabs( y2 - y1 ) > FLT_EPSILON )
						{
							*drawX = *drawX + ( fy - *drawY ) * ( x2 - x1 ) / ( y2 - y1 ) ;
						}

						*drawY = fy ;

						return 1 ;
					}

					return 0 ;
				}
			} clip;

			numClampedUp += clip( &drawX1, &drawY1, x1, x2, y1, y2, fy, 1.f ) ;
			numClampedUp += clip( &drawX2, &drawY2, x1, x2, y1, y2, fy, 1.f ) ;

			numClampedDown += clip( &drawX1, &drawY1, x1, x2, y1, y2, fy + fh, -1.f ) ;
			numClampedDown += clip( &drawX2, &drawY2, x1, x2, y1, y2, fy + fh, -1.f ) ;

#if 0
			float fyd = fy + fh ;

			if( drawY1 > fyd )
			{
				drawY1 = fyd ;

				if( fabs( y2 - y1 ) > FLT_EPSILON )
				{
					drawX1 = x1 + ( fyd - y1 ) * ( x2 - x1 ) / ( y2 - y1 ) ;
				}

				numClampedDown ++ ;
			}
#endif

			if( numClampedUp < 2 && numClampedDown < 2 )
			{
				r3dDrawLine2D(drawX1, drawY1, drawX2, drawY2, line_w, clr_curve);
			}
		}
	}

	r3dRenderer->Flush();

	return;  
}


void UICurveEditor::DrawBezierCurve(const r3dBezierGradient& grad)
{
	static const r3dColor clr_curve(255, 255, 255);
	static const float    line_w  = 2;

	int subDivCount = grad.NumValues * 16;

	float prevX, prevY;

	r3dBezierGradient::val_s val;

	val.time	= 0; 
	val.val[0]	= grad.GetFloatValue( val.time );

	CalcFromValue( val, prevX, prevY );

	for( int i = 1 ; i < subDivCount; i++ ) 
	{
		r3dBezierGradient::val_s val;

		val.time	= float( i ) / ( subDivCount - 1 ); 
		val.val[0]	= grad.GetFloatValue( val.time );

		float x, y;
		CalcFromValue( val, x, y );

		r3dDrawLine2D(prevX, prevY, x, y, line_w, clr_curve);

		prevX = x;
		prevY = y;
	}

	r3dRenderer->Flush();

	return;  
}

void UICurveEditor::DrawFloatPoints(const r3dTimeGradient2& grad, bool useSelect)
{
  static const r3dColor clr_pnts(0, 255, 0);
  static const r3dColor clr_selpnts(255, 0, 0);
  
  selectedPnt = -1;
  for(int i=0; i<grad.NumValues; i++) 
  {
    // get screen coords for current gradient value
    float x, y;
    CalcFromValue(grad.Values[i], x, y);

	// clip ( only y cause there's only vertical scale)
	if( y < fy || y > fy + fh )
		continue ;

    if(!g_imgui_InDrag && useSelect && fabs(x - imgui_mx) < 5 && fabs(y - imgui_my) < 5) {
      selectedPnt = i;
    }
    
    if(i == selectedPnt || (imgui_val == &grad && i == draggingPnt)) {
      r3dDrawBox2D(x-5, y-5, 10, 10, clr_selpnts);
    }

    r3dDrawBox2D(x-3, y-3, 6, 6, clr_pnts);
  }

  return;  
}


void UICurveEditor::DrawBezierPoints(const r3dBezierGradient& grad, bool useSelect)
{
	static const r3dColor clr_pnts(0, 255, 0);
	static const r3dColor clr_selpnts(255, 0, 0);

	selectedPnt = -1;
	for(int i=0; i<grad.NumValues; i++) 
	{
		// get screen coords for current gradient value
		float x, y;
		CalcFromValue(grad.Values[i], x, y);

		if(!g_imgui_InDrag && useSelect && fabs(x - imgui_mx) < 5 && fabs(y - imgui_my) < 5) {
			selectedPnt = i;
		}

		if(i == selectedPnt || (imgui_val == &grad && i == draggingPnt)) {
			r3dDrawBox2D(x-5, y-5, 10, 10, clr_selpnts);
		}

		r3dDrawBox2D(x-3, y-3, 6, 6, clr_pnts);

		if ( i<grad.NumValues - 1 )
		{
			float x2, y2;
			CalcFromValue(grad.Values[i+1], x2, y2);

			float x1, y1;

			CalcFromValue(grad.FirstControlPoints[i], x1, y1);
			r3dDrawBox2D(x1-3, y1-3, 6, 6, r3dColor::red);
			r3dDrawLine2D ( x,y, x1,y1, 2.f, r3dColor::red );
			
			CalcFromValue(grad.SecondControlPoints[i], x1, y1);
			r3dDrawBox2D(x1-3, y1-3, 6, 6, r3dColor::blue);
			r3dDrawLine2D ( x2,y2, x1,y1, 2.f, r3dColor::blue );
		}
	}

	return;  
}

void UICurveEditor::DrawIM(	float in_x, float in_y, float in_w, float in_h, const char *Title, r3dTimeGradient2* edit_val, float* pVertScale,
							float in_minVal, float in_maxVal, int x_steps /*= 10*/, int y_steps /*= 10*/,
							int x_precision /*= 2*/, int y_precision /*= 2*/, float currentTime /*= -1.0f*/, bool bUseDesktop )
{
	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( in_x, in_y, in_x + in_w, in_y + in_h ) )
			return;
		in_x -= Desktop().GetX();
		in_y -= Desktop().GetY();
	}  

  const int   x_dels    = x_steps;
  const int   y_dels    = y_steps;

  static const float off_field  = 30;
  static const float off_header = 20;

  static const r3dColor clr_bg(50, 50, 50);
  static const r3dColor clr_hdr(80, 120, 80);
  static const r3dColor clr_fld(30, 30, 30);
  static const r3dColor clr_line(100, 100, 100);
  static const r3dColor clr_selline(200, 200, 200);

  float ui_x = in_x;
  float ui_y = in_y;
  float ui_w = in_w;
  float ui_h = in_h - 8;

  // field coords
  fx = ui_x + off_field; 
  fw = ui_w - off_field - off_field; 
  fy = ui_y + off_header; 
  fh = ui_h - off_field - off_header;

  // draw backgoround
  r3dDrawBox2D(ui_x, ui_y, ui_w, in_h, clr_bg);
  r3dDrawBox2D(fx, fy, fw, fh, clr_fld);
  r3dDrawBox2D(ui_x, ui_y, ui_w, off_header-2, clr_hdr);

  if( pVertScale )
  {
	static float tempVal = 0.f ;

	tempVal = 33.f - *pVertScale ;
	tempVal = R3D_MAX( R3D_MIN( tempVal, 32.f), 1.f ) ;
	imgui_DrawVSlider( ui_x + ui_w - 22, ui_y + 22, fh - 22, 1.f, 32.f, &tempVal, bUseDesktop ) ;

	vertScale = *pVertScale = 33.f - tempVal ;	
  }
  else
  {
	vertScale = 1.f ;
  }

  r3dTimeGradient2& grad = *edit_val;
 
  if(in_minVal > -9999) {
    minMaxLocked = true;
    minVal = in_minVal;
    maxVal = in_maxVal;

    // apply min/max limits
    float min = R3D_MIN(minVal, maxVal);
    float max = R3D_MAX(minVal, maxVal);
    for(int i=0; i<grad.NumValues; i++) {
      r3dTimeGradient2::val_s& gv = grad.Values[i];
    
      gv.val[0] = R3D_CLAMP(gv.val[0], min, max);
    }
  } else { 
    minMaxLocked = false;

    // unrestricted mode - detect min/max
    minVal = 9999;
    maxVal = -9999;
    for(int i=0; i<grad.NumValues; i++) {
      r3dTimeGradient2::val_s& gv = grad.Values[i];

      if(minVal > gv.val[0]) minVal = gv.val[0];
      if(maxVal < gv.val[0]) maxVal = gv.val[0];
    }
    minVal -= 1.0f;
    maxVal += 1.0f;
  
    if(imgui_val == edit_val) {
      // do not change min/val while dragging!
      minVal = dragSavedMin;
      maxVal = dragSavedMax;
    }
  }

  MenuFont_Editor->PrintF(ui_x+5, ui_y+2, r3dColor(255,255,255), Title);

  x_precision = R3D_MIN( R3D_MAX( x_precision, 0 ), 9 );
  y_precision = R3D_MIN( R3D_MAX( y_precision, 0 ), 9 );

  char num_fmt[] = "%.0f";
  const int fmt_precision_pos = 2;

  // draw x lines - from 0.0 to 1.0
  {
    float x  = fx;
    float y  = fy;
    float dx = fw / x_dels;

	num_fmt[fmt_precision_pos] = '0' + x_precision;
  
    for(int i=0; i<=x_dels; i++) {
      r3dDrawVLine(x, y, ui_h - off_header, clr_line);
      MenuFont_Editor->PrintF(x+2, y+fh+2, r3dColor::white, num_fmt, float(i) / x_dels);
      x += dx;
    }
  }

  // draw y lines - from minVal to maxVal
  {
    float x   = float(ui_x);
    float y   = fy;
    float dy  = fh / y_dels;
    float val = maxVal;
    float vd  = (minVal - maxVal) / y_dels;

	num_fmt[fmt_precision_pos] = '0' + y_precision;
  
    for(int i=0; i<=y_dels; i++) {
      r3dDrawHLine(x, y, ui_w, clr_line);
      MenuFont_Editor->PrintF(x, y, r3dColor::white, num_fmt, val / vertScale );
      y += dy;
      val += vd;
    }
  }
 
  DrawFloatCurve(*edit_val);
  DrawFloatPoints(*edit_val, imgui_val == NULL);

  if( currentTime >= 0.f && currentTime <= 1.0f )
  {
    float lx = fx + fw * currentTime;
	r3dDrawVLine( lx, fy, fh, r3dColor::green );
  }

  if(imgui_disabled)
    return;
  if(imgui_val && imgui_val != edit_val)
    return;

  // draw mouseover lines && current coordinate/value
  if(!g_imgui_InDrag && imgui_mx >= fx && imgui_mx <= fx+fw && imgui_my >= fy && imgui_my <= fy+fh && (imgui_val == NULL || imgui_val == edit_val)) {
    r3dDrawHLine(fx, imgui_my, fw, clr_selline);
    r3dDrawVLine(imgui_mx, fy, fh, clr_selline);

    if(selectedPnt == -1) {
      float vx, vy;
      CalcFromCoord(imgui_mx, imgui_my, vx, vy);
      MenuFont_Editor->PrintF(imgui_mx+2, imgui_my+2, r3dColor::white, "----Time:%.2f Val:%.4f", vx, vy);
    }
  }
  
  // if we snapped on point - print it's coordinate/value
  if(selectedPnt != -1) {
    const r3dTimeGradient2::val_s& gv = grad.Values[selectedPnt];
    float x, y;
    CalcFromValue(gv, x, y);

    MenuFont_Editor->PrintF(x+2, y+2, r3dColor::white, "****Time:%.2f Val:%.4f", gv.time, gv.val[0]);
    MenuFont_Editor->PrintF(ui_x+0, ui_y-2, r3dColor::white, "[%.2f]:%.4f", gv.time, gv.val[0]);
  }
  
  bool act1 = !g_imgui_InDrag && imgui_lbp && !Keyboard->IsPressed(kbsLeftControl);
  bool act2 = !g_imgui_InDrag && imgui_lbp && Keyboard->IsPressed(kbsLeftControl);
  
 // 
 // point dragging
 //
  if(imgui_val == NULL && selectedPnt != -1 && imgui_val != this && act1) {
    // enable point dragging
    imgui_val   = edit_val;
    draggingPnt = selectedPnt;
    dragSavedMin= minVal;
    dragSavedMax= maxVal;
  }
  
  if(imgui_val == edit_val) {
    assert(draggingPnt != -1);
    // update dragging point
    float vx, vy;
    CalcFromCoord(imgui_mx, imgui_my, vx, vy);
    
    r3dTimeGradient2::val_s& gv = grad.Values[draggingPnt];
    if(gv.time < 0.001f || gv.time > 0.999f) {
      // do not edit value for border params
      gv.val[0] = vy;  
    } else {
      gv.time   = R3D_CLAMP(vx, 0.01f, 0.98f);
      gv.val[0] = vy;
      
      grad.ResortAfterChange(&selectedPnt);
    }
    
    if(imgui_lbr) {
      imgui_val   = NULL;
      draggingPnt = -1;
    }
    
    return;
  }

 //
 // point adding/removing
 //
  if(imgui_mx >= fx && imgui_mx <= fx+fw && imgui_my >= fy && imgui_my <= fy+fh) 
  {
    assert(imgui_val == NULL);
  
    if(selectedPnt != -1 && act2) {
      // remove selected point (only not border ones)
      if(selectedPnt != 0 && selectedPnt != grad.NumValues-1) {

        grad.NumValues--;
        memmove(&grad.Values[selectedPnt], &grad.Values[selectedPnt+1], sizeof(grad.Values[0]) * (grad.NumValues - selectedPnt));
        
        selectedPnt = -1;
        return;
      }
    }
    
    if(selectedPnt == -1 && act1) {
      // add new control point and lock it for dragging
      float vx, vy;
      CalcFromCoord(imgui_mx, imgui_my, vx, vy);
      
      draggingPnt = grad.AddValue(vx, vy);
      dragSavedMin= minVal;
      dragSavedMax= maxVal;
      imgui_val   = edit_val;
      return;
    }
  }

  return;
}


void UICurveEditor::DrawBezier(	float in_x, float in_y, float in_w, float in_h, const char *Title, r3dBezierGradient* edit_val, float* pVertScale,
						   float in_minVal, float in_maxVal, int x_steps /*= 10*/, int y_steps /*= 10*/,
						   int x_precision /*= 2*/, int y_precision /*= 2*/, bool bUseDesktop )
{
	if ( bUseDesktop )
	{
		if ( ! Desktop().IsWindowVisible( in_x, in_y, in_x + in_w, in_y + in_h ) )
			return;
		in_x -= Desktop().GetX();
		in_y -= Desktop().GetY();
	}


	const int   x_dels    = x_steps;
	const int   y_dels    = y_steps;

	static const float off_field  = 30;
	static const float off_header = 20;

	static const r3dColor clr_bg(50, 50, 50);
	static const r3dColor clr_hdr(80, 120, 80);
	static const r3dColor clr_fld(30, 30, 30);
	static const r3dColor clr_line(100, 100, 100);
	static const r3dColor clr_selline(200, 200, 200);

	float ui_x = in_x;
	float ui_y = in_y;
	float ui_w = in_w;
	float ui_h = in_h - 8;

	// field coords
	fx = ui_x + off_field; 
	fw = ui_w - off_field - off_field; 
	fy = ui_y + off_header; 
	fh = ui_h - off_field - off_header;

	r3dBezierGradient& grad = *edit_val;

	if( pVertScale )
	{
		r3d_assert( "Copy me from UICurveEditor::DrawIM!!" );
	}
	else
	{
		vertScale = 1.f ;
	}

	if(in_minVal > -9999) {
		minMaxLocked = true;
		minVal = in_minVal;
		maxVal = in_maxVal;

		// apply min/max limits
		float min = R3D_MIN(minVal, maxVal);
		float max = R3D_MAX(minVal, maxVal);
		for(int i=0; i<grad.NumValues; i++) {
			r3dBezierGradient::val_s& gv = grad.Values[i];

			gv.val[0] = R3D_CLAMP(gv.val[0], min, max);
		}
	} else { 
		minMaxLocked = false;

		// unrestricted mode - detect min/max
		minVal = 9999;
		maxVal = -9999;
		for(int i=0; i<grad.NumValues; i++) {
			r3dBezierGradient::val_s& gv = grad.Values[i];

			if(minVal > gv.val[0]) minVal = gv.val[0];
			if(maxVal < gv.val[0]) maxVal = gv.val[0];
		}
		minVal -= 1.0f;
		maxVal += 1.0f;

		if(imgui_val == edit_val) {
			// do not change min/val while dragging!
			minVal = dragSavedMin;
			maxVal = dragSavedMax;
		}
	}

	// draw backgoround
	r3dDrawBox2D(ui_x, ui_y, ui_w, in_h, clr_bg);
	r3dDrawBox2D(fx, fy, fw, fh, clr_fld);
	r3dDrawBox2D(ui_x, ui_y, ui_w, off_header-2, clr_hdr);

	MenuFont_Editor->PrintF(ui_x+5, ui_y+2, r3dColor(255,255,255), Title);

	x_precision = R3D_MIN( R3D_MAX( x_precision, 0 ), 9 );
	y_precision = R3D_MIN( R3D_MAX( y_precision, 0 ), 9 );

	char num_fmt[] = "%.0f";
	const int fmt_precision_pos = 2;

	// draw x lines - from 0.0 to 1.0
	{
		float x  = fx;
		float y  = fy;
		float dx = fw / x_dels;

		num_fmt[fmt_precision_pos] = '0' + x_precision;

		for(int i=0; i<=x_dels; i++) {
			r3dDrawVLine(x, y, ui_h - off_header, clr_line);
			MenuFont_Editor->PrintF(x+2, y+fh+2, r3dColor::white, num_fmt, float(i) / x_dels);
			x += dx;
		}
	}

	// draw y lines - from minVal to maxVal
	{
		float x   = float(ui_x);
		float y   = fy;
		float dy  = fh / y_dels;
		float val = maxVal;
		float vd  = (minVal - maxVal) / y_dels;

		num_fmt[fmt_precision_pos] = '0' + y_precision;

		for(int i=0; i<=y_dels; i++) {
			r3dDrawHLine(x, y, ui_w, clr_line);
			MenuFont_Editor->PrintF(x, y, r3dColor::white, num_fmt, val);
			y += dy;
			val += vd;
		}
	}

	DrawBezierCurve(*edit_val );
	DrawBezierPoints(*edit_val, imgui_val == NULL);

	if(imgui_disabled)
		return;
	if(imgui_val && imgui_val != edit_val)
		return;

	// draw mouseover lines && current coordinate/value
	if(!g_imgui_InDrag && imgui_mx >= fx && imgui_mx <= fx+fw && imgui_my >= fy && imgui_my <= fy+fh && (imgui_val == NULL || imgui_val == edit_val)) {
		r3dDrawHLine(fx, imgui_my, fw, clr_selline);
		r3dDrawVLine(imgui_mx, fy, fh, clr_selline);

		if(selectedPnt == -1) {
			float vx, vy;
			CalcFromCoord(imgui_mx, imgui_my, vx, vy);
			MenuFont_Editor->PrintF(imgui_mx+2, imgui_my+2, r3dColor::white, "----Time:%.2f Val:%.4f", vx, vy);
		}
	}

	// if we snapped on point - print it's coordinate/value
	if(selectedPnt != -1) {
		const r3dBezierGradient::val_s& gv = grad.Values[selectedPnt];
		float x, y;
		CalcFromValue(gv, x, y);

		MenuFont_Editor->PrintF(x+2, y+2, r3dColor::white, "****Time:%.2f Val:%.4f", gv.time, gv.val[0]);
		MenuFont_Editor->PrintF(ui_x+0, ui_y-2, r3dColor::white, "[%.2f]:%.4f", gv.time, gv.val[0]);
	}

	bool act1 = !g_imgui_InDrag && imgui_lbp && !Keyboard->IsPressed(kbsLeftControl);
	bool act2 = !g_imgui_InDrag && imgui_lbp && Keyboard->IsPressed(kbsLeftControl);

	// 
	// point dragging
	//
	if(imgui_val == NULL && selectedPnt != -1 && imgui_val != this && act1) {
		// enable point dragging
		imgui_val   = edit_val;
		draggingPnt = selectedPnt;
		dragSavedMin= minVal;
		dragSavedMax= maxVal;
	}

	if(imgui_val == edit_val) {
		assert(draggingPnt != -1);
		// update dragging point
		float vx, vy;
		CalcFromCoord(imgui_mx, imgui_my, vx, vy);

		r3dBezierGradient::val_s& gv = grad.Values[draggingPnt];
		if(gv.time < 0.001f || gv.time > 0.999f) {
			// do not edit value for border params
			gv.val[0] = vy;  
		} else {
			gv.time   = R3D_CLAMP(vx, 0.01f, 0.98f);
			gv.val[0] = vy;

			grad.ResortAfterChange(&selectedPnt);
		}

		if(imgui_lbr) {
			imgui_val   = NULL;
			draggingPnt = -1;
		}

		return;
	}

	//
	// point adding/removing
	//
	if(imgui_mx >= fx && imgui_mx <= fx+fw && imgui_my >= fy && imgui_my <= fy+fh) 
	{
		assert(imgui_val == NULL);

		if(selectedPnt != -1 && act2) {
			// remove selected point (only not border ones)
			if(selectedPnt != 0 && selectedPnt != grad.NumValues-1) {

				grad.NumValues--;
				memmove(&grad.Values[selectedPnt], &grad.Values[selectedPnt+1], sizeof(grad.Values[0]) * (grad.NumValues - selectedPnt));

				grad.UpdateControlPoints ();
				selectedPnt = -1;
				return;
			}
		}

		if(selectedPnt == -1 && act1) {
			// add new control point and lock it for dragging
			float vx, vy;
			CalcFromCoord(imgui_mx, imgui_my, vx, vy);

			draggingPnt = grad.AddValue(vx, vy);
			dragSavedMin= minVal;
			dragSavedMax= maxVal;
			imgui_val   = edit_val;
			return;
		}
	}

	return;
}
