/*
 * alive_client.cpp
 *
 *  Created on: Sep 11, 2013
 *      Author: after1990s
 */

#include "alive_client.h"


bool alive_client::insert(struct client *c)
{/*IN: only c.socket, c.addr is set*/
	c->active = false;
	c->buff = (char*)malloc(1024*1024);
	c->buff_max_len = 1024*1024;
	memset (c->buff, 0, c->buff_max_len);
	pthread_mutex_init(&c->buff_mutex, NULL);
	c->buff_used_len = 0;
	c->ip = inet_ntoa(c->addr.sin_addr);

	pipe(c->pipe);
	c->readpipe = c->pipe[0];
	c->writepipe = c->pipe[1];
	client.push_back(*c);
	return true;
}
/*return like "1. 192.168.1.1\n2. 192.168.2.1"*/
std::string alive_client::get_all_client(void)
{
	std::string result="";
	int i=1;
	char buf[2]={0};
	if (client.size()==0)
	{
		result += "no client\n";
		return result;
	}
	for (std::vector<struct client>::iterator itr = client.begin(); itr!=client.end(); ++itr)
	{

		snprintf(buf, 2, "%d",i++);
		result += buf;
		result += ". ";
		result += itr->ip;
		result += "\n";
		memset (buf, 0, 2);
	}
	return result;
}
struct client * alive_client::get_client_by_index(int index) throw (int)
{
	if (client.size() > index-1)
		return &client[index-1];
	throw -1;
}
struct client & alive_client::get_client_by_socket(int socket) throw(int)
{
	std::vector<struct client>::iterator itr = client.begin();
	for (; itr!=client.end(); ++itr)
	{
		if (itr->socket == socket)
			return *itr;
	}
	throw -1;
}
void  alive_client::clear(void)
{//delete all client, the server will shutdown.
	for (std::vector<struct client>::iterator itr = client.begin(); itr!=client.end(); ++itr)
	{
		free(itr->buff);
		close(itr->pipe[0]);
		close(itr->pipe[1]);
		pthread_mutex_destroy(&itr->buff_mutex);

	}
	client.clear();

}
void alive_client::remove(int socket)
{//when remove ,assume mutex is not in use
	for (std::vector<struct client>::iterator itr = client.begin(); itr!=client.end(); ++itr)
	{
		if (itr->socket == socket)
		{
			free(itr->buff);
			close(itr->pipe[0]);
			close(itr->pipe[1]);
			pthread_mutex_destroy(&itr->buff_mutex);
			client.erase(itr);
			break;
		}
	}
}
/*
struct client alive_client::get(int socket)
{
	for (std::vector<struct client>::iterator itr = client.begin(); itr!=client.end(); ++itr)
	{
		if (itr->socket == socket)
			return *itr;
	}
	return NULL;
}

void alive_client::active(int socket)//set one client active on shell,DON'T clear buff.
{
	for (std::vector<struct client>::iterator itr = client.begin(); itr!=client.end(); ++itr)
	{
		if (itr->socket == socket){
			itr->active = true;
			break;
		}
	}
}
void alive_client::deactive(int socket)//set one client deactive on shell, clear buff, reset buff_len.
{
	for (std::vector<struct client>::iterator itr = client.begin(); itr!=client.end(); ++itr)
	{
		if (itr->socket == socket){
			itr->active = false;
			break;
		}
	}
}
bool alive_client::isactive(int socket)
{
	for (std::vector<struct client>::iterator itr = client.begin(); itr!=client.end(); ++itr)
	{
		if (itr->socket == socket){
			return itr->active;
			break;
		}
	}
	return false;
}
int alive_client::getreadpipe(int socket)
{
	for (std::vector<struct client>::iterator itr = client.begin(); itr!=client.end(); ++itr)
	{
		if (itr->socket == socket){
			return itr->pipe[0];
			break;
		}
	}
	return -1;
}
int alive_client::getwritepipe(int socket)
{
	for (std::vector<struct client>::iterator itr = client.begin(); itr!=client.end(); ++itr)
	{
		if (itr->socket == socket){
			return itr->pipe[1];
			break;
		}
	}
	return -1;
}
char* alive_client::get_buff(int socket)
{
	std::string s;
	for (std::vector<struct client>::iterator itr = client.begin(); itr!=client.end(); ++itr)
	{
		if (itr->socket == socket){
			return itr->buff;
			break;
		}
	}
	return NULL;
}
int alive_client::get_buff_used_len(int socket)
{
	std::string s;
	for (std::vector<struct client>::iterator itr = client.begin(); itr!=client.end(); ++itr)
	{
		if (itr->socket == socket){
			return itr->buff_used_len;
			break;
		}
	}
	return NULL;
}
int alive_client::set_buff_used_len(int socket, int used_len);
std::string& alive_client::get_decimal_ip(int socket);
/*return like "1. 192.168.1.1\n2.192.168.2.1"*/

