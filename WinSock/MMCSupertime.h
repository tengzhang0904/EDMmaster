/*****************************************************************
*header file for Master Mind Control Message to Supertime on MMC
*Winsock Client Source Code
*Function call syntax:  MMCSupertime(string message) 
*IP addr "128.118.29.8" for EDM Supertime computer
*localhost "127.0.0.1"
*Last modified by KZ 10FEB2012
******************************************************************/

#ifndef _MMCSupertime_included_
#define _MMCSupertime_included_
#define _CRT_SECURE_NO_DEPRECATE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <cstdlib>
using namespace std;

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define SupertimeIPaddr "128.118.29.8" //Supertime computer IP address
#define DEFAULT_PORT "8888" //Port number, see settings in "Supertime Master.vi"
#define DEFAULT_BUFLEN 512   //Max size of a message in bytes

int MMCSupertime(string message) 
{
    string header(60, '-');

    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;

	string isBlank="";
    char *sendbuf ;
    char recvbuf[DEFAULT_BUFLEN];
    int iResult;
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(SupertimeIPaddr, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

	cout<<header<<endl; 
	cout<<"Connected to "<<SupertimeIPaddr<<endl; 
	cout<<"Message to send: "<<message<<endl; 
	if (message==isBlank)  {
			  message="Blank Message, do nothing!";
	}
	sendbuf = (char *)message.c_str( );

    // Send an buffer
    iResult = send( ConnectSocket, sendbuf, (int)strlen(sendbuf), 0 );
    if (iResult == SOCKET_ERROR) {
                printf("send failed with error: %d\n", WSAGetLastError());
                closesocket(ConnectSocket);
                WSACleanup();
    }
    printf("Bytes Sent: %ld  waiting for Supertime to finish...\n", iResult);

	iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
	string message_recv(recvbuf); 
	message_recv.erase( iResult);//empty blanks

    if ( iResult > 0 ) 
		     cout<<"Message received from Supertime (sequence executed): "<<endl<<message_recv<<endl;
    else if ( iResult == 0 )
             printf("Connection closed\n");
    else
             printf("recv failed with error: %d\n", WSAGetLastError());


    // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
	printf("Connection closed\n");

    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;

}


#endif




