#ifndef _RAPTILE_CONNECTION_H_
#define _RAPTILE_CONNECTION_H_
#include "WINSOCK2.H"

#define HTTPPORT 80

/*
return value:
0 for success, -1 for fail
*/
int InitNetEnvironment();
void CleanUpNetEnvironment();
SOCKET ConnectByURL(const char *domain, int port);

class Connection
{

};

#endif