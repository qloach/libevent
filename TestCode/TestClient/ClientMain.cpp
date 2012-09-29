
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>

#include <iostream>
using namespace std;

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "libevent_core.lib")

bufferevent* MainBev;
bool bCanWrite;

static void eventcb(struct bufferevent *bev, short events, void *ptr)
{
    if (events & BEV_EVENT_CONNECTED)
	{
		//bufferevent_enable(bev, EV_READ|EV_WRITE);
		MainBev = bev;
		cout<<"event & bev_event_connected"<<endl;
		char HelloText[50];
		memset(HelloText, 0, 50);
		strcpy(HelloText, "hello server");
		int ret = bufferevent_write(bev, HelloText, strlen(HelloText));
		cout<<"write eventbuffer ret="<<ret<<endl;
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

static void readcb(struct bufferevent *bev, void *ptr)
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
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) 
	{
        cout<<"WSAStartup function failed with error:"<<iResult<<endl;
        return 1;
    }

    event_base *base;
    bufferevent *bev;
    sockaddr_in sin;

	MainBev = NULL;
	bCanWrite = false;

    base = event_base_new();

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr("127.0.0.1");
    sin.sin_port = htons(9999);

    bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(bev, readcb, writecb, eventcb, NULL);

	bufferevent_enable(bev, EV_READ|EV_WRITE);

    if (bufferevent_socket_connect(bev, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
        /* Error starting connection */
        bufferevent_free(bev);
        return -1;
    }

	
	event_base_loop(base, EVLOOP_NONBLOCK);
   // event_base_dispatch(base);

	char input[1024];
	memset(input, 0, 1024);
	while(true)
	{
		memset(input, 0, 1024);
		cin>>input;
		SendData(input, strlen(input));

		event_base_loop(base, EVLOOP_NONBLOCK);
	}
	

    return 0;
}




//
//#include <winsock2.h>
//#include <Ws2tcpip.h>
//
//#include <iostream>
//using namespace std;
//
//// Link with ws2_32.lib
//#pragma comment(lib, "Ws2_32.lib")
//
//#define DEFAULT_BUFLEN 1024
//#define DEFAULT_PORT 9995
//
//int main() {
//
//    //----------------------
//    // Declare and initialize variables.
//    int iResult;
//    WSADATA wsaData;
//
//    SOCKET ConnectSocket = INVALID_SOCKET;
//    struct sockaddr_in clientService; 
//
//    //----------------------
//    // Initialize Winsock
//    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
//    if (iResult != NO_ERROR) 
//	{
//        cout<<"WSAStartup failed with error:"<<iResult<<endl;
//        return 1;
//    }
//
//    //----------------------
//    // Create a SOCKET for connecting to server
//    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//    if (ConnectSocket == INVALID_SOCKET) 
//	{
//        cout<<"socket failed with error:"<<WSAGetLastError()<<endl;
//        WSACleanup();
//        return 1;
//    }
//
//    //----------------------
//    // The sockaddr_in structure specifies the address family,
//    // IP address, and port of the server to be connected to.
//    clientService.sin_family = AF_INET;
//    clientService.sin_addr.s_addr = inet_addr( "127.0.0.1" );
//    clientService.sin_port = htons( DEFAULT_PORT );
//
//    //----------------------
//    // Connect to server.
//    iResult = connect( ConnectSocket, (SOCKADDR*) &clientService, sizeof(clientService) );
//    if (iResult == SOCKET_ERROR) 
//	{
//        cout<<"connect failed with error:"<<WSAGetLastError()<<endl;
//        closesocket(ConnectSocket);
//        WSACleanup();
//        return 1;
//  }
//
//	while(true)
//	{
//		char SendBuff[1024];
//		memset(SendBuff, 0, 1024);
//		cin>>SendBuff;
//		iResult = send(ConnectSocket, SendBuff, strlen(SendBuff), 0);
//		cout<<"Send Result = "<<iResult<<endl;
//	}
//
//    //----------------------
//    // Send an initial buffer
//    //iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
//    //if (iResult == SOCKET_ERROR) {
//    //    cout<<"send failed with error:"<<WSAGetLastError()<<endl;
//    //    closesocket(ConnectSocket);
//    //    WSACleanup();
//    //    return 1;
//    //}
//
//   // printf("Bytes Sent: %d\n", iResult);
//
//    // shutdown the connection since no more data will be sent
//    //iResult = shutdown(ConnectSocket, SD_SEND);
//    //if (iResult == SOCKET_ERROR) {
//    //    wprintf(L"shutdown failed with error: %d\n", WSAGetLastError());
//    //    closesocket(ConnectSocket);
//    //    WSACleanup();
//    //    return 1;
//    //}
//
//    // Receive until the peer closes the connection
//    //do {
//
//    //    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
//    //    if ( iResult > 0 )
//    //        cout<<"Bytes received:"<<iResult<<endl;
//    //    else if ( iResult == 0 )
//    //        cout<<"Connection closed\n"<<endl;
//    //    else
//    //        cout<<"recv failed with error:"<<WSAGetLastError()<<endl;
//
//    //} while( iResult > 0 );
//
//
//    // close the socket
//    iResult = closesocket(ConnectSocket);
//    if (iResult == SOCKET_ERROR) {
//        cout<<"close failed with error:"<<WSAGetLastError()<<endl;
//        WSACleanup();
//        return 1;
//    }
//
//    WSACleanup();
//    return 0;
//}
//