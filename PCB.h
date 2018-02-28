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
	int getSpot(void);
	
	time_t getBurstTime(void);
	State getState(void);

	bool execute(time_t, int);
	bool changeState(State);

	void age(int ageBy = 1);

	PCB(int, int, time_t, State, int spot = 0);
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

	class CompareBySpot
	{
		bool operator()(const PCB& lhs, const PCB& rhs)
		{
			return lhs.i_spot < rhs.i_spot;
		} // end operator()
	};

private:
	int i_ID,
		i_priority,
		i_spot;

	time_t t_burst;
	State current_state;
};

#endif
