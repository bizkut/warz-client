// Copyright (C) 2001-2003 Even Balance, Inc.
//
//
// pbsv.cpp
//
// EVEN BALANCE - T.RAY
//

#ifdef __WITH_PB__
#define _cplusplus

#define GETPBSDKPOINTER

#include "pbsdk.h"
#include "pbmd5.h"



#ifdef __WITH_PB__



//
// Functions and wrappers 
//   these are declared in pbcommon.h

extern "C" {

void PbSvAddEvent ( int event , int clientIndex , int datalen , char *data )
{
	if ( pbsdk == NULL ) return ;
	pbsdk->pbsv.AddPbEvent ( event , clientIndex , datalen , data , 0 ) ;
}

void PbServerInitialize ( void )
{
	static int inited = 0 ;
	if ( inited ) return ;
	if ( pbsdk == NULL ) return ;
	inited = 1 ;
	pbsdk->pbsv.initialize() ;

	pbsdk->pb_getBasePath ( pbsdk->pbsv.m_basepath , PB_Q_MAXRESULTLEN ) ;
	pbsdk->pb_getHomePath ( pbsdk->pbsv.m_homepath , PB_Q_MAXRESULTLEN ) ;

	PbSvAddEvent ( PB_EV_CONFIG , -1 , 0 , "" ) ;
	if ( pbsdk->pbsv.m_AddPbEvent == NULL ) pbsdk->pb_SetSvPunkBuster ( "0" ) ;
}

void PbServerProcessEvents ( void )
{
	if ( pbsdk == NULL ) return ;
	PbServerInitialize() ;
	pbsdk->pbsv.ProcessPbEvents() ;
}

void PbServerForceProcess ( void )
{
	if ( pbsdk == NULL ) return ;
	pbsdk->pbsv.ProcessPbEvents ( -1 ) ;
}

void PbServerCompleteCommand ( char *buf , int buflen )
{
	if ( pbsdk == NULL ) return ;
	pbsdk->pbsv.AddPbEvent ( PB_EV_CMDCOMPL , -1 , buflen , buf ) ;
}

void PbPassConnectString ( char *fromAddr , char *connectString )
{
	if ( pbsdk == NULL ) return ;
	if ( pbsdk->pbsv.m_PassConnectString == NULL ) return ;//means PB not installed/enabled
	pbsdk->pbsv.m_PassConnectString ( &pbsdk->pbsv , fromAddr , connectString ) ;
}

char *PbAuthClient ( char *fromAddr , int cl_pb , char *cl_guid )
{
	if ( pbsdk == NULL ) return NULL ;
	if ( pbsdk->pbsv.m_AuthClient == NULL ) return NULL ;//means PB not installed/enabled

	char guid[33] ;
	memset ( guid , 0 , 33 ) ;
	strncpy ( guid , cl_guid, 32 ) ;
	MD5_CTX m ;
	MD5Init ( &m , 73827 ) ;
	MD5Update ( &m , (unsigned char *) guid , 32 ) ;
	MD5Final ( &m ) ;
	sprintf ( guid , "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x" ,
		m.digest[0] , m.digest[1] , m.digest[2] , m.digest[3] , m.digest[4] , m.digest[5] , m.digest[6] , 
		m.digest[7] , m.digest[8] , m.digest[9] , m.digest[10] , m.digest[11] , m.digest[12] , m.digest[13] , 
		m.digest[14] , m.digest[15] ) ;
	MD5_CTX m2 ;
	MD5Init ( &m2 , 72837 ) ;
	MD5Update ( &m2 , (unsigned char *) guid , 32 ) ;
	MD5Final ( &m2 ) ;
	sprintf ( guid , "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x" ,
		m2.digest[0] , m2.digest[1] , m2.digest[2] , m2.digest[3] , m2.digest[4] , m2.digest[5] , m2.digest[6] , 
		m2.digest[7] , m2.digest[8] , m2.digest[9] , m2.digest[10] , m2.digest[11] , m2.digest[12] , m2.digest[13] , 
		m2.digest[14] , m2.digest[15] ) ;
	return pbsdk->pbsv.m_AuthClient ( &pbsdk->pbsv , fromAddr , cl_pb , guid ) ;
}

int isPbSvEnabled ( void )
{
	if ( pbsdk == NULL ) return 0 ;
	return (int) pbsdk->pbsv.AddPbEvent ( PB_EV_ISENABLED , -1 , 0 , NULL ) ;
}

void EnablePbSv ( void )
{
	if ( pbsdk == NULL ) return ;
	pbsdk->pbsv.AddPbEvent ( PB_EV_ENABLE , -1 , 0 , NULL ) ;
}

void DisablePbSv ( void )
{
	if ( pbsdk == NULL ) return ;
	pbsdk->pbsv.AddPbEvent ( PB_EV_DISABLE , -1 , 0 , NULL ) ;
}

} //extern "C"

extern void PbClientTrapConsole ( char *msg , int msglen ) ;//in pbcl.cpp
void PbCaptureConsoleOutput ( char *msg , int msglen )
{
	if ( pbsdk == NULL ) {
		return;
	}

#if !defined( ID_DEDICATED )
	PbClientTrapConsole( msg , msglen );
#endif

	if ( pbsdk->pbsv.m_TrapConsole != NULL ) pbsdk->pbsv.m_TrapConsole ( &pbsdk->pbsv , msg , msglen ) ;

	if ( pbsdk->ConsoleCaptureBuf == NULL ) return ;
	int sl = strlen ( pbsdk->ConsoleCaptureBuf ) ;
	if ( sl + (int) strlen ( msg ) >= pbsdk->ConsoleCaptureBufLen ) return ;
	strcpy ( pbsdk->ConsoleCaptureBuf + sl , msg ) ;
}


//
// PbSvGameCommand
//
char *PbSvGameCommand ( char *Cmd , char *Result )
{
	if ( pbsdk == NULL ) return NULL ;
	if ( !_stricmp ( Cmd , "set_sv_punkbuster" ) ) pbsdk->pb_SetSvPunkBuster ( Result ) ;
	else if ( !_stricmp ( Cmd , "ConCapBufLen" ) ) pbsdk->ConsoleCaptureBufLen = (long) atol ( Result ) ;
	else if ( !_stricmp ( Cmd , "ConCapBuf" ) ) {
		pbsdk->ConsoleCaptureBuf = Result ;
	}
	else if ( !_stricmp ( Cmd , "Cmd_Exec" ) ) {
		if ( !_strnicmp ( Result, "Cmd_ForceTeam", 13 ) ) {
			char *cp = Result ;
			char *arg1, *arg2;

			while ( *cp && *cp != ' ' ) ++cp ;
			*cp = 0;
			++cp;
			while ( *cp && *cp == ' ' ) ++cp ;
			arg1 = cp;
			while ( *cp && *cp != ' ' ) ++cp ;		
			arg2 = cp;

			int slot, team;
			slot = atoi ( arg1 ) ;
			team = atoi ( arg2 ) ;

			pbsdk->pb_ForceTeam ( slot, team ) ;

		}
		else {
			int pb = !_strnicmp ( Result , "pb_" , 3 ) ;
			pbsdk->pb_ExecCmd ( Result ) ;
			if ( pb ) PbServerForceProcess() ;
		}
	}
	else {
		char *arg1 = Result ;
		while ( *arg1 == ' ' ) ++arg1 ;
		while ( *arg1 && *arg1 != ' ' ) ++arg1 ;
		char *endResult = arg1 ;
		while ( *arg1 == ' ' ) ++arg1 ;

		if ( !_stricmp ( Cmd , "DropClient" ) ) pbsdk->pb_DropClient ( atoi ( Result ) , arg1 ) ;
		else if ( !_stricmp ( Cmd , "Cvar_Set" ) ) {
			char hold = *endResult ;
			*endResult = 0 ;
			pbsdk->pb_CvarSet ( Result , arg1 ) ;
			*endResult = hold ;
		}
	}
	
	return NULL ;
}



//
// PbSvGameQuery
//
// assumes Data buffer is appropriately size/allocated for Qtype call
char *PbSvGameQuery ( int Qtype , char *Data )
{
	if ( pbsdk == NULL ) {
		return NULL;
	}

	Data[PB_Q_MAXRESULTLEN] = 0;

	int i;
	switch ( Qtype ) {
	case PB_Q_MAXCLIENTS: _itoa ( pbsdk->pb_GetMaxClients() , Data , 10 ); break;
	case PB_Q_CLIENT:
		i = atoi ( Data );
		if ( !pbsdk->pb_GetClientInfo ( i , (stPb_Sv_Client *) Data ) ) return PbsQueryFail;
		break;
	case PB_Q_CVAR: 
		strncpy( Data , pbsdk->pb_GetCvarValue ( Data , 1 ) , PB_Q_MAXRESULTLEN );
		break;
	case PB_Q_STATS:
		i = atoi ( Data ) ;
		if ( !pbsdk->pb_GetClientStats ( i , Data ) ) return PbsQueryFail;
		break;
	}
	return NULL;
}



//
// PbSvGameMsg
//
char *PbSvGameMsg ( char *Msg , int Type )
{
	if ( pbsdk == NULL ) return NULL ;
	if ( Type & PB_MSG_CONSOLE ) {
		pbsdk->pb_Outf ( 0xffffff00 , "%s: %s\n" , pbsdk->pbsv.m_msgPrefix , Msg ) ;
	}
	if ( Type & PB_MSG_SCREEN ) {
//todo: output message to screen -not console
	}
	if ( Type & PB_MSG_LOG ) {
//todo: output message to game log file
	}
	return NULL ;
}



//
// PbSvSendToClient
//
char *PbSvSendToClient ( int DataLen , char *Data , int clientIndex )
{
	if ( pbsdk == NULL ) return NULL ;
	pbsdk->pb_SendSvPacket ( DataLen , Data , clientIndex ) ;
	return NULL ;
}



//
// PbSvSendToAddrPort
//
char *PbSvSendToAddrPort ( char *addr , unsigned short port , int DataLen , char *Data )
{
	if ( pbsdk == NULL ) return NULL ;
	pbsdk->pb_SendUdpPacket( addr , port , DataLen , Data, true );
	return NULL ;
}

#endif //#ifdef __WITHPB__
#endif

