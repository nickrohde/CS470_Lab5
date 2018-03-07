#include "PCB.h"
#include "Queues.h"
#include <random>
#include <queue>
#include <vector>
#include <unordered_set>
#include <mutex>
#include <stack>
//#include <unistd.h>
//#include <pthread.h>
#include <thread>
#include <deque>

#ifndef NULL
#define NULL 0
#endif

#define DEBUG 0

using namespace std;

// Globals:
//priority_queue<PCB, vector<PCB>, CompareBySpot> procOne;
//priority_queue<PCB> procTwo;
MinHeap procOne(true);
MinHeap procTwo(false);



bool p1Done;
bool p2Done;

mutex threadLock;

// Prototypes:
void  printArgError(const char*, const char*, const char*);
void  printArgError(const char*);
void* startSimProc1(void* args);
void* startSimProc2(void* args);
void* ageProcesses(void* args);
void* stateRandomizer(void* args);
void* balanceLoad(void* args);


int main(int argc, char ** argv)
{
	time_t t_PROC_1_QUANTUM = 1000;
	time_t t_PROC_2_QUANTUM = 500;

	int numProcessesOne;
	int numProcessesTwo;

	if (argc == 3)
	{
		numProcessesOne = atoi(argv[1]);
		numProcessesTwo = atoi(argv[2]);

		if (numProcessesOne <= 0 || numProcessesTwo <= 0)
		{
			printArgError(argv[0], argv[1], argv[2]);
			exit(EXIT_FAILURE);
		} // end if
	} // end if
	else
	{
		printArgError(argv[0]);
		system("pause");
		exit(EXIT_FAILURE);
	} // end else

	srand((unsigned)time(NULL));

	p1Done = false;
	p2Done = false;
	unordered_set<int> taken;

	for(int j = 0; j < 2; j++)
	{
		int id = 0,
			priority = 0;
		time_t burst = 0;
		State state = State::Ready;
		int number = (j == 0 ? numProcessesOne : numProcessesTwo);

		for (int i = 0; i < number; i++)
		{
			do
			{
				id = rand() + 1;
			} while (taken.count(id) != 0);

			taken.insert(id);

			priority = rand() % 128;

			burst = static_cast<time_t>((rand() % 10) + 1);
			burst *= 500;

			state = State::Ready;

			PCB pcb = PCB(id, priority, burst, state, i);
			j == 0 ? procOne.push(pcb) : procTwo.push(pcb);
		} // end for i
	} // end for j

	/*
	pthread_t threads[5];

	pthread_create(&threads[0], NULL, startSimProc1, ((void*)t_PROC_1_QUANTUM));
	pthread_create(&threads[1], NULL, startSimProc2, ((void*)t_PROC_2_QUANTUM));
	pthread_create(&threads[2], NULL, ageProcesses, NULL);
	pthread_create(&threads[3], NULL, stateRandomizer, NULL);
	pthread_create(&threads[4], NULL, balanceLoad, NULL);
	*/

	
	thread threads[5];
	threads[0] = thread(startSimProc1, (void*)t_PROC_1_QUANTUM);
	threads[1] = thread(startSimProc2, (void*)t_PROC_2_QUANTUM);
	threads[2] = thread(ageProcesses, (void*)NULL);
	threads[3] = thread(stateRandomizer, (void*)NULL);
	threads[4] = thread(balanceLoad, (void*)NULL);

	for (int i = 0; i < 5; i++)
	{
		threads[i].join();
	} // end for
	
	//startSimProc2((void*)t_PROC_1_QUANTUM);
	system("pause");


	return 0;
} // end Main


void printArgError(const char* name)
{
	cout << "Invalid arguments received!" << endl;
	cout << "You must supply exactly 2 arguments." << endl;
	cout << "Example call: " << name << " 4 17" << endl;
} // end method printArgError(1)


void printArgError(const char* name, const char* one, const char* two)
{
	cout << "Invalid arguments received!" << endl;
	cout << "The number of processes must be positive integers." << endl;
	cout << "The arguements received were: " << one << " and " << two << endl;
	cout << "Example call: " << name << " 4 17" << endl;
} // end method printArgError(3)


void* startSimProc1(void* args)
{
	const time_t t_QUANTUM = (time_t)args;
	const int    i_PROCNUM = 1;
	bool b_success = false;
	stack<PCB> onHold;
	
	if(procOne.empty())
	{
		while (!threadLock.try_lock());
		//threadLock.lock();
		p1Done = true;
		threadLock.unlock();
	} // end if
	
	if (DEBUG)
		p2Done = true;
	
	while (!p1Done || !p2Done)
	{
		PCB temp;

		do
		{
			if (!procOne.empty())
			{
				while (!threadLock.try_lock());

				temp = procOne.top();
				procOne.pop();				

				b_success = temp.execute(t_QUANTUM, i_PROCNUM);

				if (!b_success)
				{
					if (!procOne.empty())
					{
						onHold.push(temp);
					} // end if
					else if (temp.getState() != State::Terminated)
					{
						procOne.push(temp);
					} // end elif
				} // end if
				else if (temp.getState() != State::Terminated)
				{
					procOne.push(temp);
				} // end elif
				
				threadLock.unlock();

				this_thread::sleep_for(200ms);
			} // end if !empty
			else
			{
				break;
			} // end else
		} while (!b_success);
		
		while (!onHold.empty())
		{
			PCB temp = onHold.top();
			onHold.pop();
			while (!threadLock.try_lock());
			procOne.push(temp);
			threadLock.unlock();
		} // end while
		
		while (!threadLock.try_lock());

		cout << "[Processor " << i_PROCNUM << "]: Processes left: " << procOne.size() << endl;

		if(procOne.empty())
		{
			p1Done = true;
		} // end if
		threadLock.unlock();

		while (p1Done && !p2Done);

	} // end while
	
	return NULL;
} // end method startSimProc1


void* startSimProc2(void* args)
{
	const time_t t_QUANTUM = (time_t)args;
	const int    i_PROCNUM = 2;
	bool b_success = false;
	stack<PCB> onHold;
	
	if (DEBUG)
		p1Done = true;
		
	if(procTwo.empty())
	{
		p2Done = true;
	} // end if
	
	while (!p1Done || !p2Done)
	{
		PCB temp;

		do
		{
			if (!procTwo.empty())
			{
				while (!threadLock.try_lock());

				temp = procTwo.top();
				procTwo.pop();
				b_success = temp.execute(t_QUANTUM, i_PROCNUM);				

				if (!b_success)
				{
					if (!procTwo.empty())
					{
						onHold.push(temp);
					} // end if
					else if (temp.getState() != State::Terminated)
					{
						procTwo.push(temp);
					} // end elif
				} // end if
				else if (temp.getState() != State::Terminated)
				{
					procTwo.push(temp);
				} // end elif
				
				threadLock.unlock();
				this_thread::sleep_for(200ms);
			} // end if !empty
			else
			{
				break;
			} // end else
		} while (!b_success);
		
		while (!onHold.empty())
		{
			PCB temp = onHold.top();
			onHold.pop();
			while (!threadLock.try_lock());
			procTwo.push(temp);
			threadLock.unlock();
		} // end while
		
		while (!threadLock.try_lock());

		cout << "[Processor " << i_PROCNUM << "]: Processes left: " << procTwo.size() << endl;

		if(procTwo.empty())
		{
			p2Done = true;
		} // end if
		threadLock.unlock();

		while (!p1Done && p2Done);

	} // end while
	
	return NULL;
} // end method startSimProc2


void* ageProcesses(void* args)
{
	const time_t FREQUENCY = 2;

	while (!p1Done || !p2Done)
	{
		//sleep(FREQUENCY);
		this_thread::sleep_for(2s);
		while (!threadLock.try_lock());
		//threadLock.lock();
		
		stack<PCB> pile;
		
		while(!procOne.empty())
		{
			PCB temp = procOne.top();
			procOne.pop();
			temp.age();
			pile.push(temp);
		} // end while
		
		while(!pile.empty())
		{
			PCB temp = pile.top();
			pile.pop();
			procOne.push(temp);
		} // end while
		
		while(!procTwo.empty())
		{
			PCB temp = procTwo.top();
			procTwo.pop();
			temp.age();
			pile.push(temp);
		} // end while
		
		while(!pile.empty())
		{
			PCB temp = pile.top();
			pile.pop();
			procTwo.push(temp);
		} // end while
		
		threadLock.unlock();
	} // end while
	return NULL;
} // end method ageProcesses


void* stateRandomizer(void* args)
{
	const time_t FREQUENCY = 5;
	
	while(!p1Done || !p2Done)
	{
		//sleep(FREQUENCY);
		this_thread::sleep_for(5s);
		while (!threadLock.try_lock());
		
		stack<PCB> pile;
		
		while(!procOne.empty())
		{
			PCB temp = procOne.top();
			procOne.pop();
			temp.newState();
			pile.push(temp);
		} // end while
		
		while(!pile.empty())
		{
			PCB temp = pile.top();
			pile.pop();
			procOne.push(temp);
		} // end while
		
		while(!procTwo.empty())
		{
			PCB temp = procTwo.top();
			procTwo.pop();
			temp.newState();
			pile.push(temp);
		} // end while
		
		while(!pile.empty())
		{
			PCB temp = pile.top();
			pile.pop();
			procTwo.push(temp);
		} // end while
		
		threadLock.unlock();
	} // end while

	return NULL;
} // end method stateRandomizer


void* balanceLoad(void* args)
{
	int processesToMove;

	while (!p1Done || !p2Done)
	{
		while (!p1Done && !p2Done);
		
		if (procOne.size() > 1 || procTwo.size() > 1)
		{
			threadLock.lock();
			
			cout << "[Balancer]: Processor 1 has " << procOne.size() << " process(es) left." << endl;
			cout << "[Balancer]: Processor 2 has " << procTwo.size() << " process(es) left." << endl;
			cout << "[Balancer]: Rebalancing ..." << endl;
			
			threadLock.unlock();
		} // end if
		else
		{
			break;
		} // end else

		if (p1Done && !p2Done)
		{
			threadLock.lock();

			processesToMove = procTwo.size() / 2;

			if (p1Done && !p2Done && processesToMove > 0)
			{
				cout << "[Balancer]: Processor 1 is finished, moving " << processesToMove << " process(es) from processor 2 to processor 1." << endl;
				for (int i = 0; i < processesToMove && procTwo.size() > 0; i++)
				{
					PCB temp = procTwo.top();
					procTwo.pop();
					procOne.push(temp);
				} // end for
				cout << "[Balancer]: Done rebalancing load." << endl;
				cout << "[Balancer]: Processor 1 now has " << procOne.size() << " process(es)." << endl;
				cout << "[Balancer]: Processor 2 now has " << procTwo.size() << " process(es)." << endl;
				p1Done = false;
			} // end if
			else if (processesToMove == 0)
			{
				cout << "[Balancer]: Not enough processes to rebalance." << endl;
			} // end elif

			threadLock.unlock();
		} // end if
		else if (!p1Done && p2Done)
		{
			threadLock.lock();

			processesToMove = procOne.size() / 2;
			if (!p1Done && p2Done && processesToMove > 0)
			{
				cout << "[Balancer]: Processor 2 is finished, moving " << processesToMove << " process(es) from processor 1 to processor 2." << endl;
				for (int i = 0; i < processesToMove && procOne.size() > 0; i++)
				{
					PCB temp = procOne.top();
					procOne.pop();
					procTwo.push(temp);
				} // end for
				cout << "[Balancer]: Done rebalancing load." << endl;
				cout << "[Balancer]: Processor 1 now has " << procOne.size() << " process(es)." << endl;
				cout << "[Balancer]: Processor 2 now has " << procTwo.size() << " process(es)." << endl;
				p2Done = false;
			} // end if
			else if (processesToMove == 0)
			{
				cout << "[Balancer]: Not enough processes to rebalance." << endl;
			} // end elif

			threadLock.unlock();
		} // end elif
	} // end while

	return NULL;
} // end method balanceLoad