#include <memory.h>
#include "RingBuffer.h"


RingBuffer::RingBuffer()
{
	Data = new char[RING_BUFFER_MAX_SIZE];
	ReadIndex = WriteIndex = 0;
}

RingBuffer::~RingBuffer()
{
	if (Data)
	{
		delete []Data;
	}
}

RingBufferError RingBuffer::PushBack(char* InData, int InSzie)
{
	if (InSzie <= 0)
	{
		return Error_NoError;
	}

	if (InSzie > RING_BUFFER_MAX_SIZE - GetSize())
	{
		return Error_NoEnoughSpace;
	}
	int NewIdx = WriteIndex + InSzie;
	if (NewIdx <= RING_BUFFER_MAX_SIZE)
	{
		memcpy(&Data[WriteIndex], InData, InSzie);
		WriteIndex = NewIdx % RING_BUFFER_MAX_SIZE;
	}
	else
	{
		memcpy(&Data[WriteIndex], InData, RING_BUFFER_MAX_SIZE - WriteIndex);
		memcpy(&Data[0], &InData[RING_BUFFER_MAX_SIZE - WriteIndex], InSzie - RING_BUFFER_MAX_SIZE + WriteIndex);
		WriteIndex = InSzie - RING_BUFFER_MAX_SIZE + WriteIndex;
	}
	return Error_NoError;
}

RingBufferError RingBuffer::PopFront(char* OutData, int InSize)
{
	if (InSize > GetSize())
	{
		return Error_NoEnoughData;
	}

	int NewIdx = ReadIndex + InSize;
	if (NewIdx <= RING_BUFFER_MAX_SIZE)
	{
		memcpy(OutData, &Data[ReadIndex], InSize);
		ReadIndex = NewIdx % RING_BUFFER_MAX_SIZE;
	}
	else
	{
		memcpy(OutData, &Data[ReadIndex], RING_BUFFER_MAX_SIZE - ReadIndex);
		memcpy(&OutData[RING_BUFFER_MAX_SIZE - ReadIndex], Data, InSize - RING_BUFFER_MAX_SIZE + ReadIndex);
		ReadIndex = InSize - RING_BUFFER_MAX_SIZE + ReadIndex;
	}

	return Error_NoError;
}

int RingBuffer::GetSize()
{
	if (ReadIndex <= WriteIndex)
	{
		return WriteIndex - ReadIndex;
	}
	else
	{
		return RING_BUFFER_MAX_SIZE - 1 - ReadIndex + WriteIndex;
	}
}