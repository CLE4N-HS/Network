#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <time.h>
#include <thread>
#include <string>
#pragma comment(lib, "Ws2_32.lib")

int main(int argc, char** argv)
{
	const u_short PORT = 21;
	const char* IP = "192.168.10.147";//"192.168.75.1";//"127.0.0.1";
	const int BUFFERLENGTH = 1473;

	sockaddr_in Info_Process;
	std::string pseudo = "Default";
	SOCKET Socket;

	std::cout << "Enter your pseudo :" << std::endl;
	std::cin >> pseudo;

	// INIT WINSOCK ////////////////////////////////
	WSADATA data;
	int iresult = WSAStartup(MAKEWORD(2, 2), &data);
	if (iresult != NO_ERROR)
	{
		std::cout << "WSAStartup function failed with error :" << iresult;
		return 1;
	}
	if (data.szDescription)
	{
		std::cout << int(LOBYTE(data.wVersion)) << '.' << int(HIBYTE(data.wVersion)) << std::endl;
	}

	// SETTING UP SOCK_ADRR ////////////////////////////////
	Info_Process.sin_family = AF_INET;
	Info_Process.sin_port = htons(PORT);
	Info_Process.sin_addr.S_un.S_addr = inet_addr(IP);
	////////////////////////////////////////////////////////

	// CREATE SOCKET ////////////////////////////////////
	if ((Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}
	/////////////////////////////////////////////////////

	// CONNECT ///////////////////////////////////
	if (connect(Socket, (sockaddr*)&Info_Process, sizeof(Info_Process)) == SOCKET_ERROR)
	{
		std::cout << "Error at connect(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}
	
	// RECEIVE //////////////////////////////
	std::thread thread_Recieve([&]
		{
			bool running = true;

			while (running)
			{
				char recvbuf[200];
				int bytesRecv = recv(Socket, recvbuf, 199, 0);

				if (bytesRecv == SOCKET_ERROR)
				{
					int error = WSAGetLastError();
					if (error == 10054) // deconnection
					{
						running = false;
						std::cout << "Lost Connection with Server" << std::endl;
					}
					else
						printf("Server: recv() error %ld.\n", error);

				}
				else
				{
					recvbuf[bytesRecv] = 0;

					std::string Buffer(recvbuf);

					std::string recievedPseudo(Buffer.substr(0, Buffer.find(" ")));
					Buffer.erase(0, recievedPseudo.size() + 1);

					std::cout << "["
						<< recievedPseudo
						<< "] : "
						<< Buffer << std::endl;
				}
			}
		});

	bool L_Continuer = true;

	//char L_Message[BUFFERLENGTH];
	std::string S_Message;

	while (L_Continuer)
	{
		// SEND ////////////////////////////////
		if (_kbhit())
		{
			int iSended = 0;
			
			getline(std::cin, S_Message);

			if (S_Message[0] == 'q')
			{
				L_Continuer = false;
			}

			if (L_Continuer)
			{
				std::string toSend = pseudo + " " + S_Message;

				if ((iSended = send(Socket, toSend.c_str(), toSend.length(), 0)) == SOCKET_ERROR)
				{
					std::cout << "send failed: " << WSAGetLastError() << std::endl;
					L_Continuer = false;
				}
			}

			if (L_Continuer)
			{
				std::cout << "You -> " << S_Message << std::endl;
			}
		}

	}

	thread_Recieve.join();
	WSACleanup();
	return 0;
}