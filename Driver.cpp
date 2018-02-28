#include "PCB.h"
#include "Queues.h"
#include <random>
#include <queue>
#include <vector>
#include <unordered_set>
#include <mutex>


using namespace std;

// Globals:
iterable_queue<PCB> procOne;
iterable_priority_queue<PCB> procTwo;

bool p1Done;
bool p2Done;

mutex threadLock;

// Prototypes:
void printArgError(const char*, const char*, const char*);
void printArgError(const char*);


int main(int argc, char ** argv)
{
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
		int id, priority;
		time_t burst;
		State state;

		int number = (j == 0 ? numProcessesOne : numProcessesTwo);

		for (int i = 0; i < number; i++)
		{
			do
			{
				id = rand() + 1;
			} while (taken.count(id) != 0);

			taken.insert(id);

			priority = rand() % 128;

			burst = static_cast<time_t>((rand() % 5) + 1);
			burst *= 500;

			state = State::Ready;

			PCB pcb = PCB(id, priority, burst, state);
			j == 0 ? procOne.push(pcb) : procTwo.push(pcb);
		} // end for i

		taken.clear();
	} // end for j

	startSimProc1(NULL);
	startSimProc2(NULL);
	ageProcesses(NULL);

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
	const time_t QUANTUM = 1;

	list<PCB> onHold;

	while (!p1Done || !p2Done)
	{
		if (procOne.front().getState == State::Ready)
		{
			threadLock.lock();
			PCB temp = procOne.front();
			procOne.pop();
			temp.execute(QUANTUM, 1);
			threadLock.unlock();
		}
	}


	return NULL;
} // end method startSimProc1


void* startSimProc2(void* args)
{
	return NULL;
} // end method startSimProc2


void* ageProcesses(void* args)
{
	while (!p1Done || !p2Done)
	{
		threadLock.lock();

		for (auto& x : procOne)
		{
			x.age();
		} // end foreach
		for (auto& x : procTwo)
		{
			x.age();
		} // end foreach

		threadLock.unlock();
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
	} // end while
	return NULL;
} // end method ageProcesses


void* balanceLoad(void* args)
{
	int processesToMove;

	while (!p1Done || !p2Done)
	{
		while (!p1Done && !p2Done);

		if (p1Done)
		{
			threadLock.lock();

			processesToMove = procTwo.size() / 2;

			if (p1Done && !p2Done && processesToMove > 0)
			{
				cout << "[Balancer]: Processor 1 is finished, moving " << processesToMove << " processes from processor 2 to processor 1." << endl;
				for (int i = 0; i < processesToMove && procTwo.size() > 0; i++)
				{
					PCB temp = procTwo.top();
					procTwo.pop();
					procOne.push(temp);
				} // end for
				cout << "[Balancer]: Done rebalancing load. Both processors now have " << processesToMove << " processes." << endl;
				p1Done = false;
			} // end if

			threadLock.unlock();
		} // end if
		else if (p2Done)
		{
			threadLock.lock();

			processesToMove = procOne.size() / 2;
			if (!p1Done && p2Done && processesToMove > 0)
			{
				cout << "[Balancer]: Processor 2 is finished, moving " << processesToMove << " processes from processor 1 to processor 2." << endl;
				for (int i = 0; i < processesToMove && procOne.size() > 0; i++)
				{
					PCB temp = procOne.front();
					procOne.pop();
					procTwo.push(temp);
				} // end for
				cout << "[Balancer]: Done rebalancing load. Both processors now have " << processesToMove << " processes." << endl;
				p2Done = false;
			} // end if

			threadLock.unlock();
		} // end elif
	} // end while
} // end method balanceLoad