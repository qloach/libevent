#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <map>
using namespace std;

struct bufferevent;

class QServerNetwork
{
public:
	QServerNetwork();
	~QServerNetword();
	bool Init(char* ipstr, int port);
	bool UnInit();

	void Tick(float DeltaTime);

private:
	map<int, bufferveent*> Sessions;
};

class QClientNetwork
{
public:
	QClientNetwork();
	~QClientNetwork();

	bool Init(char* ipstr, int port);
	bool UnInit();

	void Tick(float DeltaTime);
};


#endif