#pragma once

#include "TimeGradient.h"
#include "BezierGradient.h"

class MenuLayer;

class UICurveEditor
{
  private:
	// field area
	float		fx;
	float		fy;
	float		fw;
	float		fh;
	
	float		minVal;
	float		maxVal;
	bool		minMaxLocked;

	float		vertScale ;

	int		selectedPnt;
static	int		draggingPnt;	

	void		DrawFloatCurve(const r3dTimeGradient2& grad);
	void		DrawFloatPoints(const r3dTimeGradient2& grad, bool useSelect);
	
	void		DrawBezierCurve(const r3dBezierGradient& grad);
	void		DrawBezierPoints(const r3dBezierGradient& grad, bool useSelect);

	void		CalcFromCoord(const float x, const float y, float& xvar, float& yvar);		// return gradient data for specified screen corrds
	void		CalcFromValue(const r3dTimeGradient2::val_s& gv, float& x, float &y);		// return screen coords for specified gradient value
	void		CalcFromValue(const r3dBezierGradient::val_s& gv, float& x, float &y);		// return screen coords for specified gradient value

  public:
	UICurveEditor();
	~UICurveEditor();
	void		DrawIM(float in_x, float in_y, float in_w, float in_h, const char *Title, r3dTimeGradient2* edit_val, float* pVertScale, float in_minVal, float in_maxVal, int x_steps = 10, int y_steps = 10, int x_precision = 2, int y_precision = 2, float currentTime = -1.0f, bool bUseDesktop = true  );
	void		DrawBezier(float in_x, float in_y, float in_w, float in_h, const char *Title, r3dBezierGradient* edit_val, float* pVertScale, float in_minVal, float in_maxVal, int x_steps = 10, int y_steps = 10, int x_precision = 2, int y_precision = 2, bool bUseDesktop = true  );
};

