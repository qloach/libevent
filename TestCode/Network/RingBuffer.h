#ifndef _RING_BUFFER_H_
#define _RING_BUFFER_H_

#define RING_BUFFER_MAX_SIZE 65535

enum RingBufferError
{
	Error_NoError,
	Error_NoEnoughSpace,
	Error_NoEnoughData,
};

class RingBuffer
{
private:
	char* Data;
	int ReadIndex;
	int WriteIndex;

public:
	RingBuffer();
	~RingBuffer();
	RingBufferError PushBack(char* InData, int InSzie);

	RingBufferError PopFront(char* OutData, int InSize);

	int GetSize();
};


#endif