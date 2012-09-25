#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <iostream>
using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libevent_core.lib")

bufferevent* MainBev;
bool bCanWrite;

void eventcb(struct bufferevent *bev, short events, void *ptr)
{
    if (events & BEV_EVENT_CONNECTED)
	{
		MainBev = bev;
		cout<<"event & bev_event_connected"<<endl;
    }
	else if (events & (BEV_EVENT_ERROR|BEV_EVENT_EOF)) 
	{
         struct event_base *base = (event_base*)ptr;
         if (events & BEV_EVENT_ERROR) 
		 {
			 cout<<"error"<<endl;
         }
         cout<<"bufferevent close!"<<endl;
         bufferevent_free(bev);
         event_base_loopexit(base, NULL);
    }
}

void readcb(struct bufferevent *bev, void *ptr)
{
    char buf[1024];
	memset(buf, 0, 1024);
    int n;
    struct evbuffer *input = bufferevent_get_input(bev);
    while ((n = evbuffer_remove(input, buf, sizeof(buf))) > 0) 
	{
		cout<<buf<<endl;
    }
}

static void writecb(struct bufferevent* bev, void* user_data)
{
	struct evbuffer* output = bufferevent_get_output(bev);
	int len = evbuffer_get_length(output);
	cout<<"evbuffer length == 0"<<endl;
	bCanWrite = true;
}

void SendData(char* data, int size)
{
	if(MainBev)
	{
		int ret = bufferevent_write(MainBev, data, size);
		cout<<"write eventbuffer ret="<<ret<<endl;
		//evbuffer* output = bufferevent_get_output(MainBev);
		//if(output)
		//{
		//	bool ret = evbuffer_write(output, (void*)data, size);
		//	cout<<"write result is "<<ret<<endl;
		//}
	}
	else
	{
		cout<<"MainBev is null ,can not send buff"<<endl;
	}
}

int main()
{
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
    event_base *base;
    bufferevent *bev;
    sockaddr_in sin;

	MainBev = NULL;
	bCanWrite = false;

    base = event_base_new();

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(0x7f000001); /* 127.0.0.1 */
    sin.sin_port = htons(9995);

    bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(bev, readcb, writecb, eventcb, NULL);

    if (bufferevent_socket_connect(bev, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
        /* Error starting connection */
        bufferevent_free(bev);
        return -1;
    }

    event_base_dispatch(base);

	char input[1024];
	memset(input, 0, 1024);
	while(true)
	{
		memset(input, 0, 1024);
		cin>>input;
		SendData(input, strlen(input));
	}
	

    return 0;
}