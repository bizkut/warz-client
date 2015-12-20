#ifndef TIMER_H
#define TIMER_H

class Timer
{
	public:
		Timer();
		void Reset();
		int Time() const;
		int Seconds() const;
		void Set(int count){Count = count;};
		void Decrease(int count) {Count += count;}
    void Increase(int count) {Count -= count;}
		int GetCount() { return Count; }
	private:
		int Count;
};

#endif

