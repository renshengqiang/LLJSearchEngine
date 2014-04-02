#include "Connection.h"

int InitNetEnvironment()
{
	int err;
    WORD versionRequired;
    WSADATA wsaData;

    versionRequired = MAKEWORD(1,1);
    err = WSAStartup(versionRequired,&wsaData); //协议库的版本信息
	if(err != 0)
	{
		return -1;
	}
	return 0;
}

void CleanUpNetEnvironment()
{
	WSACleanup();
}

SOCKET ConnectByURL(const char *domain, int port)
{
	struct hostent *host = gethostbyname(domain);
	if(host == NULL)
	{
		return INVALID_SOCKET;
	}

	SOCKET clientSocket = socket(AF_INET,SOCK_STREAM,0);
	if(clientSocket == INVALID_SOCKET)
	{
		return INVALID_SOCKET;
	}

	struct sockaddr_in serveraddr;

	memset(&serveraddr,0,sizeof(struct sockaddr_in));
    memcpy(&serveraddr.sin_addr,host->h_addr_list[0],host->h_length);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
	return (connect(clientSocket,(struct sockaddr *)&serveraddr,sizeof(struct sockaddr)) == SOCKET_ERROR) ? INVALID_SOCKET : clientSocket;
}

/*
SOCKET ConnectByURL(char *domain, int port)
{
	struct hostent *host = gethostbyname(domain);
	if(host == NULL)
	{
		cout << "gethostbyname error\n";
		return INVALID_SOCKET;
	}
	cout << host->h_name << endl;
	cout << inet_ntoa(*((struct in_addr *)host->h_addr));

	SOCKET clientSocket = socket(AF_INET,SOCK_STREAM,0);
	if(clientSocket == INVALID_SOCKET)
	{
		cout << "create socket error\n";
		return INVALID_SOCKET;
	}

	struct sockaddr_in serveraddr;

	memset(&serveraddr,0,sizeof(struct sockaddr_in));
    memcpy(&serveraddr.sin_addr,host->h_addr_list[0],host->h_length);
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
	return (connect(clientSocket,(struct sockaddr *)&serveraddr,sizeof(struct sockaddr)) == SOCKET_ERROR) ? INVALID_SOCKET : clientSocket;
}
*/