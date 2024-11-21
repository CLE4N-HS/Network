#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <iostream>
#include <conio.h>
#include <stdio.h> 
#include <list>
#include <algorithm>
#include <thread>

#pragma comment(lib, "Ws2_32.lib")


#define DEFAULT_PORT 9666
#define DEFAULT_BUFLEN 1473
const char* IP = "192.168.10.170";//"127.0.0.1";

void Sending(SOCKET& Socket, std::string pseudo, std::string message)
{
	message = pseudo + std::string(" ") + message;

	if (send(Socket, message.c_str(), message.length(), 0) == SOCKET_ERROR)
	{
		std::cout << "send failed: " << WSAGetLastError() << std::endl;
	}
}

void Listener(SOCKET& Socket, std::list<std::pair<sockaddr_in, SOCKET>>& Info_Client, std::list< std::pair<std::thread, bool>>& Client_Thread)
{
	bool done = false;

	auto reciev_func = [&Socket, &Info_Client, &done, &Client_Thread]()
	{
		std::list<std::pair<sockaddr_in, SOCKET>>::iterator Actual_Client = --Info_Client.end();
		bool& running = Client_Thread.back().second;
		done = true;

		while (running)
		{
			int bytesRecv = SOCKET_ERROR;
			char recvbuf[2000];
			bytesRecv = recv(Actual_Client->second, recvbuf, 1999, 0);

			if (bytesRecv == SOCKET_ERROR)
			{
				int error = WSAGetLastError();
				if (error == 10054) // deconnection
				{
					std::cout << "Disconected : [" << inet_ntoa(Actual_Client->first.sin_addr) << "]:"
						<< htons(Actual_Client->first.sin_port)
						<< std::endl;

					running = false;
					Info_Client.erase(Actual_Client);
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

				std::cout << "Recieved from [" << recievedPseudo << "] : " << Buffer << std::endl;

				for (auto it = Info_Client.begin(); it != Info_Client.end(); it++)
				{
					if (it->second != Actual_Client->second && it->second != SOCKET_ERROR)
					{
						Sending(it->second, recievedPseudo, Buffer);
					}
				}
			}
		}

		running = false;
	};

	listen(Socket, 10);

	while (true /*program running (need bool)*/)
	{
		Info_Client.emplace_back();																					// reserve for next client
		std::cout << "Waiting for incoming connections..." << std::endl;

		Info_Client.back().second = SOCKET_ERROR;																	// set next socket to an error one

		while (Info_Client.back().second == SOCKET_ERROR && Info_Client.back().second == INVALID_SOCKET)			// while socket is not good
		{
			int Sizeof_ClientInfos = sizeof(Info_Client.back().first);
			Info_Client.back().second = accept(Socket, (sockaddr*)&Info_Client.back().first, &Sizeof_ClientInfos);	// try accept a connection
		}

		done = false;
		Client_Thread.push_back(std::make_pair<std::thread, bool>(std::thread(reciev_func), true));					// add a new thread for the client
		while (!done) { /*pause*/ }

		std::cout << "Connection accepted : "
			<< "[" << inet_ntoa(Info_Client.back().first.sin_addr) << "]:"
			<< htons(Info_Client.back().first.sin_port) << std::endl;
	}


	if (Info_Client.back().second == INVALID_SOCKET)
		std::cout << "accept failed with error code : " << WSAGetLastError() << std::endl;

	std::cout << "Listener Terminated" << std::endl;
}

int main(int argc, char** argv)
{
	sockaddr_in Info_Process;

	SOCKET Socket;
	std::list<std::pair<sockaddr_in, SOCKET>> Info_Client;
	std::list<std::pair<std::thread, bool>> Client_Thread;

	srand(time(0));
	// INIT WINSOCK ////////////////////////////////
	WSADATA data;
	int iresult = WSAStartup(MAKEWORD(2, 2), &data);
	if (iresult != NO_ERROR)
	{
		std::cout << L"WSAStartup function failed with error :" << iresult;
		return 1;
	}
	if (data.szDescription)
	{
		std::cout << int(LOBYTE(data.wVersion)) << '.' << int(HIBYTE(data.wVersion)) << std::endl;
	}

	///////////////////////////////////////////////
	Info_Process.sin_family = AF_INET;
	Info_Process.sin_port = htons(DEFAULT_PORT);
	Info_Process.sin_addr.S_un.S_addr = inet_addr(IP);
	//inet_pton(AF_INET, "127.0.0.1", &Info_Process.sin_addr.s_addr);
	///////////////////////////////////////////////

	// SOCKET ////////////////////////////////////

	if ((Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
	{
		std::cout << "Error at socket(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
		//getip
	}

	if (bind(Socket, (struct sockaddr*)&Info_Process, sizeof(Info_Process)) == SOCKET_ERROR)
	{
		std::cout << "Error at bind(): " << WSAGetLastError() << std::endl;
		WSACleanup();
		return 1;
	}

	std::cout << "Binded..." << std::endl;

	std::thread thread_Listen([&] { Listener(Socket, Info_Client, Client_Thread); });
	std::cout << "Ready..." << std::endl;

	bool running = true;
	while (running)
	{
		Client_Thread.remove_if([](std::pair<std::thread, bool>& pair)
			{
				if (pair.second == false)
				{
					if (pair.first.joinable())
						pair.first.join();
					return true;
				}

				return false;
			});
	}

	thread_Listen.join();
	std::for_each(std::begin(Client_Thread), std::end(Client_Thread), [](std::pair<std::thread, bool>& pair) {pair.first.join(); });

	WSACleanup();
	return 0;
}