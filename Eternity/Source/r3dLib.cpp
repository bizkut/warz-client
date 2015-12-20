#include "r3dPCH.h"
#include "r3d.h"
#include "r3dLib.h"

// def to 0 to disable license key check
#define CHECK_LICENSE_KEY 0

char* gLicenseKey = 0;

//Translation Table as described in RFC1113
static const char cb64[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
//Translation Table to decode
static const char cd64[]="|$$$}rstuvwxyz{$$$$$$$>?@ABCDEFGHIJKLMNOPQRSTUVW$$$$$$XYZ[\\]^_`abcdefghijklmnopq";

void encodeblock( unsigned char in[3], unsigned char out[4], int len )
{
    out[0] = cb64[ in[0] >> 2 ];
    out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xf0) >> 4) ];
    out[2] = (unsigned char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}

void encode( char *indata, char *outdata )
{
    unsigned char in[3], out[4];
    int i, len, blocksout = 0;

    while( *indata ) 
	{
        len = 0;
        for( i = 0; i < 3; i++ ) 
		{
            if( *indata != 0 ) 
			{
				in[i] = *indata; ++indata;
                len++;
            }
            else 
			{
                in[i] = 0;
            }
        }
        if( len ) 
		{
            encodeblock( in, out, len );
            for( i = 0; i < 4; i++ ) 
			{
                *outdata = out[i];
				++outdata;
            }
            blocksout++;
        }
    }
}

void decodeblock( unsigned char in[4], unsigned char out[3] )
{   
    out[ 0 ] = (unsigned char ) (in[0] << 2 | in[1] >> 4);
    out[ 1 ] = (unsigned char ) (in[1] << 4 | in[2] >> 2);
    out[ 2 ] = (unsigned char ) (((in[2] << 6) & 0xc0) | in[3]);
}

void decode( char *infile, char *outfile )
{
    unsigned char in[4], out[3], v;
    int i, len;

    while( *infile )
	{
        for( len = 0, i = 0; i < 4 && *infile; i++ ) 
		{
            v = 0;
            while( *infile && v == 0 ) 
			{
                v = (unsigned char) *infile;
                v = (unsigned char) ((v < 43 || v > 122) ? 0 : cd64[ v - 43 ]);
                if( v ) {
                    v = (unsigned char) ((v == '$') ? 0 : v - 61);
                }
            }
            if( *infile ) 
			{
                len++;
				++infile;
                if( v ) 
                    in[ i ] = (unsigned char) (v - 1);
            }
            else 
                in[i] = 0;
        }
        if( len ) 
		{
            decodeblock( in, out );
            for( i = 0; i < len - 1; i++ ) 
			{
				*outfile = out[i];
				++outfile;
            }
        }
    }
}

void r3dLibraryInit(char* license)
{
#if CHECK_LICENSE_KEY
	if(gLicenseKey)
		delete [] gLicenseKey;
	gLicenseKey = 0;
	if(license && strlen(license) == 44)
	{
		gLicenseKey = game_new char[34]; memset(gLicenseKey, 0, 34);
		decode(license, gLicenseKey);
	}
#endif
}

void r3dLibraryExit()
{
#if CHECK_LICENSE_KEY
	if(gLicenseKey)
		delete [] gLicenseKey;
	gLicenseKey = 0;
#endif
}

// license key generator
char* __internal_gnrt_lkey(const char* name, int exp_year, int exp_month, int exp_day)
{
	char* raw_key = game_new char[35]; memset(raw_key, 0, 35);
	char* result = game_new char[46]; memset(result, 0, 46);

	time_t rawtime;
	struct tm timeinfo;

	/* get current timeinfo and modify it to the user's choice */
	time ( &rawtime );
	unsigned int dword1 = (unsigned int)rawtime;
	timeinfo = *localtime ( &rawtime );
	timeinfo.tm_year = exp_year - 1900;
	timeinfo.tm_mon = exp_month - 1;
	timeinfo.tm_mday = exp_day;

	/* call mktime: timeinfo->tm_wday will be set */
	time_t exp_time = mktime ( &timeinfo );
	unsigned int dword2 = (unsigned int)exp_time;

	char sname[16];
	for(int i=0; i<16; ++i)
		sname[i] = 'a'+i;
	sname[15] = 0;
	strncpy(sname, name, R3D_MIN((int)strlen(name), 15));
	sprintf(raw_key, "%s %8x:%8x", sname, dword1, dword2);
	encode(raw_key, result);
	
	return result;
}

// stupid license key check
bool __internal_check_license_key()
{
	if(gLicenseKey)
	{
		// license key
		// 0..16 - name to whom licensed
		// 17..24 - secret data
		// 25..32 - exp.date
		int len = strlen(gLicenseKey);
		r3d_assert(len == 33);
		if(len == 33)
		{
			time_t exptime;
			unsigned int dword1=0, dword2=0;
			char temp[32];
			sscanf(gLicenseKey, "%s %x:%x", temp, &dword1, &dword2);
			exptime = (__int64)dword2;
			time_t curtime;
			time(&curtime);

			struct tm curdate = *localtime(&curtime);
			struct tm expdate = *localtime(&exptime);
			if(curdate.tm_year < expdate.tm_year ||
				curdate.tm_mon < expdate.tm_mon ||
				curdate.tm_mday < expdate.tm_mday)
				return true;
			else
				return false;
		}
	}
	return false;	
}

int frame_timer = 1;
void checkLicenseKey()
{
#if CHECK_LICENSE_KEY
	if(!__internal_check_license_key())
		--frame_timer;

	if(frame_timer <= 0)
	{
		MessageBox(0, "License expired", "License", MB_OK);
		exit(0);
	}
#endif
}
