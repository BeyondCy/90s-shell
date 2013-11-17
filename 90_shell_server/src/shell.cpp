/*
 * shell.cpp
 *
 *  Created on: Sep 11, 2013
 *      Author: after1990s
 */
#include "shell.h"
extern alive_client g_client;
extern int g_slisten;
void shell_init(void)
{
	struct client *pc = (struct client*)malloc(sizeof(struct client));
	memset(pc, 0, sizeof(struct client));
	pc->socket = 0;
	std::cout << "welcome 90s shell server" << std::endl;
	std::cout << shell_prompt(pc);

	while (true)
	{
		char *cmd;
		cmd = readline(shell_prompt(pc).c_str());
		std::cout << shell_process_cmd(cmd, pc);
		free(cmd);
	}
}
std::string shell_process_cmd(std::string cmd, struct client * pc)
{
	//the pc is different frome the one in vector.when i found the switch one, read the buff, copy others to pc,
	//attation, when change content that pointer assigned in pc, the vector content will change too.
	std::string result = "";
	if (cmd.length()!=0)
	{
		add_history(cmd.c_str());
	}
	//when a client active
	if (pc->socket!=0 &&pc->active ==true)
	{
		if (cmd.compare("exit")==0){
			pc->active = false;
			memset(pc, 0, sizeof(struct client));
			result += shell_prompt(pc);
			return result;
		}
		cmd += "\r\n";
		if (send(pc->socket, cmd.c_str(), cmd.length(), NULL)!=cmd.length())
		{
			//client crash ,del it and return shell
			g_client.remove(pc->socket);
			memset(pc, 0, sizeof(struct client));
			result = shell_prompt(pc);
		}
		return result;
	}
	if (cmd.compare("ls")==0)
	{
		result += g_client.get_all_client();
		result += "\n";
		result += shell_prompt(pc);
		return result;
	}
	else if (cmd.find("switch")==0)
	{

		int index = atoi(cmd.substr(7, cmd.length()-7).c_str());
		try{
			struct client *c = g_client.get_client_by_index(index);

			result += "switch client, the client received message:\n";
			pthread_mutex_lock(&c->buff_mutex);
			c->active = true;
			result += c->buff;
			pthread_mutex_unlock(&c->buff_mutex);

			//result += "\n";
			//result += shell_prompt(&c);
			memmove(pc, c, sizeof(struct client));

		}
		catch(int e)
		{
			result += "no such client";
			return result;
		}
		return result;
	}
	else if (cmd.compare("exit")==0)
	{
		close(g_slisten);
		g_client.clear();
		exit(0);
	}
	else if (cmd.compare("help")==0)
	{
		result =
"ls -- list all client\n\
switch N -- switch to number N client shell\n\
exit -- exit current shell\n";
		result += shell_prompt(pc);
	}
	else
	{
		result = "Invalid command, type \"help\" for help.\n";
		result += shell_prompt(pc);
	}
	return result;
}
std::string shell_prompt(struct client  *c)//0 for no client
{
	if (c->socket==0)
		return ">";
	if (c->active==true)
		return "";
	std::string result = c->ip;
	result += ">";
	return result;

}

