/*
 * server.h
 *
 *  Created on: Sep 11, 2013
 *      Author: after1990s
 */

#ifndef SERVER_H_
#define SERVER_H_

#include "alive_client.h"
#include "system.h"

void *init_server(void*);
void *process_client(void*);
#endif /* SERVER_H_ */
