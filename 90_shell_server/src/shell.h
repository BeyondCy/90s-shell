/*
 * shell.h
 *
 *  Created on: Sep 11, 2013
 *      Author: after1990s
 */

#ifndef SHELL_H_
#define SHELL_H_
#include "alive_client.h"
#include "system.h"
void shell_init(void);

std::string shell_prompt(struct client  *c);//0 for no client

//process command, return string include the prompt
std::string shell_process_cmd(std::string cmd, struct client * pc/*in:the current client,out:the new client*/);
#endif /* SHELL_H_ */
