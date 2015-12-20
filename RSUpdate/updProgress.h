#pragma once

class updProgress
{
  public:
	__int64		total;
	__int64		cur;

	updProgress() {
	  total = 0;
	  cur   = 0;
	}
	
	void		set(__int64 t) {
	  total = t;
	  cur   = 0;
	}

	float		getCoef() const {
	  if(total == 0)
	    return 0.0f;
          float k = (float)cur / (float)total;
          if(k > 1.0f) k = 1.0f;
          return k;
	}
};

