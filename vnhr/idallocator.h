#pragma once
#include <vector>

#define DEFAULT_MAX_ID_NUM 32

// Used when a number of different int(unsigned int) IDs are needed.
// Maps IDs to int(unsigned int) values.
class IDAllocator
{
public:
	explicit IDAllocator(int min_id, int max_id);
	~IDAllocator() = default;
	int AllocateID();
	int SetValueAtID(int id, int value);
	int GetValueAttID(int id);
	bool SetIDRange(int min_id, int max_id);
	int DeallocateID(int id);
	bool IsIDValid(int id);
private:
	std::vector<int> id_vector_;
	int min_id_;
};

