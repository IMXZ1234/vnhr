#pragma once
#include <vector>
#include <map>

#define DEFAULT_MAX_ID_NUM	1024
#define IDALLOCATOR_TIMER	0

// Used when a number of different int(unsigned int) IDs are needed.
// Maps IDs to int(unsigned int) values.
// Since for the whole instance different timers should have different ids,
// while timers and commands having same ids will not be confused, 
// a allocator pool is maintained to make sure that each type of id has its own allocator which is shared across the instance.
class IDAllocator
{
public:
	static IDAllocator* GetIDAllocatorFor(int id_type);

	explicit IDAllocator(int min_id, int max_id);
	~IDAllocator() = default;
	int AllocateID();
	int SetValueAtID(int id, int value);
	int GetValueAtID(int id);
	bool SetIDRange(int min_id, int max_id);
	int DeallocateID(int id);
	bool IsIDValid(int id);
private:
	static std::map<int, IDAllocator*> allocator_pool;

	std::vector<int> id_vector_;
	int min_id_;
};

