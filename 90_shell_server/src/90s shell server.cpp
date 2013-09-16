//============================================================================
// Name        : 90s.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
#include "alive_client.h"
#include "server.h"
#include "pthread.h"
#include "shell.h"
#include <unistd.h>
#include <iostream>
using namespace std;
alive_client g_client;
int main() {
	pthread_t t;
	pthread_create(&t,NULL, init_server, NULL);
	shell_init();
	return 0;
}
