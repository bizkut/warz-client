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

#ifndef SAMPLE_COMMANDLINE_H
#define SAMPLE_COMMANDLINE_H

#include <assert.h>

namespace SampleFramework
{

	// Container for command-line arguments.
	// This class assumes argument 0 is always the executable path!
	class SampleCommandLine
	{
	public:
		//! commandLineFilePathFallback is an optional fall-back to a configuration file containing command line arguments.
		//  Its contents are only processed and used if the other constructor arguments yield only an executable path.
		//	This is especially useful in the case of an Andriod platform, which does not support command line options.
		//  e.g. commandLineFilePathFallback = "commandline.txt"
		//  e.g. contents of commandline.txt = --argument1 --argument2

		SampleCommandLine(unsigned int argc, const char *const* argv, const char * commandLineFilePathFallback = 0);
		SampleCommandLine(const char *args, const char * commandLineFilePathFallback = 0);
		~SampleCommandLine(void);

		//! has a given command-line switch?
		//  e.g. s=="foo" checks for -foo
		bool hasSwitch(const char *s, const unsigned int argNum = invalidArgNum) const;

		//! gets the value of a switch... 
		//  e.g. s="foo" returns "bar" if '-foo=bar' is in the commandline.
		const char* getValue(const char *s, unsigned int argNum = invalidArgNum) const;

		// return how many command line arguments there are
		unsigned int getNumArgs(void) const;

		// what is the program name
		const char* getProgramName(void) const;

		// get the string that contains the unsued args
		unsigned int unusedArgsBufSize(void) const;

		// get the string that contains the unsued args
		const char* getUnusedArgs(char *buf, unsigned int bufSize) const;

		//! if the first argument is the given command.
		bool isCommand(const char *s) const;

		//! get the first argument assuming it isn't a switch.
		//  e.g. for the command-line "myapp.exe editor -foo" it will return "editor".
		const char *getCommand(void) const;

		//! get the raw command-line argument list...
		unsigned int      getArgC(void) const { return m_argc; }
		const char *const*getArgV(void) const { return m_argv; }

		//! whether or not an argument has been read already
		bool isUsed(unsigned int argNum) const;

	private:
		SampleCommandLine(const SampleCommandLine&);
		SampleCommandLine(void);
		SampleCommandLine operator=(const SampleCommandLine&);
		void initCommon(const char * commandLineFilePathFallback);
		unsigned int		m_argc;
		const char *const	*m_argv;
		void				*m_freeme;
		static const unsigned int invalidArgNum = 0xFFFFFFFFU;
		bool*				m_argUsed;
	};

} // namespace SampleFramework

#endif // SAMPLE_COMMANDLINE_H
