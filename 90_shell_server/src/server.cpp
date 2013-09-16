/*
 * server.cpp
 *
 *  Created on: Sep 11, 2013
 *      Author: after1990s
 */
#include "server.h"
#include "alive_client.h"
extern alive_client g_client;
int g_slisten;
void* init_server(void * unuse)
{
	g_slisten = socket(AF_INET, SOCK_STREAM, NULL);
	struct sockaddr_in addr;
	int addr_len = sizeof(addr);
	memset(&addr, 0,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(PORT);
	while (bind (g_slisten, (struct sockaddr*)&addr, addr_len)!= -1)
		sleep(1);
	listen(g_slisten, 10);
	pthread_t t;
	while (true)
	{
		struct sockaddr_in accept_addr;
		socklen_t accept_len = sizeof(struct sockaddr_in);
		memset(&accept_addr, 0, sizeof(accept_addr));
		int s_accept = accept(g_slisten,(struct sockaddr*) &accept_addr, &accept_len);
		if (s_accept==-1)
			continue;
		struct client *c = (struct client*)malloc(sizeof(struct client));
		memcpy((void*)&c->addr,(const void *)&accept_addr, sizeof(c->addr));;
		c->socket = s_accept;
		g_client.insert(c);

		/*after insert, the c address is different from vector client*/
		int *p = (int*)malloc(sizeof(int));
		*p = s_accept;
		pthread_create(&t, NULL, process_client,p);
	}
	return NULL;
}
void * process_client(void *p)
{//todo:the p prama need free?;
	struct client *c = &g_client.get_client_by_socket(*(int*)p);
	char buff[1024*1024] = {0};
	int bufflen = 1024*1024;
	int recvlen = 0;
	while(true)
	{
		recvlen = recv(c->socket, buff, bufflen, NULL);
		if (recvlen==0){
			g_client.remove(c->socket);
			break;
		}
		if (recvlen==-1)
			continue;
		pthread_mutex_lock(&c->buff_mutex);
		if (!c->active)
		{//write to buff
			memcpy(c->buff+c->buff_used_len,
					buff,
					recvlen);
			c->buff_used_len += recvlen;
			pthread_mutex_unlock(&c->buff_mutex);
		}
		else
		{
			pthread_mutex_unlock(&c->buff_mutex);
			std::cout << buff;
			std::cout.flush();
		}
		memset(buff, 0, bufflen);
	}
	return NULL;

}


