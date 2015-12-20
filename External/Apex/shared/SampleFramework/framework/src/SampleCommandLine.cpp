/*
 * Copyright 2008-2012 NVIDIA Corporation.  All rights reserved.
 *
 * NOTICE TO USER:
 *
 * This source code is subject to NVIDIA ownership rights under U.S. and
 * international Copyright laws.  Users and possessors of this source code
 * are hereby granted a nonexclusive, royalty-free license to use this code
 * in individual and commercial software.
 *
 * NVIDIA MAKES NO REPRESENTATION ABOUT THE SUITABILITY OF THIS SOURCE
 * CODE FOR ANY PURPOSE.  IT IS PROVIDED "AS IS" WITHOUT EXPRESS OR
 * IMPLIED WARRANTY OF ANY KIND.  NVIDIA DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOURCE CODE, INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE.
 * IN NO EVENT SHALL NVIDIA BE LIABLE FOR ANY SPECIAL, INDIRECT, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS,  WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION,  ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOURCE CODE.
 *
 * U.S. Government End Users.   This source code is a "commercial item" as
 * that term is defined at  48 C.F.R. 2.101 (OCT 1995), consisting  of
 * "commercial computer  software"  and "commercial computer software
 * documentation" as such terms are  used in 48 C.F.R. 12.212 (SEPT 1995)
 * and is provided to the U.S. Government only as a commercial end item.
 * Consistent with 48 C.F.R.12.212 and 48 C.F.R. 227.7202-1 through
 * 227.7202-4 (JUNE 1995), all U.S. Government End Users acquire the
 * source code with only those rights set forth herein.
 *
 * Any use of this source code in individual and commercial software must
 * include, in the user documentation and internal comments to the code,
 * the above Disclaimer and U.S. Government End Users Notice.
 */
#include "FrameworkFoundation.h"
#include <SampleCommandLine.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "PsFile.h"

#ifdef WIN32
#include <windows.h>
#elif defined(ANDROID)
typedef int errno_t;
#endif


#include <PsString.h>

using namespace SampleFramework;

static char **CommandLineToArgvA(const char *cmdLine, unsigned int &_argc)
{
	char        **argv     = 0;
	char         *_argv    = 0;
	unsigned int  len      = 0;
	unsigned int  argc     = 0;
	char          a        = 0;
	unsigned int  i        = 0;
	unsigned int  j        = 0;
	bool          in_QM    = false;
	bool          in_TEXT  = false;
	bool          in_SPACE = true;

	len   = (unsigned int)strlen(cmdLine);
	i     = ((len+2)/2)*sizeof(char*) + sizeof(char*);
	argv  = (char **)malloc(i + (len+2)*sizeof(char));
	_argv = (char *)(((char *)argv)+i);
	argv[0] = _argv;

	i = 0;

	a = cmdLine[i];
	while(a)
	{
		if(in_QM)
		{
			if(a == '\"')
			{
				in_QM = false;
			}
			else
			{
				_argv[j] = a;
				j++;
			}
		}
		else
		{
			switch(a)
			{
			case '\"':
				in_QM = true;
				in_TEXT = true;
				if(in_SPACE)
				{
					argv[argc] = _argv+j;
					argc++;
				}
				in_SPACE = false;
				break;
			case ' ':
			case '\t':
			case '\n':
			case '\r':
				if(in_TEXT)
				{
					_argv[j] = '\0';
					j++;
				}
				in_TEXT = false;
				in_SPACE = true;
				break;
			default:
				in_TEXT = true;
				if(in_SPACE)
				{
					argv[argc] = _argv+j;
					argc++;
				}
				_argv[j] = a;
				j++;
				in_SPACE = false;
				break;
			}
		}
		i++;
		a = cmdLine[i];
	}
	_argv[j] = '\0';
	argv[argc] = 0;
	_argc = argc;
	return argv;
}

static bool isSwitchChar(char c)
{
	return (c == '-' || c == '/');
}

namespace
{
void operationOK(int e)
{
	PX_FORCE_PARAMETER_REFERENCE(e);
	PX_ASSERT(0 == e);
}

const char * const * getCommandLineArgumentsFromFile(unsigned int & argc, const char * programName, const char * commandLineFilePath)
{
	const char * const * argv = NULL;
	argc = 0;
	PX_ASSERT(NULL != programName);
	PX_ASSERT(NULL != commandLineFilePath);
	if(NULL != programName && NULL != commandLineFilePath)
	{
		FILE * commandLineFile = NULL;
		physx::shdfnd::fopen_s(&commandLineFile, commandLineFilePath, "r");
		if(NULL != commandLineFile)
		{
			operationOK(::fseek(commandLineFile, 0, SEEK_END));
			const unsigned int commandLineFileCount = static_cast<unsigned int>(::ftell(commandLineFile));
			::rewind(commandLineFile);
			const unsigned int bufferCount = static_cast<unsigned int>(::strlen(programName)) + 1 + commandLineFileCount + 1;
			if(bufferCount > 0)
			{
				char * argsOwn = static_cast<char*>(::malloc(bufferCount));
				physx::string::strcpy_s(argsOwn, bufferCount, programName);
				physx::string::strcat_s(argsOwn, bufferCount, " ");
				const unsigned int offset = static_cast<unsigned int>(::strlen(argsOwn));
				PX_ASSERT((bufferCount - offset - 1) == commandLineFileCount);
				if(NULL != ::fgets(argsOwn + offset, bufferCount - offset, commandLineFile))
				{
					argv = CommandLineToArgvA(argsOwn, argc);
				}
				::free(argsOwn);
				argsOwn = NULL;
			}
			operationOK(::fclose(commandLineFile));
			commandLineFile = NULL;
		}
	}
	return argv;
}
}; //namespace nameless

SampleCommandLine::SampleCommandLine(unsigned int argc, const char * const * argv, const char * commandLineFilePathFallback)
	:m_argc(0)
	,m_argv(NULL)
	,m_freeme(NULL)
	,m_argUsed(NULL)
{
	//initially, set to use inherent command line arguments
	PX_ASSERT((0 != argc) && (NULL != argv) && "This class assumes argument 0 is always the executable path!");
	m_argc = argc;
	m_argv = argv;

	//finalize init
	initCommon(commandLineFilePathFallback);
}

SampleCommandLine::SampleCommandLine(const char * args, const char * commandLineFilePathFallback)
	:m_argc(0)
	,m_argv(NULL)
	,m_freeme(NULL)
	,m_argUsed(NULL)
{
	//initially, set to use inherent command line arguments
	unsigned int argc = 0;
	const char * const * argvOwning = NULL;
	argvOwning = CommandLineToArgvA(args, argc);
	PX_ASSERT((0 != argc) && (NULL != argvOwning) && "This class assumes argument 0 is always the executable path!");
	m_argc = argc;
	m_argv = argvOwning;
	m_freeme = const_cast<void *>(static_cast<const void *>(argvOwning));
	argvOwning = NULL;

	//finalize init
	initCommon(commandLineFilePathFallback);
}

SampleCommandLine::~SampleCommandLine(void)
{
	if(m_freeme)
	{
		free(m_freeme);
		m_freeme = NULL;
	}
	if(m_argUsed)
	{
		delete[] m_argUsed;
		m_argUsed = NULL;
	}
}

void SampleCommandLine::initCommon(const char * commandLineFilePathFallback)
{
	//if available, set to use command line arguments from file
	const bool tryUseCommandLineArgumentsFromFile = ((1 == m_argc) && (NULL != commandLineFilePathFallback));
	if(tryUseCommandLineArgumentsFromFile)
	{
		unsigned int argcFile = 0;
		const char * const * argvFileOwn = NULL;
		argvFileOwn = getCommandLineArgumentsFromFile(argcFile, m_argv[0], commandLineFilePathFallback);
		if((0 != argcFile) && (NULL != argvFileOwn))
		{
			if(NULL != m_freeme)
			{
				::free(m_freeme);
				m_freeme = NULL;
			}
			m_argc = argcFile;
			m_argv = argvFileOwn;
			m_freeme = const_cast<void *>(static_cast<const void *>(argvFileOwn));
			argvFileOwn = NULL;
		}
	}

	//for tracking use-status of arguments
	if((0 != m_argc) && (NULL != m_argv))
	{
		m_argUsed = new bool[m_argc];
		for(unsigned int i = 0; i < m_argc; i++)
		{
			m_argUsed[i] = false;
		}
	}
}

unsigned int SampleCommandLine::getNumArgs(void) const
{
	return(m_argc);
}

const char * SampleCommandLine::getProgramName(void) const
{
	return(m_argv[0]);
}

unsigned int SampleCommandLine::unusedArgsBufSize(void) const
{
	unsigned int bufLen = 0;
	for(unsigned int i = 1; i < m_argc; i++)
	{
		if((!m_argUsed[i]) && isSwitchChar(m_argv[i][0]))
		{
			bufLen += (unsigned int) strlen(m_argv[i]) + 1;	// + 1 is for the space between unused args
		}
	}
	if(bufLen != 0)
	{
		bufLen++;	// if there are unused args add a space for the '\0' char.
	}
	return(bufLen);
}

const char* SampleCommandLine::getUnusedArgs(char *buf, unsigned int bufSize) const
{
	if(bufSize != 0)
	{
		buf[0] = '\0';
		for(unsigned int i = 1; i < m_argc; i++)
		{
			if((!m_argUsed[i]) && isSwitchChar(m_argv[i][0]))
			{
				physx::string::strcat_s(buf, bufSize, m_argv[i]);
				physx::string::strcat_s(buf, bufSize, " ");
			}
		}
	}
	return(buf);
}

//! has a given command-line switch?
//  e.g. s=="foo" checks for -foo
bool SampleCommandLine::hasSwitch(const char *s, unsigned int argNum) const
{
	bool has = false;
	PX_ASSERT(*s);
	if(*s)
	{
		unsigned int n = (unsigned int) strlen(s);
		unsigned int firstArg;
		unsigned int lastArg;
		if(argNum != invalidArgNum)
		{
			firstArg = argNum;
			lastArg  = argNum;
		}
		else
		{
			firstArg = 1;
			lastArg  = (m_argc > 1) ? (m_argc - 1) : 0;
		}
		for(unsigned int i=firstArg; i<=lastArg; i++)
		{
			const char *arg = m_argv[i];
			// allow switches of '/', '-', and double character versions of both.
			if( (isSwitchChar(*arg) && !physx::string::strnicmp(arg+1, s, n) && ((arg[n+1]=='\0')||(arg[n+1]=='='))) ||
				(isSwitchChar(*(arg+1)) && !physx::string::strnicmp(arg+2, s, n) && ((arg[n+2]=='\0')||(arg[n+2]=='='))) )
			{
				m_argUsed[i] = true;
				has = true;
				break;
			}
		}
	}
	return has;
}

//! gets the value of a switch... 
//  e.g. s="foo" returns "bar" if '-foo=bar' is in the commandline.
const char *SampleCommandLine::getValue(const char *s,  unsigned int argNum) const
{
	const char *value = 0;
	PX_ASSERT(*s);
	if(*s)
	{
		unsigned int firstArg;
		unsigned int lastArg;
		if(argNum != invalidArgNum)
		{
			firstArg = argNum;
			lastArg  = argNum;
		}
		else
		{
			firstArg = 1;
			lastArg  = m_argc - 1;
		}
		for(unsigned int i=firstArg; i<=lastArg; i++)
		{
			const char *arg = m_argv[i];
			if(isSwitchChar(*arg))
			{
				arg++;
				if(isSwitchChar(*arg))	// is it a double dash arg?  '--'
				{
					arg++;
				}
				const char *st=s;
				for(; *st && *arg && toupper(*st)==toupper(*arg) && *arg!='='; st++,arg++)
				{
				}
				if(!*st && *arg=='=')
				{
					m_argUsed[i] = true;
					value = arg+1;
					break;
				}
				if(!*st && !*arg && ((i+1)<m_argc) && (!isSwitchChar(*m_argv[i+1])))
				{
					m_argUsed[i] = true;
					value = m_argv[i+1];
					break;
				}
			}
		}
	}
	return value;
}

//! if the first argument is the given command.
bool SampleCommandLine::isCommand(const char *s) const
{
	bool has = false;
	const char *command = getCommand();
	if(command && !physx::string::stricmp(command, s))
	{
		has = true;
	}
	return has;
}

//! get the first argument assuming it isn't a switch.
//  e.g. for the command-line "myapp.exe editor -foo" it will return "editor".
const char *SampleCommandLine::getCommand(void) const
{
	const char *command = 0;
	if(m_argc > 1 && !isSwitchChar(*m_argv[1]))
	{
		command = m_argv[1];
	}
	return command;
}

//! whether or not an argument has been read already
bool SampleCommandLine::isUsed(unsigned int argNum) const
{
	return argNum < m_argc ? m_argUsed[argNum] : false;
}
