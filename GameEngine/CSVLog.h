//=========================================================================
//	Module: CSVLog.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

// NOTE: This is currently limited to logging float data type.

#pragma once

#include <map>
#include <deque>
#include <vector>
#include <string>

class CSVEntry
{
	static const int MAX_ENTRIES = 64;

	float m_values[MAX_ENTRIES];

public:
	CSVEntry();
	CSVEntry(const CSVEntry& rhs);

	const CSVEntry& operator=(const CSVEntry& rhs);
	float& operator[](int index);

	void Reset();
	int FPrintCSVLine(FILE* f, int numEntries);
};

class CSVLog
{

	std::vector<std::string>		m_columnNames;
	std::map<std::string, uint32_t>	m_mapNameToColumn;
	std::deque<CSVEntry>			m_queEntries;
	CSVEntry						m_currentEntry;
	bool							m_firstEntryMade;

	static char*		m_csvLogFilename;
	static FILE*		m_csvLogFile;
	static int			m_csvLogInited;
	static int			m_csvLogOpened;
	static bool			m_csvColNamesWritten;

protected:
	void InitCSVLogFile();
	void CloseCSVLogFile();
	void FlushToCSVLogFile();
	void WriteColumnNamesToCSVLogFile();

public:
	CSVLog();
	CSVLog(const char* fname);
	~CSVLog();

	void StartEntry();
	void EndEntry();
	void Update();

	void ChangeLogFile(const char* fname);

	void Log(const char* colname, float value);
};

//extern CSVLog* g_pCSV;

