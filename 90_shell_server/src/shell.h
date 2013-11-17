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
//
std::string shell_prompt(struct client  *c);//0 for no client
//
////process command, return string include the prompt
std::string shell_process_cmd(std::string cmd, struct client * pc/*in:the current client,out:the new client*/);

/*
 * 初始化shell，记录目前操作的client。
 * 能够处理上下左右和退格键。
 * 能够处理ctr+C信号。
 * 命令：
 * ls 列出目前的客户端。
 * switch N 操作第N个客户端
 * exit 推出客户端或者程序
*/
/*class shell
{
public:
	shell();
	~shell();
	void process();
private:
  struct client *current;
  WINDOW *wnd;
  std::string prompt;//显示的提示符
};*/
#endif /* SHELL_H_ */
