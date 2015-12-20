#pragma once

class CCheckSerialHelper
{
  public:
	enum {
	  CA_Unactive,
	  CA_Processing,
	};
	volatile DWORD	processCode;
	
	char		serial[128];
	
	// result from API call
	int		CheckCode;
	int		SerialType;
	char		CheckMsg[512];
	
  public:
	CCheckSerialHelper()
	{
	  serial[0]   = 0;
	  processCode = CA_Unactive;
	}
	
	int		DoCheckSerial();
};
