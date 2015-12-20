// Copyright (C) 2001-2003 Even Balance, Inc.
//
//
// pbcl.cpp
//
// EVEN BALANCE - T.RAY
//


#ifdef __WITH_PB__
#define _cplusplus

#include "pbmd5.h"

#define GETPBSDKPOINTER
#include "pbsdk.h"



#ifdef __WITH_PB__

#define PB_MAX_LL		300
#define PB_MAX_FNLEN	63

struct stLL {
	char fn[PB_MAX_FNLEN+1];
	unsigned char digest[16];
	long sz;
};
stLL PB_LL[PB_MAX_LL];
int PB_nLL = 0;

// PunkBuster ScreenShot Function
PBReceiveSS_t PBReceiveSS = NULL ;

// Dimensions of the buffer to capture
int PB_SScenterXpct = 0 ;
int PB_SScenterYpct = 0 ;
int PB_SSrectWidth = 320 ;
int PB_SSrectHeight = 200 ;

//
// PbClGameCommand
//
char *PbClGameCommand ( char *Cmd , char *Result )
{
	if ( pbsdk == NULL ) return NULL ;

	char *arg1 = Result ;
	while ( *arg1 == ' ' ) ++arg1 ;
	while ( *arg1 && *arg1 != ' ' ) ++arg1 ;
	while ( *arg1 == ' ' ) ++arg1 ;

	if ( !_stricmp ( Cmd , "set_cl_punkbuster" ) ) pbsdk->pb_SetClPunkBuster ( Result ) ;
	else if ( !_stricmp ( Cmd , "Cvar_Set" ) ) pbsdk->pb_CvarSet ( Result , arg1 ) ;
	else if ( !_stricmp ( Cmd , "Cmd_Exec" ) ) pbsdk->pb_ExecCmd ( Result ) ;
	return NULL ;
}



//extern int PbSearchBindings ( char *subtext , int iStart ) ;								//cl_keys.c
//
// PbClGameQuery
//
// assumes Data points to buffer at least as large as PB_Q_MAXRESULTLEN+1
//
char *PbClGameQuery ( int Qtype , char *Data )
{
	if ( pbsdk == NULL ) return NULL ;
	if ( Data == NULL ) return NULL ;

	if ( Qtype != PB_Q_SSFUNC )
		Data[PB_Q_MAXRESULTLEN] = 0 ;
	unsigned char md5[16] ;
	long sz = 0 ;

	char *arg2 = Data , *name , *string , *resetString ;
	int i , n , flags ;

	if ( Qtype != PB_Q_SSFUNC )
	{
		while ( *arg2 && *arg2 != ' ' ) ++arg2 ;
		while ( *arg2 == ' ' ) ++arg2 ;
	}
	switch ( Qtype ) {
	case PB_Q_CVAR: 		
		strncpy ( Data , pbsdk->pb_GetCvarValue ( Data , 0 ) , PB_Q_MAXRESULTLEN ) ; break ;
	case PB_Q_SINFO: 
		strncpy ( Data , pbsdk->pb_GetCvarValue ( Data , 0 ) , PB_Q_MAXRESULTLEN ) ; break ;
	case PB_Q_SADDR: strncpy ( Data , pbsdk->pb_GetServerAddr() , PB_Q_MAXRESULTLEN ) ; break ;
	case PB_Q_SEARCHBINDINGS:
		n = pbsdk->pb_GetMaxKeys() ;
		for ( i = atoi ( Data ) ; i < n ; i++ ) if ( stristr ( pbsdk->pb_GetKeyBinding ( i ) , arg2 ) != NULL ) {
			_itoa ( i , Data , 10 ) ;
			return NULL ;
		}
		_itoa ( -1 , Data , 10 ) ;
		break ;
	case PB_Q_GETBINDING:
		strncpy ( Data , pbsdk->pb_GetKeyBinding ( atoi ( Data ) ) , PB_Q_MAXRESULTLEN ) ; break ;
	case PB_Q_KEYNAME:
		strncpy ( Data , pbsdk->pb_GetKeyName ( atoi ( Data ) ) , PB_Q_MAXRESULTLEN ) ; break ;
	case PB_Q_SEARCHCVARS:
		if ( ! (*Data) ) break ;
		while ( pbsdk->pb_CvarWalk ( &name , &string , &flags , &resetString ) ) {
			if ( name == NULL || string == NULL ) continue ;
			if ( !(*name) || !(*string) ) continue ;
			if ( stristr ( string , Data ) != NULL ) {
				strncpy ( Data , name , PB_Q_MAXRESULTLEN ) ;
				return NULL ;
			}
		}
		*Data = 0 ;
		break ;
	case PB_Q_CVARVALID: return pbsdk->pb_CvarValidate ( Data ) ;
	case PB_Q_CVARFLAGS:				//note: this query type returns NULL when done only, cvar name otherwise
		if ( pbsdk->pb_CvarWalk ( &name , &string , &flags , &resetString ) == 0 ) return NULL ;
		if ( name == NULL ) name = "" ;
		_itoa ( flags , Data , 10 ) ;
		return name ;
	case PB_Q_CVARDEFAULTS:				//note: this query type returns NULL when done only, cvar name otherwise
		if ( pbsdk->pb_CvarWalk ( &name , &string , &flags , &resetString ) == 0 ) return NULL ;
		if ( name == NULL || string == NULL || resetString == NULL ) name = string = resetString = "" ;
		if ( !strcmp ( string , resetString ) ) *Data = 0 ;
		else {
			strncpy ( Data + 1 , resetString , PB_Q_MAXRESULTLEN - 2 ) ;
			Data[0] = '"' ;
			strcat ( Data , "\"" ) ;
		}
		return name ;
	case PB_Q_EXEINSTANCE: 
#ifdef __PBWIN32__
		return (char *) pbsdk->exeInstance ;
#else
		strncpy ( Data , (char *) pbsdk->exeInstance , PB_Q_MAXRESULTLEN ) ;
		return NULL ;
#endif
	case PB_Q_DLLHANDLE: return (char *) pbsdk->pb_DllHandle ( Data ) ;
	case PB_Q_FILEMD5:
		pbsdk->pb_GetFileMD5 ( atoi ( Data ) , Data , md5 , &sz ) ;
		Data[PB_Q_MAXRESULTLEN-50] = 0 ;
		sprintf ( Data + strlen ( Data ) , " %02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x %ld" ,
			(int) md5[0] , (int) md5[1] , (int) md5[2] , (int) md5[3] ,
			(int) md5[4] , (int) md5[5] , (int) md5[6] , (int) md5[7] ,
			(int) md5[8] , (int) md5[9] , (int) md5[10] , (int) md5[11] ,
			(int) md5[12] , (int) md5[13] , (int) md5[14] , (int) md5[15] , sz ) ;
		break ;
	case PB_Q_SSFUNC:
		PBReceiveSS = (PBReceiveSS_t) Data ;
		break ;
	case PB_Q_SSCENTERXPCT:
		PB_SScenterXpct = atoi ( Data ) ;
		if ( PB_SScenterXpct < 0 || PB_SScenterXpct > 100 )
			PB_SScenterXpct = 100 ;
		break ;
	case PB_Q_SSCENTERYPCT:
		PB_SScenterYpct = atoi ( Data ) ;
		if ( PB_SScenterYpct < 0 || PB_SScenterYpct > 100 )
			PB_SScenterYpct = 100 ;
		break ;
	case PB_Q_SSRECTWIDTH:
		PB_SSrectWidth = atoi ( Data ) ;
		if ( PB_SSrectWidth < 320 )
			PB_SSrectWidth = 320 ;
		break ;
	case PB_Q_SSRECTHEIGHT:
		PB_SSrectHeight = atoi ( Data ) ;
		if ( PB_SSrectHeight < 200 )
			PB_SSrectHeight = 200 ;
		break ;
	default: *Data = 0 ; break ;
	}
	return NULL ;
}

//
// PbClGameMsg
//
char *PbClGameMsg ( char *Msg , int Type )
{
	if ( pbsdk == NULL ) return NULL ;

	if ( Type & PB_MSG_CONSOLE ) {
		pbsdk->pb_Outf ( 0xff00ffff , "%s: %s\n" , pbsdk->pbcl.m_msgPrefix , Msg ) ;
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
// PbClSendToServer
//
char *PbClSendToServer ( int DataLen , char *Data )
{
	if ( pbsdk == NULL ) return NULL ;
	pbsdk->pb_SendClPacket ( DataLen , Data ) ;
	return NULL ;
}



//
// PbClSendToAddrPort
//
char *PbClSendToAddrPort ( char *addr , unsigned short port , int DataLen , char *Data )
{
	if ( pbsdk == NULL ) return NULL ;
	pbsdk->pb_SendUdpPacket ( addr , port , DataLen , Data, false ) ;
	return NULL ;
}



//
// Function wrappers used to call C++ functions from C
//   these are declared in pbcommon.h

extern "C" {

void PbClAddEvent ( int event , int datalen , char *data )
{
	if ( pbsdk == NULL ) return ;
	pbsdk->pbcl.AddPbEvent ( event , datalen , data , 0 ) ;
}

int PbTrapPreExecCmd ( char *cmdtext )//return 0 if game should continue exec'ing the command, 1 if not
{
	if ( pbsdk == NULL ) return 0 ;
	if ( pbsdk->pbcl.m_TrapPreExecCmd == NULL ) return 0 ;
	return pbsdk->pbcl.m_TrapPreExecCmd ( &pbsdk->pbcl , cmdtext ) ;
}

void PbClientTrapConsole ( char *msg , int msglen )
{
	if ( pbsdk == NULL ) return ;
	if ( pbsdk->pbcl.m_TrapConsole == NULL ) return ;
	pbsdk->pbcl.m_TrapConsole ( &pbsdk->pbcl , msg , msglen ) ;
}

void PbClientInitialize ( void *exeInst )
{
	if ( pbsdk == NULL ) return ;
	pbsdk->exeInstance = exeInst ;
	pbsdk->pbcl.pbsvptr = &pbsdk->pbsv ;
	if ( pbsdk->pbsv.m_GameCommand == NULL ) PbServerInitialize() ;
	pbsdk->pbcl.initialize() ;

	pbsdk->pb_getBasePath ( pbsdk->pbcl.m_basepath , PB_Q_MAXRESULTLEN ) ;
	pbsdk->pb_getHomePath ( pbsdk->pbcl.m_homepath , PB_Q_MAXRESULTLEN ) ;

	PbClAddEvent ( PB_EV_CONFIG , 0 , "" ) ;
	if ( pbsdk->pbcl.m_ClInstance == NULL ) pbsdk->pb_SetClPunkBuster ( "0" ) ;
}

void PbClientProcessEvents ( void )
{
	static int inited = 0 ;
	if ( pbsdk == NULL ) return ;
	if ( !inited ) {
		inited = 1 ;
		PbClientInitialize ( NULL ) ;
	}
	pbsdk->pbcl.ProcessPbEvents() ;
}

void PbClientForceProcess ( void )
{
	if ( pbsdk == NULL ) return ;
	pbsdk->pbcl.ProcessPbEvents ( -1 ) ;
}

//added for Enemy Territory - PB knows max pktlen is 1024
void PbClientConnecting ( int status , char *pkt , int *pktlen )
{
	if ( pbsdk == NULL ) return ;
	if ( pbsdk->pbcl.m_ClientConnect == NULL ) return ;
	pbsdk->pbcl.m_ClientConnect ( &pbsdk->pbcl , status , pkt , pktlen ) ;
}

void PbClientCompleteCommand ( char *buf , int buflen )
{
	if ( pbsdk == NULL ) return ;
	pbsdk->pbcl.AddPbEvent ( PB_EV_CMDCOMPL , buflen , buf ) ;
}

static int PbAddLL ( char *fn, unsigned char *digest, long sz )
{
	int pos ;

	// Check to see if there is an existing slot for this file
	for ( pos = 0 ; pos < PB_nLL ; pos ++ ) 
		if ( !_stricmp ( PB_LL[pos].fn , fn ) ) break ;

	// Not a duplicate, so try adding it to the list
	if ( pos >= PB_nLL ) {
		if ( PB_nLL >= PB_MAX_LL ) return -1 ; //array is full
		pos = PB_nLL ;
		++PB_nLL ;
	}

	// Find the last directory separator
	int i , j = -1 ;
	for ( i = strlen ( fn ) - 1 ; i >= 0 ; i-- ) {
		if ( fn[i] == *pbDIRSEP ) {
			j = 1 ;
			break ;
		}
	}

	if ( j < 0 ) i = 0 ;
	else i = j + strlen ( pbDIRSEP ) ;
	strncpy ( PB_LL[pos].fn , fn + i , PB_MAX_FNLEN ) ;
	PB_LL[pos].fn[PB_MAX_FNLEN] = 0 ;
	memcpy ( PB_LL[pos].digest , digest, 16 ) ;
	PB_LL[pos].sz = sz ;

	return pos ;

}

int PbAddDigest ( char *fn ) 
{

	MD5_CTX m ;
	MD5Init ( &m ) ;

	FILE *f = fopen ( fn, "rb" ) ;
	if ( f == NULL ) return PbAddLL ( fn , m.digest , -1 );

	char buf[0x8000];
	long trb = 0;
	for(;;) {
		long rb = fread ( buf, 1, 0x8000, f ) ;
		if ( rb < 1 ) break ;
		trb += rb ;
		MD5Update( &m , (unsigned char *) buf , rb ) ;
	}

	fclose ( f ) ;
	MD5Final( &m ) ;
	return PbAddLL ( fn, m.digest, trb ) ;

}

void md5Digest2text ( MD5_CTX *m , char *textbuf )//assumes textbuf is 33+ chars
{
	sprintf ( textbuf , "%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x", 
		(int) m->digest[0] , (int) m->digest[1] , (int) m->digest[2] , (int) m->digest[3] ,
		(int) m->digest[4] , (int) m->digest[5] , (int) m->digest[6] , (int) m->digest[7] ,
		(int) m->digest[8] , (int) m->digest[9] , (int) m->digest[10] , (int) m->digest[11] ,
		(int) m->digest[12] , (int) m->digest[13] , (int) m->digest[14] , (int) m->digest[15] ) ;
}

char *PbSetGuid ( char *nums , int len )//updated for ET
{
	if ( pbsdk == NULL ) return "" ;
	MD5_CTX m ;
	MD5Init ( &m , 11961507 ) ;
	MD5Update ( &m , (unsigned char *) nums , len ) ;
	MD5Final ( &m ) ;
	md5Digest2text ( &m , pbsdk->pbcl.m_guid ) ;
	MD5Init ( &m , 334422 ) ;
	MD5Update ( &m , (unsigned char *) pbsdk->pbcl.m_guid , strlen ( pbsdk->pbcl.m_guid ) ) ;
	MD5Final ( &m ) ;
	md5Digest2text ( &m , pbsdk->pbcl.m_guid ) ;
	return pbsdk->pbcl.m_guid ;
}

int isPbClEnabled ( void )
{
	if ( pbsdk == NULL ) return 0 ;
	return (int) pbsdk->pbcl.AddPbEvent ( PB_EV_ISENABLED , 0 , NULL ) ;
}

int getPbGuidAge ( void )
{
	if ( !isPbClEnabled() ) return -2 ;
	return (int) pbsdk->pbcl.AddPbEvent ( PB_EV_GUIDAGE , 0 , NULL ) ;//returns -1 if bad/missing cdkey
}

void EnablePbCl ( void )
{
	if ( pbsdk == NULL ) return ;
	pbsdk->pbcl.AddPbEvent ( PB_EV_ENABLE , 0 , NULL ) ;
}

void DisablePbCl ( void )
{
	if ( pbsdk == NULL ) return ;
	pbsdk->pbcl.AddPbEvent ( PB_EV_DISABLE , 0 , NULL ) ;
}

void PbGetFileMD5( int index, char *fn, unsigned char *md5, long *sz )
{
	if ( index < 0 || index >= PB_nLL ) {
		strcpy ( fn, pbDIRSEP ) ;
		*md5 = 0;
		*sz = 0;
		return;
	}
	strcpy ( fn, PB_LL[index].fn ) ;
	memcpy ( md5, PB_LL[index].digest, 16 ) ;
	*sz = PB_LL[index].sz ;
}


} //extern "C"

#endif //#ifdef __WITHPB__
#endif

