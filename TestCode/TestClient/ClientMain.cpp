#include <event2/event.h>
#include <event2/bufferevent.h>
#include <string.h>

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libevent_core.lib")

void eventcb(struct bufferevent *bev, short events, void *ptr)
{
	int a;
    if (events & BEV_EVENT_CONNECTED) 
	{
         /* We're connected to 127.0.0.1:8080.   Ordinarily we'd do
            something here, like start reading or writing. */
		a = 1;
		
    } else if (events & BEV_EVENT_ERROR) 
	{
         /* An error occured while connecting. */
		a = 2;
    }
}

int main()
{
	WSADATA wsa_data;
	WSAStartup(0x0201, &wsa_data);
    event_base *base;
    bufferevent *bev;
    sockaddr_in sin;

    base = event_base_new();

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(0x7f000001); /* 127.0.0.1 */
    sin.sin_port = htons(9995);

    bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(bev, NULL, NULL, eventcb, NULL);

    if (bufferevent_socket_connect(bev,
        (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        /* Error starting connection */
        bufferevent_free(bev);
        return -1;
    }

    event_base_dispatch(base);
    return 0;
}