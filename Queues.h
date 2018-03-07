#include <queue>
#include <deque>
#include <iostream>
#include <vector>
#include <functional>
#include <algorithm>
#include "PCB.h"


#ifndef __QUEUES_
#define __QUEUES_

using namespace std;


class MinHeap
{
public:
	vector<PCB> heap;
	bool sortByPriority;
	
	MinHeap(bool order) 
	{
		sortByPriority = order;
	} // end constructor

	~MinHeap(void) {} // end destructor
	
	PCB& top(void)
	{
		if(!heap.empty())
		{
			PCB& temp(heap.at(heap.size()-1));
			return temp;
		} // end if
		else
		{
			throw new invalid_argument("heap is empty");
		} // end else
	} // end method top
	
	void pop(void)
	{
		if(!heap.empty())
		{
			heap.pop_back();
		} // end if
	} // end method pop

	void push(PCB& pcb)
	{
		heap.insert(heap.begin(), pcb);
		if(sortByPriority)
		{
			std::sort(heap.begin(),heap.end(), std::greater<PCB>());
		} // end if
		else
		{
			std::sort(heap.begin(),heap.end(), CompareBySpot());
		} // end else
	} // end method push
	
	int size(void) const
	{
		return heap.size();
	} // end method size
	
	bool empty(void)
	{
		return heap.empty();
	} // end method empty
}; // end class MinHeap




#endif // !__QUEUES_

