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
	inline int AllocateID();
	inline int SetValueAtID(int id, int value);
	inline int GetValueAttID(int id);
	inline bool SetIDRange(int min_id, int max_id);
	inline int DeallocateID(int id);
private:
	std::vector<int> id_vector_;
	int min_id_;
};

