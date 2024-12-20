// SPDX-License-Identifier: MIT
//
// Copyright (c) 2022-2024 Gregory Machefer (Fewnity)
//
// This file is part of Xenity Engine

#include "network.h"

#if defined(__vita__) || defined(__PSP__)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>
#elif defined(__LINUX__) || defined(__PS3__)
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <unistd.h>
#elif defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#include <fcntl.h>
#include <cstring>

#if defined(__vita__)
#include <psp2/sysmodule.h>
#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#endif

#if defined(__PSP__)
#include <pspgu.h>
#include <pspnet.h>
#include <pspnet_apctl.h>
#include <pspnet_inet.h>
#include <pspnet_resolver.h>
#include <psputility.h>
#include <psputils.h>
pspUtilityNetconfData NetworkManager::s_pspNetworkData;
struct pspUtilityNetconfAdhoc adhocparam;
int NetworkManager::s_result = -1;
#endif

#include <engine/debug/debug.h>
#include <engine/engine_settings.h>
#include <engine/debug/stack_debug_object.h>
#include <engine/debug/performance.h>

bool NetworkManager::s_done = false;

std::vector<std::shared_ptr<Socket>> NetworkManager::s_sockets;
bool NetworkManager::s_needDrawMenu = false;

void NetworkManager::Init()
{
	STACK_DEBUG_OBJECT(STACK_HIGH_PRIORITY);
#if defined(__PSP__)
	sceUtilityLoadNetModule(PSP_NET_MODULE_COMMON);
	sceUtilityLoadNetModule(PSP_NET_MODULE_INET);

	sceNetInit(128 * 1024, 42, 4 * 1024, 42, 4 * 1024);
	sceNetInetInit();
	sceNetApctlInit(0x8000, 48);

	memset(&s_pspNetworkData, 0, sizeof(s_pspNetworkData));
	s_pspNetworkData.base.size = sizeof(s_pspNetworkData);
	s_pspNetworkData.base.language = PSP_SYSTEMPARAM_LANGUAGE_ENGLISH;
	s_pspNetworkData.base.buttonSwap = PSP_UTILITY_ACCEPT_CROSS;
	s_pspNetworkData.base.graphicsThread = 17;
	s_pspNetworkData.base.accessThread = 19;
	s_pspNetworkData.base.fontThread = 18;
	s_pspNetworkData.base.soundThread = 16;
	s_pspNetworkData.action = PSP_NETCONF_ACTION_CONNECTAP;

	memset(&adhocparam, 0, sizeof(adhocparam));
	s_pspNetworkData.adhocparam = &adhocparam;

	sceUtilityNetconfInitStart(&s_pspNetworkData);
#else
#if defined(_WIN32) || defined(_WIN64)
	WSADATA WSAData;
	int startupResult = WSAStartup(MAKEWORD(2, 0), &WSAData);
	if (startupResult != 0)
	{
		Debug::PrintError("[NetworkManager::CreateSocket] Could not start win socket");
	}
#endif

#if !defined(EDITOR)
	if (EngineSettings::values.useOnlineDebugger)
	{
		Debug::ConnectToOnlineConsole();
	}
#endif
#endif
}

std::shared_ptr<Socket> NetworkManager::GetClientSocket()
{
#if defined(_WIN32) || defined(_WIN64)
	SOCKADDR_IN sin;
	SOCKADDR_IN csin;
	int newSocketId = 1;
	int newClientSocketId = 1;
	newSocketId = static_cast<int>(socket(AF_INET, SOCK_STREAM, 0));
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(6004);
	bind(newSocketId, reinterpret_cast<SOCKADDR*>(& sin), sizeof(sin));
	listen(newSocketId, 0);
	while (true) /* Boucle infinie. Exercice : am�liorez ce code. */
	{
		int sinsize = sizeof(csin);
		if ((newClientSocketId = accept(newSocketId, reinterpret_cast<SOCKADDR*>(&csin), &sinsize)) != INVALID_SOCKET)
		{
			Debug::Print("New client connected");
			break;
			/*send(csock, "Hello world!\r\n", 14, 0);
			closesocket(csock);*/
		}
	}
	unsigned long nonblocking_long = false ? 0 : 1;
	ioctlsocket(newClientSocketId, FIONBIO, &nonblocking_long);

	std::shared_ptr<Socket> myNewSocket = std::make_shared<Socket>(newClientSocketId);
	s_sockets.push_back(myNewSocket);
	return myNewSocket;
#else
	return nullptr;
#endif
}

void NetworkManager::Update()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
	SCOPED_PROFILER("NetworkManager::Update", scopeBenchmark);

	const int socketCount = (int)s_sockets.size();
	for (int i = 0; i < socketCount; i++)
	{
		s_sockets[i]->Update();
	}
}

void Socket::Update()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
#if !defined(_EE)
	if (m_socketId < 0)
		return;

	char recvBuff[1024];
	int recvd_len;

	// Read a maximum of 1022 char in one loop
	while ((recvd_len = recv(m_socketId, recvBuff, 1023, 0)) > 0) // if recv returns 0, the socket has been closed. (Sometimes yes, sometimes not, lol)
	{
		recvBuff[recvd_len] = 0;
		m_incommingData += recvBuff;
	}
#endif
}

void Socket::Close()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
#if !defined(_EE)
#if defined(_WIN32) || defined(_WIN64)
	closesocket(m_socketId);
#elif defined(__PSP__) || defined(__vita__)
	close(m_socketId);
#endif
#endif
}

Socket::~Socket()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
	Close();
}

void Socket::SendData(const std::string& text)
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
	if (m_socketId < 0 || text.empty())
		return;

	const int infoLentgh = (int)text.size();
#if !defined(_EE)
	send(m_socketId, text.c_str(), infoLentgh, 0); // Send data to server
#endif
}

void NetworkManager::DrawNetworkSetupMenu()
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
	if (!s_done)
	{
#if defined(__PSP__)
		sceGuFinish();
		sceGuSync(0, 0);

		switch (sceUtilityNetconfGetStatus())
		{
		case PSP_UTILITY_DIALOG_NONE:
		{
			s_result = s_pspNetworkData.base.result;
			Debug::Print("Network setup: " + std::to_string(s_result), true);
			if (s_result == 0)
			{
				if (EngineSettings::values.useOnlineDebugger)
				{
					Debug::ConnectToOnlineConsole();
				}
			}
			s_done = true;
		}
		break;

		case PSP_UTILITY_DIALOG_VISIBLE:
			sceUtilityNetconfUpdate(1);
			break;

		case PSP_UTILITY_DIALOG_QUIT:
			sceUtilityNetconfShutdownStart();
			break;

		case PSP_UTILITY_DIALOG_FINISHED:
			break;

		default:
			break;
		}
#endif
	}
}

std::shared_ptr<Socket> NetworkManager::CreateSocket(const std::string& address, int port)
{
	STACK_DEBUG_OBJECT(STACK_MEDIUM_PRIORITY);
	int newSocketId = 1;
	#if !defined(__LINUX__)
	if (address.empty() || port <= 0)
	{
		Debug::PrintError("[NetworkManager::CreateSocket] Invalid address or port");
		return nullptr;
	}

#if !defined(_EE)
	struct sockaddr_in serv_addr;

	// memset(recvBuff, '0', sizeof(recvBuff));
//#if defined(_WIN32) || defined(_WIN64)
//	WSADATA WSAData;
//	int startupResult = WSAStartup(MAKEWORD(2, 0), &WSAData);
//	if (startupResult != 0)
//	{
//		Debug::PrintError("[NetworkManager::CreateSocket] Could not start win socket");
//		return nullptr;
//	}
//#endif
	if ((newSocketId = static_cast<int>(socket(AF_INET, SOCK_STREAM, 0))) < 0)
	{
		Debug::PrintError("[NetworkManager::CreateSocket] Could not create socket");
		return nullptr;
	}
	memset(&serv_addr, '0', sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if (inet_pton(AF_INET, address.c_str(), &serv_addr.sin_addr) <= 0)
	{
		Debug::PrintError("[NetworkManager::CreateSocket] inet_pton error occured");
		return nullptr;
	}
	if (connect(newSocketId, reinterpret_cast<struct sockaddr*>(&serv_addr), sizeof(serv_addr)) < 0)
	{
		Debug::PrintError("[NetworkManager::CreateSocket] Connect Failed");
		return nullptr;
	}
#if defined(_WIN32) || defined(_WIN64)
	unsigned long nonblocking_long = false ? 0 : 1;
	ioctlsocket(newSocketId, FIONBIO, &nonblocking_long);
#elif defined(__PS3__)

#else
	int i = 1;
	if (setsockopt(newSocketId, SOL_SOCKET, SO_NONBLOCK, (char*)&i, sizeof(i)) < 0)
	{
		Debug::PrintError("Failed to change socket flags");
		return nullptr;
	}
#endif
#endif
#endif
	std::shared_ptr<Socket> myNewSocket = std::make_shared<Socket>(newSocketId);
	s_sockets.push_back(myNewSocket);
	return myNewSocket;
}