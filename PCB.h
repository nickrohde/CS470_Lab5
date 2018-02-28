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
	
	time_t getBurstTime(void);
	State getState(void);

	bool execute(time_t, int);
	bool changeState(State);

	void age(int ageBy = 1);

	PCB(int, int, time_t, State);
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


private:
	int i_ID,
		i_priority;

	time_t t_burst;
	State current_state;
};

#endif
