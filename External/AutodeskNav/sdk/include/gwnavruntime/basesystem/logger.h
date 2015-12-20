/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

// primary contact: GUAL - secondary contact: NOBODY
#ifndef Navigation_Logger_H
#define Navigation_Logger_H

#include "gwnavruntime/kernel/SF_Debug.h"
#include "gwnavruntime/kernel/SF_Log.h"
#include "gwnavruntime/base/types.h"

namespace Kaim
{

class LogWrapper
{
public:
	LogWrapper(LogMessageId id) : m_id(id) {}

	void Print(const char* fmt, ...) const KY_LOG_VAARG_ATTRIBUTE(2,3);

private:
	LogMessageId m_id;
};

}

#define KY_LOG_BIG_TITLE_BEGIN(prefix, title) \
	   prefix << "==============================================================\n" \
	<< prefix << "======== " << title << "\n\n"

#define KY_LOG_BIG_TITLE_END(prefix, title) \
	   prefix << "======== " << title << "\n" \
	<< prefix << "==============================================================\n\n"
	
#define KY_LOG_SMALL_TITLE_BEGIN(prefix, title) \
	   prefix << "------------------------------------------------------\n" \
	<< prefix << "---- " << title << "\n"

#define KY_LOG_SMALL_TITLE_END(prefix, title) \
	   prefix << "---- " << title << "\n" \
	<< prefix << "------------------------------------------------------\n\n"




#ifndef KY_NO_LOG_OUTPUT

	// To log in DEBUG/RELEASE but not in SHIPPING
	// usage KY_LOG_XXX(("x = %d; y = %d", x, y));
	// KY_LOG_TEXT:    prefix=""         , suffix=""
	// KY_LOG_MESSAGE: prefix=""         , suffix=" [__FILE__:__LINE__]\n"
	// KY_LOG_WARNING: prefix="Warning: ", suffix=" [__FILE__:__LINE__]\n"
	// KY_LOG_ERROR:   prefix="Error: "  , suffix=" [__FILE__:__LINE__]\n"
	#define KY_LOG_TEXT(args)      KY_MACRO_START Kaim::LogWrapper(Kaim::Log_Message).Print args;                KY_MACRO_END
	#define KY_LOG_MESSAGE(args)   KY_MACRO_START Kaim::LogWrapper(Kaim::Log_Message).Print KY_LOG_SUFFIX(args); KY_MACRO_END
	#define KY_LOG_WARNING(args)   KY_MACRO_START Kaim::LogWrapper(Kaim::Log_Warning).Print KY_LOG_SUFFIX(args); KY_MACRO_END
	#define KY_LOG_ERROR(args)     KY_MACRO_START Kaim::LogWrapper(Kaim::Log_Error).Print   KY_LOG_SUFFIX(args); KY_MACRO_END

	// usage KY_LOG_OUTPUT((logMessageId ,"x = %d; y = %d", x, y))
	// where logMessageId is of LogMessageId, this will command prefix and suffix used, and it allows to specify your own channel
	#define KY_LOG_OUTPUT(id, args)    KY_MACRO_START Kaim::LogWrapper(id).Print args; KY_MACRO_END


	#define KY_LOG_TEXT_IF(condition, args)      KY_MACRO_START if (condition) { KY_LOG_TEXT(args);    }  KY_MACRO_END
	#define KY_LOG_MESSAGE_IF(condition, args)   KY_MACRO_START if (condition) { KY_LOG_MESSAGE(args); }  KY_MACRO_END
	#define KY_LOG_WARNING_IF(condition, args)   KY_MACRO_START if (condition) { KY_LOG_WARNING(args); }  KY_MACRO_END
	#define KY_LOG_ERROR_IF(condition, args)     KY_MACRO_START if (condition) { KY_LOG_ERROR(args);   }  KY_MACRO_END
	#define KY_LOG_OUTPUT_IF(condition, id, args)    KY_MACRO_START if (condition) { KY_LOG_OUTPUT(id, args);  }  KY_MACRO_END
#else
	#define KY_LOG_TEXT(args)     ((void)0)
	#define KY_LOG_MESSAGE(args)  ((void)0)
	#define KY_LOG_WARNING(args)  ((void)0)
	#define KY_LOG_ERROR(args)    ((void)0)
	#define KY_LOG_OUTPUT(args, id)   ((void)0)

	#define KY_LOG_TEXT_IF(condition, args)     ((void)0)
	#define KY_LOG_MESSAGE_IF(condition, args)  ((void)0)
	#define KY_LOG_WARNING_IF(condition, args)  ((void)0)
	#define KY_LOG_ERROR_IF(condition, args)    ((void)0)
	#define KY_LOG_OUTPUT_IF(condition, id, args)   ((void)0)

#endif


#endif //Navigation_Logger_H
