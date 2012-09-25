#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>

#include "event2/bufferevent.h"
#include "event2/buffer.h"
#include "event2/listener.h"
#include "event2/util.h"
#include "event2/event.h"
#include "event2/event_compat.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libevent_core.lib")

static const u_short PORT = 9995;

static void listener_cb(struct evconnlistener*, evutil_socket_t, struct sockaddr*, int socklen, void*);
static void conn_readcb(struct bufferevent*, void*);
static void conn_writecb(struct bufferevent*, void*);
static void conn_eventcb(struct bufferevent*, short, void*);
static void signal_cb(evutil_socket_t, short, void*);

int main(int argc, char** argv)
{
	WSADATA wsa_data;
	struct event_base* base;
	struct evconnlistener* listener;
	struct event* signal_event;
	struct sockaddr_in sin;
	WSAStartup(0x0201, &wsa_data);

	base = event_base_new();
	if (!base)
	{
		fprintf(stderr, "Could not initialize libevent\n");
		return 1;
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);

	listener = evconnlistener_new_bind(
		base,
		listener_cb, 
		(void*)base, 
		LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, 
		-1, 
		(struct sockaddr*)&sin, 
		sizeof(sin)
		);

	if (!listener)
	{
		fprintf(stderr, "Could not create a listener!\n");
		return 1;
	}

	signal_event = evsignal_new(base, SIGINT, signal_cb, (void*)base);
	if (!signal_event || event_add(signal_event, NULL) < 0)
	{
		fprintf(stderr, "could not create/add a signal event!\n");
		return 1;
	}

	event_base_dispatch(base);

	evconnlistener_free(listener);
	event_free(signal_event);
	event_base_free(base);

	printf("done\n");

	return 0;
}

static void listener_cb(struct evconnlistener* linstener, evutil_socket_t fd, struct sockaddr* sa, int socklen, void* user_data)
{
	struct timeval tv;
	struct event_base* base = (event_base*)user_data;
	struct bufferevent* bev;

	bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
	if (!bev)
	{
		fprintf(stderr, "Error constructing bufferevent");
		event_base_loopbreak(base);
		return;
	}

	bufferevent_setcb(bev, conn_readcb, conn_writecb, conn_eventcb, NULL);

	tv.tv_sec = 30;
	tv.tv_usec = 0;
	bufferevent_set_timeouts(bev,&tv, NULL);
	bufferevent_setwatermark(bev, EV_READ, 10, 0);

	bufferevent_enable(bev, EV_READ | EV_WRITE);
}

static void conn_readcb(struct bufferevent* bev, void* user_data)
{
	struct evbuffer* input = bufferevent_get_output(bev);
	if (evbuffer_get_length(input) == 0)
	{
		bufferevent_free(bev);
	}
	else
	{
		bufferevent_write_buffer(bev, input);
	}
}

static void conn_writecb(struct bufferevent* bev, void* user_data)
{
	struct evbuffer* output = bufferevent_get_output(bev);
	if (evbuffer_get_length(output) == 0)
	{
		printf("flushed answer\n");
	}
}

static void conn_eventcb(struct bufferevent* bev, short events, void* user_data)
{
	if (events & BEV_EVENT_EOF)
	{
		printf("Connection closed\n");
	}
	else if (events & BEV_EVENT_ERROR)
	{
		printf("Got an error on the connection:%s\n", strerror(errno));
	}
	else if (events & (BEV_EVENT_READING | BEV_EVENT_TIMEOUT))
	{
		printf("read timeout:%s\n", strerror(errno));
	}

	//bufferevent_free(bev);

}

static void signal_cb(evutil_socket_t sig, short events, void* user_data)
{
	struct event_base* base = (event_base*)user_data;
	struct timeval delay = {2, 0};
	printf("Caught an interrupt signal; exitiong cleanly in two seconds.\n");

	event_base_loopexit(base, &delay);
}


