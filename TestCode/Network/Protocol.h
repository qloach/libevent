#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#progma pack(1)

#define u8 unsigned char
#define s8 char
#define u16 unsigned short
#define s16 short
#define u32 unsigned int
#define s32 int
#define u64 unsigned long long
#define s64 long long

class NetBuff
{
	u16 Len;   // not include the size of the len
	u8 Buff[0];
};

class ProBase
{
	u16 Id;
};

class ChatMsg : public ProBase
{
	u16 Len;	// not include the size of the Len
	u8 Context[0];
};












#progma pack()
#endif