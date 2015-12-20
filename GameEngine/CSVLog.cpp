//=========================================================================
//	Module: CSVLog.cpp
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#include "r3dPCH.h"
#include "r3d.h"

#include "CSVLog.h"

//CSVLog* g_pCSV = new CSVLog;

CSVEntry::CSVEntry()
{
	Reset();
}

CSVEntry::CSVEntry(const CSVEntry& rhs)
{
	memcpy( m_values, rhs.m_values, sizeof(float) * MAX_ENTRIES );
}

const CSVEntry& CSVEntry::operator=(const CSVEntry& rhs)
{
	memcpy( m_values, rhs.m_values, sizeof(float) * MAX_ENTRIES );
	return *this;
}

float& CSVEntry::operator[](int index)
{
	r3d_assert( index < MAX_ENTRIES );

	return m_values[ index ];
}

void CSVEntry::Reset()
{
	memset( m_values, 0, sizeof(float) * MAX_ENTRIES );
}

int CSVEntry::FPrintCSVLine(FILE* f, int numEntries)
{
	r3d_assert(numEntries < MAX_ENTRIES);
	char buf[4096];
	int pos = 0;
	for(int i = 0; i < numEntries && pos < 4096; ++i)
	{
		int numChars;
		if( i < numEntries - 1 )
			numChars = sprintf(&(buf[pos]), "%0.6f,", m_values[i]);
		else
			numChars = sprintf(&(buf[pos]), "%0.6f\n", m_values[i]);
		if( numChars > 0 )
			pos += numChars;
	}
	fprintf(f, buf);
	fflush(f);

	return pos;
}

//////////////////////////////////////////////////////////////////////////////

char* CSVLog::m_csvLogFilename		= NULL;
FILE* CSVLog::m_csvLogFile			= NULL;
int CSVLog::m_csvLogInited			= 0;
int CSVLog::m_csvLogOpened			= 0;
bool CSVLog::m_csvColNamesWritten	= false;

void CSVLog::InitCSVLogFile()
{
	FILE	*f;
	if((f = fopen(m_csvLogFilename, "wt")) != NULL)
	{
		fclose(f);
	}
}

void CSVLog::CloseCSVLogFile()
{
	if(m_csvLogFile) 
	{
		FlushToCSVLogFile();
		fclose(m_csvLogFile);
		m_csvLogOpened = 0;
		m_csvLogFile   = NULL;
	}
}

void CSVLog::FlushToCSVLogFile()
{
	if(!m_csvLogInited)
	{
		m_csvLogInited = 1;
		InitCSVLogFile();
	}

	if(!m_csvLogOpened) 
	{
		m_csvLogOpened = 1;
		m_csvLogFile   = fopen(m_csvLogFilename, "at");
	}

	if( m_csvLogFile )
	{
		while( !m_queEntries.empty() )
		{
			std::deque<CSVEntry>::reference ref = m_queEntries.front();
			ref.FPrintCSVLine( m_csvLogFile, m_columnNames.size() );
			m_queEntries.pop_front();
		}
	}
}

void CSVLog::WriteColumnNamesToCSVLogFile()
{
	if( m_csvColNamesWritten )
		return;

	if(!m_csvLogInited)
	{
		m_csvLogInited = 1;
		InitCSVLogFile();
	}

	if(!m_csvLogOpened) 
	{
		m_csvLogOpened = 1;
		m_csvLogFile   = fopen(m_csvLogFilename, "at");
	}

	if( m_csvLogFile )
	{
		uint32_t i = 0;
		for( std::vector<std::string>::const_iterator iter = m_columnNames.begin();
			 iter != m_columnNames.end(); ++iter )
		{
			if( i < m_columnNames.size() - 1 )
				fprintf(m_csvLogFile, "%s,", iter->c_str());
			else
				fprintf(m_csvLogFile, "%s\n", iter->c_str());
			++i;
		}
		fflush(m_csvLogFile);
		m_csvColNamesWritten = true;
	}
}


CSVLog::CSVLog()
	: m_firstEntryMade( false )
{
	m_csvLogFilename = new char[ 11 ];
	strncpy( m_csvLogFilename, "csvLog.csv", 10 );
	InitCSVLogFile();
}

CSVLog::CSVLog(const char* fname)
	: m_firstEntryMade( false )
{
	uint32_t len = strlen(fname);

	r3d_assert( len > 0 );

	m_csvLogFilename = new char[ len + 1 ];
	strncpy( m_csvLogFilename, fname, len );
	m_csvLogFilename[ len ] = 0;
	InitCSVLogFile();
}

CSVLog::~CSVLog()
{
	CloseCSVLogFile();
	delete[] m_csvLogFilename;
	
	m_csvLogInited = 0;
	m_csvLogOpened = 0;
	m_csvColNamesWritten = false;
}

void CSVLog::StartEntry()
{
	if( m_firstEntryMade )
		EndEntry();
	m_currentEntry.Reset();
}

void CSVLog::EndEntry()
{
	m_queEntries.push_back( m_currentEntry );
}

void CSVLog::Update()
{
	static CRITICAL_SECTION _cs_csvlog;

	if(!m_csvLogInited)
	{
		m_csvLogInited = 1;

		InitializeCriticalSectionAndSpinCount(&_cs_csvlog, 4000);

		InitCSVLogFile();
	}

	r3dCSHolder csHolder(_cs_csvlog);

	if(!m_csvLogOpened) 
	{
		m_csvLogOpened = 1;
		m_csvLogFile   = fopen(m_csvLogFilename, "at");
	}

	if(m_csvLogFile) 
	{
		if( !m_csvColNamesWritten )
			WriteColumnNamesToCSVLogFile();
		float startTime = r3dGetTime();
		while( !m_queEntries.empty() /*&& 0.001f > r3dGetTime() - startTime*/ )
		{
			std::deque<CSVEntry>::reference ref = m_queEntries.front();
			ref.FPrintCSVLine( m_csvLogFile, m_columnNames.size() );
			m_queEntries.pop_front();
		}
	}
}

void CSVLog::ChangeLogFile(const char* fname)
{
	uint32_t len = strlen(fname);

	r3d_assert( len > 0 );

	delete[] m_csvLogFilename;
	m_csvLogFilename = new char[ len + 1 ];
	strncpy( m_csvLogFilename, fname, len );
	m_csvLogFilename[ len ] = '\0';

	if(m_csvLogOpened)
		fclose(m_csvLogFile);

	m_csvLogFile = fopen(m_csvLogFilename, "wt");
}

void CSVLog::Log(const char* colname, float value)
{
	int index = -1;
	std::string sColName( colname );
	std::map<std::string, uint32_t>::const_iterator iter = m_mapNameToColumn.find( sColName );
	if( iter == m_mapNameToColumn.end() )
	{
		index = m_columnNames.size();
		m_mapNameToColumn[ sColName ] = index;
		m_columnNames.push_back( sColName );
	}
	else
	{
		index = iter->second;
	}
	
	m_currentEntry[ index ] = value;
	m_firstEntryMade = true;
}
