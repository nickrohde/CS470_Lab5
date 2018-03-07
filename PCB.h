#include <time.h>
#include <iostream>

#ifndef __PCB_H
#define __PCB_H

enum State
{
	New, Waiting, Ready, Executing, Terminated
};

class PCB
{
public:
	int getID(void);
	int	getPriority(void);
	int getSpot(void) const;
	
	time_t getBurstTime(void);
	State getState(void);

	void terminatedMessage(const int);

	bool execute(time_t, int);
	void changeState(State);

	void age(int ageBy = 1);
	void newState(void);
	
	PCB(void);
	PCB(int, int, time_t, State, int spot = 0);
	PCB(const PCB&);
	~PCB(void);

	PCB& operator=(const PCB& rhs);

	bool operator<(const PCB& rhs);
	bool operator<=(const PCB& rhs);
	bool operator==(const PCB& rhs);
	bool operator!=(const PCB& rhs);
	bool operator>=(const PCB& rhs);
	bool operator>(const PCB& rhs);

	friend std::ostream& operator<<(std::ostream&, PCB&);
	friend bool operator<(const PCB& lhs, const PCB& rhs);
	friend bool operator<=(const PCB& lhs, const PCB& rhs);
	friend bool operator==(const PCB& lhs, const PCB& rhs);
	friend bool operator!=(const PCB& lhs, const PCB& rhs);
	friend bool operator>=(const PCB& lhs, const PCB& rhs);
	friend bool operator>(const PCB& lhs, const PCB& rhs);

//	class CompareBySpot
//	{
//	public:
//		bool operator()(const PCB& lhs, const PCB& rhs)
//		{
//			return lhs.i_spot < rhs.i_spot;
//		} // end operator()
//	};

private:
	int i_ID,
		i_priority,
		i_spot;

	time_t t_burst;
	State current_state;
};


struct CompareBySpot
{
	bool operator()(const PCB& lhs, const PCB& rhs) const
	{
		bool b = lhs.getSpot() > rhs.getSpot();
		return b;
	} // end operator (a,b)
};

#endif
