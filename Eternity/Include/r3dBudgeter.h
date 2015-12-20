#ifndef R3DBUDGETER_H_INCLUDED
#define R3DBUDGETER_H_INCLUDED

#include "r3d.h"

class r3dBudgeter
{
public:

	struct Data 
	{
		int values[BUDGET_CATEGORY_COUNT];
		void Defaults();
		void Save(const char* fileName) const;
		void Load(const char* fileName);
	};

	void Init(const char* fileName);
	void Draw();
	void UpdateCurrent();

	Data currentData;
	Data limitData;

private:
	float DrawGauge(int category, int index);
	float GetValue(int category, r3dColor& color) const;
	bool IsSkipped(int category);
};

extern r3dBudgeter gBudgeter;


#endif