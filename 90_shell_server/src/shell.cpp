/*
 * shell.cpp
 *
 *  Created on: Sep 11, 2013
 *      Author: after1990s
 */
#include "shell.h"
extern alive_client g_client;
extern int g_slisten;
extern shell  *g_pshell;

void process_ctrl_c(int signo)
{
	printf("use exit command exit shell\n");
	return;
}
shell::shell()
{
	pc = (struct client*)malloc(sizeof(struct client));
	memset(pc, 0, sizeof(struct client));
	pc->socket = 0;
	signal(SIGINT,process_ctrl_c);
}
void shell::readloop()
{
	std::cout << "welcome 90s shell server" << std::endl;
	std::cout << prompt();
	while (true)
	{
		char *cmd;
		cmd = readline(prompt().c_str());
		process(cmd);
		std::cout << result;
		free(cmd);
	}
}
void shell::process(std::string cmd)
{
	//the pc is different frome the one in vector.when i found the switch one, read the buff, copy others to pc,
	//attation, when change content that pointer assigned in pc, the vector content will change too.
	result.clear();
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
			return;
		}
		cmd += "\r\n";
		if (send(pc->socket, cmd.c_str(), cmd.length(), NULL)!=cmd.length())
		{
			//client crash ,del it and return shell
			g_client.remove(pc->socket);
			memset(pc, 0, sizeof(struct client));
			result = prompt();
		}
		return;
	}
	if (cmd.compare("ls")==0)
	{
		result += g_client.get_all_client();
		result += "\n";
		result += prompt();
		return;
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
			return;
		}
		return;
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
		result += prompt();
	}
	else
	{
		result = "Invalid command, type \"help\" for help.\n";
		result += prompt();
	}
	return;
}
std::string shell::prompt()//0 for no client
{
	if (pc->socket==0)
		return ">";
	if (pc->active==true)
		return "";
	std::string result = pc->ip;
	result += ">";
	return result;
}
shell::~shell()
{
	free (pc);
}


