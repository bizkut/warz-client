#pragma once

class obj_Road;

struct Editor_Road
{
  public:
	float		SliderX;
	float		SliderY;

	enum {
	  TAB_SELECT,
	  TAB_EDIT,
	};
	int		curTab_;
	gobjid_t	roadId_;

	int		roadAdding_;
	r3dPoint3D	roadPnt_[2];
	
	enum {
	  POINT_MODIFY,
	  POINT_ADD,
	};
	int		pointMode_;
	int		addDir_;
	int		curPoint_;
	int		isNormal_;
	
	bool		isDragging_;
	int		dragPoint_;
	
	int		drawRoadConfig;
	int		drawRoadNormal;
	
	float		lastRegenTime_;
	float		regenDelay_;

  public:
	Editor_Road();
	~Editor_Road();

	void		StartDrawHelpers();
	void		StopDrawHelpers();

	void		ProcessTab(float in_SliderX, float in_SliderY);
	void		 ProcessAdd();
	void		 ProcessRegenerate();
	void		 ProcessSelect();
	void		 ProcessEdit();
	void		  ProcessEditMaterials();
};
