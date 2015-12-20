#pragma once

#include "HWInfo.h"

class CHWInfoPoster
{
  public:
	CHWInfo		hw;
	HANDLE		postHwInfoH_;
	bool		NeedUploadReport();

  public:
	CHWInfoPoster();
	~CHWInfoPoster();
	
	void		Start();
	void		Stop();
};

extern CHWInfoPoster	gHwInfoPoster;