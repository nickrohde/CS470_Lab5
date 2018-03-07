#include "PCB.h"
#include <Windows.h>
#include <chrono>
#include <thread>

//#include <unistd.h>

int PCB::getID(void)
{
	return i_ID;
} // end method getID


int PCB::getPriority(void)
{
	return i_priority;
} // end method getPriority


int PCB::getSpot(void) const 
{
	return i_spot;
}


time_t PCB::getBurstTime(void)
{
	return t_burst;
} // end method getBurstTime


void PCB::newState(void)
{
	if(current_state == State::New || current_state == State::Waiting || current_state == State::Ready)
	{
		if(rand() % 2)
		{
			std::cout << "[Process " << i_ID << "]: Old state: " << (current_state == State::Ready ? "Ready" : "Waiting") << "; New state: Waiting" << std::endl;
			current_state = State::Waiting;
		} // end if
		else
		{
			std::cout << "[Process " << i_ID << "]: Old state: " << (current_state == State::Ready ? "Ready" : "Waiting") << "; New state: Ready" << std::endl;
			current_state = State::Ready;
		} // end else
	} // end if
} // end method newState


bool PCB::execute(time_t burst, int processor)
{
	if (current_state == State::Ready)
	{
		current_state = State::Executing;
		std::cout << "[Processor " << processor << "]: Executing: " << std::endl;
		std::cout << *this << std::endl;
		if (t_burst > burst)
		{
			t_burst -= burst; 
			//sleep(burst);
			current_state = State::Ready;
		} // end if
		else
		{
			//sleep(burst);
			t_burst = 0;
			current_state = State::Terminated;
			terminatedMessage(processor);
		} // end else
		std::cout << "[Processor " << processor << "]: Finished executing: " << std::endl;
		std::cout << *this << std::endl;

		return true;
	} // end if
	else
	{
		std::cout << "[Processor " << processor << "]: Process " << i_ID << " is next, but it's not ready yet. Picking new process to execute ..." << std::endl;
		return false;
	} // end else

} // end method execute


void PCB::changeState(State newState)
{
	current_state = newState;
} // end method changeState


void PCB::age(int ageBy)
{
	if (i_priority >= ageBy)
	{
		std::cout << "[Process " << i_ID << "]: is aging from " << i_priority << " to " << i_priority-ageBy << std::endl;
		i_priority -= ageBy;
	} // end if
} // end method age


State PCB::getState(void)
{
	return current_state;
} // end method getState

void PCB::terminatedMessage(const int i_PROC_NUM)
{
	std::cout << "[Process " << i_ID << "]: finished execution, terminating." << std::endl;
} // end terminated Message


PCB::PCB(void)
{
	i_ID = 0;
	i_priority = 0;
	i_spot = 0;
	t_burst = 0;
	current_state = State::Terminated;
}

PCB::PCB(int id, int priority, time_t burst, State state, int spot)
{
	i_ID = id;
	i_priority = priority;
	i_spot = spot;
	t_burst = burst;
	current_state = state;
} // end Constructor

PCB::PCB(const PCB & other)
{
	i_ID = other.i_ID;
	i_priority = other.i_priority; 
	t_burst = other.t_burst;
	current_state = other.current_state;
	i_spot =  other.i_spot;
} // end Copy Constructor


PCB::~PCB(void)
{
} // end Destructor


PCB& PCB::operator=(const PCB & rhs)
{
	i_ID = rhs.i_ID;
	i_spot = rhs.i_spot;
	i_priority = rhs.i_priority;
	t_burst = rhs.t_burst;
	current_state = rhs.current_state;

	return *this;
} // end copy assignment


bool PCB::operator<(const PCB & rhs)
{
	return i_priority < rhs.i_priority;
} // end operator <


bool PCB::operator<=(const PCB & rhs)
{
	return *this < rhs || *this == rhs;
} // end operator <=


bool PCB::operator==(const PCB & rhs)
{
	return i_priority == rhs.i_priority;
} // end operator ==


bool PCB::operator!=(const PCB & rhs)
{
	return !(*this == rhs);
} // end operator !=


bool PCB::operator>=(const PCB & rhs)
{
	return *this > rhs || *this == rhs;
} // end operator >=


bool PCB::operator>(const PCB & rhs)
{
	return !(*this <= rhs);
} // end operator >


std::ostream& operator<<(std::ostream & output, PCB& process)
{
	output << "\nProcess ID: " << process.i_ID;
	output << "\nCurrent priority: " << process.i_priority;
	output << "\nCurrent State: ";

	switch (process.current_state)
	{
	case State::New:
		output << " New";
		break;
	case State::Waiting:
		output << " Waiting";
		break;
	case State::Executing:
		output << " Executing";
		break;
	case State::Ready:
		output << " Ready";
		break;
	case State::Terminated:
		output << " Terminated";
		break;
	} // end switch

	output << "\nRemaining burst time: " << process.t_burst << " ms.\n";

	return output;
} // end operator <<


bool operator<(const PCB & lhs, const PCB & rhs)
{
	return lhs.i_priority < rhs.i_priority;
} // end operator <


bool operator<=(const PCB & lhs, const PCB & rhs)
{
	return lhs < rhs || lhs == rhs;
} // end operator <=


bool operator==(const PCB & lhs, const PCB & rhs)
{
	return lhs.i_priority == rhs.i_priority;
} // end operator ==


bool operator!=(const PCB & lhs, const PCB & rhs)
{
	return !(lhs == rhs);
} // end operator !=


bool operator>=(const PCB & lhs, const PCB & rhs)
{
	return !(lhs < rhs);
} // end operator >=


bool operator>(const PCB & lhs, const PCB & rhs)
{
	return !(lhs <= rhs);
} // end operator >

