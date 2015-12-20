#include "r3dPCH.h"
#include "r3d.h"

#include "WOCheckSerialHelper.h"
#include "WOBackendAPI.h"

int CCheckSerialHelper::DoCheckSerial()
{
	r3d_assert(processCode == CA_Unactive);
	r3d_assert(*serial);

	CWOBackendReq req("api_AccCheckKey.aspx");
	req.AddParam("email",    "not@used.anymore"); // previously that was order email.
	req.AddParam("serial",   serial);

	processCode = CA_Processing;
	req.Issue();
	processCode = CA_Unactive;

	if(req.resultCode_ != 0)
		return 0;

        // parse from Response.Write(string.Format("{0} {1} :{2}", CheckCode, SerialType, CheckMsg));
	CheckCode  = 99;
	SerialType = -1;
	CheckMsg[0]=0;
	sscanf(req.bodyStr_, "%d %d", &CheckCode, &SerialType);
	if(strchr(req.bodyStr_, ':')) r3dscpy(CheckMsg, strchr(req.bodyStr_, ':') + 1);
	return 1;
}
