#include <WINSOCK2.H>
#include <iostream>
#include <string>
#include <stdio.h>
#include <sstream>
#include <cstdlib>
using namespace std;
#pragma comment(lib,"ws2_32.lib")

DWORD WINAPI ReceiveThread(LPVOID lparam)
{

	int                 err, i, num;
	char                buf[100] = "";
	char                recvbuf[100] = "yes receive!!\0";
	SOCKET              ClientSocket = (SOCKET)(LPVOID)lparam;

	string token[3];

	// memset(recvbuf, 0, sizeof(recvbuf));
	memset(buf, 0, sizeof(buf));

	
	while (1){
		err = recv(ClientSocket, buf, sizeof(buf), 0);
		cout << "we receive something: " << buf << endl;
		
		string cmd(buf);
		istringstream stream(cmd);
		i = 0;
		while (stream >> token[i++]){
			if (i >= 3)
				break;
		}
		cout << "token[0]:"<<token[0]<<endl;
		if (token[0] == "Disconnect"){
			break;
		}
		memset(buf, 0, sizeof(buf));
	}
	//buf[err] = '\0';
	//printf("revc: %s\n", buf);
	//closesocket(ClientSocket);

	return 1;
}

void main()
{
	int err, i, port;
	WORD versionRequired;
	WSADATA wsaData;
	HANDLE hThread;
	DWORD dwThreadId;
	bool hasConn;
	string token[3];

	versionRequired = MAKEWORD(1, 1);
	err = WSAStartup(versionRequired, &wsaData);//协议库的版本信息
	if (!err)
	{
		printf("客户端嵌套字已经打开!\n");
	}
	else
	{
		printf("客户端的嵌套字打开失败!\n");
		return;//结束
	}
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	SOCKADDR_IN clientsock_in;
	


	//Sleep(20000);
	string tmpStr;
	char* cmd;
	hasConn = false;
	while (1){
		//cin >> tmpStr;
		getline(cin, tmpStr);
		istringstream stream(tmpStr);
		i = 0;
		while (stream >> token[i++]){
			if (i >= 3)
				break;
		}
		if (token[0] == "conn"){
			cout <<"we got into conn" << endl;
			cout << "token[1]: "<< token[1]<< endl;
			cout << "token[2]: " << token[2] << endl;
			//clientsock_in.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
			clientsock_in.sin_addr.S_un.S_addr = inet_addr(token[1].c_str());
			clientsock_in.sin_family = AF_INET;
			//clientsock_in.sin_port = htons(6000);
			port = atoi(token[2].c_str());
			cout << "port: "<<port<< endl;
			clientsock_in.sin_port = htons(port);
			
			connect(clientSocket, (SOCKADDR*)&clientsock_in, sizeof(SOCKADDR));//开始连接
			char receiveBuf[100];
			recv(clientSocket, receiveBuf, 101, 0);
			printf("%s\n", receiveBuf);

			hThread = CreateThread(NULL, NULL, ReceiveThread, (LPVOID)clientSocket, 0, &dwThreadId);
			if (hThread == NULL)
				printf("CreatThread AnswerThread() failed.\n");
			else
				printf("CreateThread OK.\n");
			hasConn = true;
		}
		else
		if (token[0] == "disconn"){
			cmd = const_cast<char*>(tmpStr.c_str());
			cout << "from client,before disconn:"<< cmd<< endl;
			send(clientSocket, cmd, tmpStr.length() + 1, 0);
			Sleep(3000);
			TerminateThread(hThread, dwThreadId);
			closesocket(clientSocket);


			hasConn = false;
		}
		else
		if (token[0] == "quit"){
			if (hasConn){
				cmd = const_cast<char*>(tmpStr.c_str());
				send(clientSocket, cmd, tmpStr.length() + 1, 0);
				TerminateThread(hThread, dwThreadId);
				closesocket(clientSocket);
				
				hasConn = false;
				break;
			}
			else
			{
				break;
			}

		}
		else
		{
			if (!hasConn){
				cout << "Please connect to the server first."<< endl;
			}
			else
			{
				cmd = const_cast<char*>(tmpStr.c_str());
				//send(clientSocket, "hello,this is client", strlen("hello,this is client") + 1, 0);
				send(clientSocket, cmd, tmpStr.length() + 1, 0);
			}
		}

		
		Sleep(1000);
		//recv(clientSocket, receiveBuf, 101, 0);
		//printf("%s\n", receiveBuf);
	}
	//Sleep(10000);
	//send(clientSocket, "hello,this is client", strlen("hello,this is client") + 1, 0);
	//closesocket(clientSocket);
	WSACleanup();
}