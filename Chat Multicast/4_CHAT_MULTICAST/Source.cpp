
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS


#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <iostream>
#include <conio.h>
#include <stdio.h>
#include <time.h>

#pragma comment (lib,"ws2_32.lib")
//=============================================


void Affiche_Erreur()
{
	DWORD L_Code_Erreur = GetLastError();
	char L_Message[1024];
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, L_Code_Erreur, 0, L_Message, 1024, NULL);
	std::cout << "Erreur : " << L_Code_Erreur << " ==> " << L_Message << std::endl;
}

SOCKET Le_Socket;
sockaddr_in Info_Process_Broadcast;
sockaddr_in Info_Process_Local;
HANDLE H_Event_Reseau;
WSANETWORKEVENTS Raison_Event_Reseau;
std::string pseudo = "Default";
std::string key = "/key";
//-------------------



void Reception_Et_Affiche()
{
	char L_Buffer[2000];
	sockaddr_in L_Info_Distant;
	int L_Taille_Info = sizeof(L_Info_Distant);

	int L_Taille = recvfrom(Le_Socket, L_Buffer, sizeof(L_Buffer) - 1, 0,
		(sockaddr*)&L_Info_Distant, &L_Taille_Info);

	if (L_Taille >= 0)
	{
		std::string Buffer(L_Buffer);
		Buffer.resize(L_Taille);

		std::string recievedPseudo(Buffer.substr(0, Buffer.find(key)));
		Buffer.erase(0, recievedPseudo.size() + key.size());

		//std::string iefpjef(inet_ntoa(L_Info_Distant.sin_addr));
		//if (iefpjef != "192.168.10.148" && iefpjef != "192.168.10.147")
		//	return;

		std::cout << "MSG de "
			<< recievedPseudo
			<< " ["
			<< inet_ntoa(L_Info_Distant.sin_addr)
			<< ":" << htons(L_Info_Distant.sin_port)
			<< "] : "
			<< Buffer << std::endl;
	}
	else Affiche_Erreur();

}

// noe 192.168.10.147
// bryan 192.168.10.148

//-----------------------------------------------------
int main()
{
	std::cout << "Enter your pseudo :" << std::endl;
	std::getline(std::cin, pseudo);

	srand(time(0));
	WSADATA data;

	//INITIALIZE WINSOCK

	int result = WSAStartup(MAKEWORD(2, 2), &data);
	if (result != NO_ERROR)
	{
		std::cout << "WSAStartup function failed with error :" << result;
		return 1;
	}
	
	if (data.szDescription)
	{
		std::cout << int(LOBYTE(data.wVersion)) << '.' << int(HIBYTE(data.wVersion)) << std::endl;
	}

	H_Event_Reseau = WSACreateEvent();
	

	//

	Le_Socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (Le_Socket == INVALID_SOCKET)
	{
		Affiche_Erreur();
		WSACleanup();
		exit(0);
	}

	char broadcast = '1';
	if (setsockopt(Le_Socket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast)) < 0)
	{
		Affiche_Erreur();
		WSACleanup();
		exit(0);
	}

	//..............
	WSAEventSelect(Le_Socket, H_Event_Reseau, FD_READ);

	//..............
	Info_Process_Local.sin_family = AF_INET;
	Info_Process_Local.sin_port = htons(9999);
	Info_Process_Local.sin_addr.S_un.S_addr = ADDR_ANY;
	//..................
	Info_Process_Broadcast.sin_family = AF_INET;
	Info_Process_Broadcast.sin_port = htons(9999);
	Info_Process_Broadcast.sin_addr.S_un.S_addr = inet_addr("225.1.2.3");
	//..................
	int L_OK = bind(Le_Socket, (sockaddr*)&Info_Process_Local, sizeof(Info_Process_Local));
	if (SOCKET_ERROR == L_OK) {
		Affiche_Erreur();
		WSACleanup();
		exit(0);
	}
	//.........................

	ip_mreq L_Info_Multicast;
	L_Info_Multicast.imr_multiaddr.s_addr = inet_addr("225.1.2.3");
	L_Info_Multicast.imr_interface.s_addr = htonl(ADDR_ANY);

	L_OK = setsockopt(Le_Socket, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*)&L_Info_Multicast, sizeof(L_Info_Multicast));

	if (L_OK == SOCKET_ERROR) {
		std::cout << "ERREUR Inscription Groupe Multicast ! " << std::endl;
		WSACleanup();
		exit(0);
	}
	else
	{
		std::cout << "Success : Inscription Groupe Multicast ! " << std::endl;
	}

	char L_Message[1473];
	int L_Continuer = 1;

	while (L_Continuer)
	{
		if (_kbhit())
		{
			gets_s(L_Message, 1472);

			if (L_Message[0] == 'e' && L_Message[1] == 'x' && L_Message[2] == 'i' && L_Message[3] == 't' && L_Message[4] == '\0')
			{
				L_Continuer = 0;
			}

			std::string Str_Message(pseudo + key + L_Message);

			sendto(Le_Socket, Str_Message.c_str(), Str_Message.length(), 0, (sockaddr*)&Info_Process_Broadcast, sizeof(Info_Process_Broadcast));
		}
		else
		{
			DWORD L_Raison = WaitForSingleObject(H_Event_Reseau, 50);
			if (L_Raison != WAIT_TIMEOUT)
			{
				WSAEventSelect(Le_Socket, H_Event_Reseau, FD_READ);
				WSAEnumNetworkEvents(Le_Socket, H_Event_Reseau, &Raison_Event_Reseau);
			
				if ((Raison_Event_Reseau.lNetworkEvents & FD_READ) == FD_READ)
				{
					Reception_Et_Affiche();
				}
			}
		}
	}

	WSACleanup();
	return EXIT_SUCCESS;
}