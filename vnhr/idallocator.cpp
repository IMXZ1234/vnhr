#include "idallocator.h"

IDAllocator::IDAllocator(int min_id, int max_id)
{
	if (min_id < 0)
		min_id = 0;
	if (max_id < 0)
		max_id = DEFAULT_MAX_ID_NUM + min_id;
	id_vector_.resize(max_id - min_id);
	min_id_ = min_id;
}

int IDAllocator::AllocateID()
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

int IDAllocator::SetValueAtID(int id, int value)
{
	int pos = id - min_id_;
	int old_value;
	if (pos >= 0 && pos < id_vector_.size())
	{
		old_value = id_vector_[pos];
		id_vector_[pos] = value;
		return old_value;
	}
	else
	{
		return -1;
	}
}

int IDAllocator::GetValueAttID(int id)
{
	int pos = id - min_id_;
	if (pos >= 0 && pos < id_vector_.size())
		return id_vector_[id - min_id_];
}

bool IDAllocator::SetIDRange(int min_id, int max_id)
{
	if (min_id < 0)
		min_id = 0;
	if (max_id < 0)
		max_id = DEFAULT_MAX_ID_NUM + min_id;
	id_vector_.clear();
	id_vector_.resize(max_id - min_id);
	this->min_id_ = min_id;
	return true;
}

int IDAllocator::DeallocateID(int id)
{
	int pos = id - min_id_;
	int old_value;
	if (pos >= 0 && pos < id_vector_.size())
	{
		old_value = id_vector_[pos];
		id_vector_[pos] = NULL;
		return old_value;
	}
	else
	{
		return -1;
	}
}

bool IDAllocator::IsIDValid(int id)
{
	int pos = id - min_id_;
	if (pos >= 0 && pos < id_vector_.size())
		return true;
	else
		return false;
}


