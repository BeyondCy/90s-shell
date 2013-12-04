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

/*
 * 初始化shell，记录目前操作的client。
 * 能够处理上下左右和退格键。
 * 能够处理ctr+C信号。
 * 命令：
 * ls 列出目前的客户端。
 * switch N 操作第N个客户端
 * exit 推出客户端或者程序
*/
class shell
{
public:
	shell();
	~shell();

	void readloop();
	//process command, save return str in result.
	void process(std::string  cmd);
private:
  std::string result;
  struct client *pc;
  std::string prompt();//显示的提示符


};
#endif /* SHELL_H_ */
