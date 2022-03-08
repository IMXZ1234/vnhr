#include "idallocator.h"

IDAllocator::IDAllocator(int min_id, int max_id)
{
	id_vector_.resize(max_id - min_id);
	min_id_ = min_id;
}

inline int IDAllocator::AllocateID()
{
	int i;
	for (i = 0; i < id_vector_.size(); ++i)
	{
		if (id_vector_[i] != NULL)
		{
			return i + min_id_;
		}
	}
	return -1;
}

inline int IDAllocator::SetValueAtID(int id, int value)
{
	id_vector_[id - min_id_] = value;
}

inline int IDAllocator::GetValueAttID(int id)
{
	return id_vector_[id - min_id_];
}

inline bool IDAllocator::SetIDRange(int min_id, int max_id)
{
	id_vector_.clear();
	id_vector_.resize(max_id - min_id);
	this->min_id_ = min_id;
	return true;
}

inline int IDAllocator::DeallocateID(int id)
{
	id_vector_[id - min_id_] = NULL;
}
