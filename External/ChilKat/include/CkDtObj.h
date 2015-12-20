// CkDtObj.h: interface for the CkDtObj class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CkDtObj_H
#define _CkDtObj_H

#include "CkString.h"
#include "CkMultiByteBase.h"

#ifndef __sun__
#pragma pack (push, 8)
#endif
 

// CLASS: CkDtObj
class CkDtObj  : public CkMultiByteBase
{
    private:
	// Don't allow assignment or copying these objects.
	CkDtObj(const CkDtObj &);
	CkDtObj &operator=(const CkDtObj &);


    public:
	CkDtObj(void *impl);

	CkDtObj();
	virtual ~CkDtObj();

	void dispose(void);

	// BEGIN PUBLIC INTERFACE
	// DESERIALIZE_BEGIN
	void DeSerialize(const char *serializedDtObj);
	// DESERIALIZE_END

	// SERIALIZE_BEGIN
	bool Serialize(CkString &outStr);
	const char *serialize(void);
	// SERIALIZE_END

	// STRUCTTMMONTH_BEGIN
	int get_StructTmMonth(void);
	void put_StructTmMonth(int newVal);
	// STRUCTTMMONTH_END

	// STRUCTTMYEAR_BEGIN
	int get_StructTmYear(void);
	void put_StructTmYear(int newVal);
	// STRUCTTMYEAR_END

	// SECOND_BEGIN
	int get_Second(void);
	void put_Second(int newVal);
	// SECOND_END

	// MINUTE_BEGIN
	int get_Minute(void);
	void put_Minute(int newVal);
	// MINUTE_END

	// HOUR_BEGIN
	int get_Hour(void);
	void put_Hour(int newVal);
	// HOUR_END

	// DAY_BEGIN
	int get_Day(void);
	void put_Day(int newVal);
	// DAY_END

	// MONTH_BEGIN
	int get_Month(void);
	void put_Month(int newVal);
	// MONTH_END

	// YEAR_BEGIN
	int get_Year(void);
	void put_Year(int newVal);
	// YEAR_END

	// UTC_BEGIN
	bool get_Utc(void);
	void put_Utc(bool newVal);
	// UTC_END


	// END PUBLIC INTERFACE


};
#ifndef __sun__
#pragma pack (pop)
#endif



#endif


