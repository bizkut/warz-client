#pragma once

class CCreateAccHelper
{
  public:
	enum {
	  CA_Unactive,
	  CA_Processing,
	};
	volatile DWORD	createAccCode;
	
	char		username[128];
	char		passwd1[128];
	char		passwd2[128];
	char		serial[128];	// serial key
	
  public:
	CCreateAccHelper()
	{
	  username[0]   = 0;
	  passwd1[0]    = 0;
	  passwd2[0]    = 0;
	  serial[0]     = 0;
	  createAccCode = CA_Unactive;
	}
	
	int		DoCreateAcc();
};
