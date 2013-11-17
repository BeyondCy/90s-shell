/*
 * alive_client.h
 *
 *  Created on: Sep 11, 2013
 *      Author: after1990s
 */

#ifndef ALIVE_CLIENT_H_
#define ALIVE_CLIENT_H_
#include <vector>
#include "system.h"
struct client{
		int socket;
		char *buff;
		int buff_used_len;//
		int buff_max_len;
		//connection info
		sockaddr_in addr;
		int pipe[2];//0 for shell read, 1 for client thread write
		bool active;//is the client active on shell
		char * ip;
		pthread_mutex_t buff_mutex;//used for read,write buff.
		int writepipe;
		int readpipe;
	};
class alive_client{
public:
	std::vector<struct client> client;

	struct client & get_client_by_socket(int socket) throw(int);
	struct client * get_client_by_index(int index)throw(int);
	void remove(int socket);
	/*return like "1. 192.168.1.1\n2.192.168.2.1"*/
	std::string  get_all_client(void);
	//void active(int socket);//set one client active on shell,DON'T clear buff.
	//void deactive(int socket);//set one client deactive on shell, clear buff, reset buff_len.
	//bool isactive(int socket);
	//int getreadpipe(int socket);
	//int getwritepipe(int socket);
	//char* get_buff(int socket);
	//int get_buff_used_len(int socket);
	//int set_buff_used_len(int socket, int used_len);
	//std::string& get_decimal_ip(int socket);

	bool  insert(struct client *c);
	void  clear(void);//delete all client, the server will shutdown.
};


#endif /* ALIVE_CLIENT_H_ */
