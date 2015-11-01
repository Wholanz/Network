#include <WINSOCK2.H>
#include <stdio.h>
#include <iostream>
#include <map>
#include <string>
#include <sstream>
#include <cstdlib>
using namespace std;
#pragma comment(lib,"ws2_32.lib")

map<int, bool> flag;
map<int, string> buffer;
map<int, SOCKET> clientList;
int total;

DWORD WINAPI AnswerThread(LPVOID lparam)
{

	int                 err, i, num;
	char                buf[100] = "";
	char                recvbuf[100] = "yes receive!!\0";
	SOCKET              ClientSocket = (SOCKET)(LPVOID)lparam;
	
	string token[3];

	// memset(recvbuf, 0, sizeof(recvbuf));
	memset(buf, 0, sizeof(buf));

	//send(ClientSocket, recvbuf, strlen(recvbuf), 0);
	send(ClientSocket, recvbuf, 100, 0);
	while (1){
		err = recv(ClientSocket, buf, sizeof(buf), 0);
		cout << "we get something: " <<buf << endl;
		string cmd(buf);
		istringstream stream(cmd);
		i = 0;
		while (stream >> token[i++]){
			if (i >= 3)
				break;
		}
		cout <<"token[1]: "<< token[1] << endl;
		cout << "token[2]: " << token[2] << endl;
		if (token[0] == "time"){
			SYSTEMTIME st = { 0 };
			GetLocalTime(&st);
			printf("%d-%02d-%02d %02d:%02d:%02d\n",
				st.wYear,
				st.wMonth,
				st.wDay,
				st.wHour,
				st.wMinute,
				st.wSecond);
			string stime;
			int k;
			string tmpStr;
			char portCh[6];
			k = st.wYear;
			tmpStr = _itoa(k, portCh, 10);
			stime = tmpStr+"/";

			k = st.wMonth;
			tmpStr = _itoa(k, portCh, 10);
			stime = stime + tmpStr + "/";

			k = st.wDay;
			tmpStr = _itoa(k, portCh, 10);
			stime = stime + tmpStr + "  ";

			k = st.wHour;
			tmpStr = _itoa(k, portCh, 10);
			stime = stime + tmpStr + ":";

			k = st.wMinute;
			tmpStr = _itoa(k, portCh, 10);
			stime = stime + tmpStr + ":";

			k = st.wSecond;
			tmpStr = _itoa(k, portCh, 10);
			stime = stime + tmpStr;

			char* message = const_cast<char*>(stime.c_str());
			send(ClientSocket, message, stime.length() + 1, 0);
		}
		else
		if (token[0] == "name"){
			char message[256];
			gethostname(message, 256);
			string hostname(message);
			send(ClientSocket, message, hostname.length() + 1, 0);
		}
		else
		if (token[0] == "list"){
			map<int, SOCKET>::iterator it;
			string tmpStr;
			SOCKADDR_IN m_address;
			int nAddrLen;
			for (it = clientList.begin(); it != clientList.end(); ++it){
				cout << "key: " << it->first << " value: " << it->second << endl;
				memset(&m_address, 0, sizeof(m_address));
				nAddrLen = sizeof(m_address);
				getpeername(it->second, (SOCKADDR*)&m_address, &nAddrLen);
				string ipaddr(inet_ntoa(m_address.sin_addr));
				cout << ipaddr<< endl;
				int j = ntohs(m_address.sin_port);
				cout << "port: "<< j << endl;
				char portCh[6];
				string portStr(_itoa(j, portCh, 10));
				string idStr(_itoa(it->first, portCh, 10));
				tmpStr = idStr + "  " + ipaddr + "  " + portStr;
				//string port = "  " + portStr;
				//cout << port << endl;
				//tmpStr = it->first + "  " + ipaddr + port;
				cout << "message: "<< tmpStr<< endl;
				char* message = const_cast<char*>(tmpStr.c_str());
				send(ClientSocket, message, tmpStr.length()+1, 0);
			}
			
		}
		else
		if (token[0] == "send"){
			cout << "we got a send order:"<<token[1]<<"  "<< token[2] << endl;
			num = atoi(token[1].c_str());
			cout << "num: " << num << endl;
			SOCKET& target = clientList[num];
			char* message = const_cast<char*>(token[2].c_str());
			send(target, message, token[2].length(), 0);
			//char* prompts = "Send Complete.\0";
			//send(ClientSocket, prompts, 15, 0);
		}
		else
		if (token[0] == "disconn" || token[0] == "quit"){
			cout << "we got into disconn"<< endl;
			SOCKADDR_IN t_address;
			int tAddrLen;
			memset(&t_address, 0, sizeof(t_address));
			tAddrLen = sizeof(t_address);
			getpeername(ClientSocket, (SOCKADDR*)&t_address, &tAddrLen);
			string tIP(inet_ntoa(t_address.sin_addr));
			int tPortNum = ntohs(t_address.sin_port);

			map<int, SOCKET>::iterator it;
			string tmpStr;
			SOCKADDR_IN m_address;
			int nAddrLen;
			for (it = clientList.begin(); it != clientList.end(); ++it){
				memset(&m_address, 0, sizeof(m_address));
				nAddrLen = sizeof(m_address);
				getpeername(it->second, (SOCKADDR*)&m_address, &nAddrLen);
				string ipaddr(inet_ntoa(m_address.sin_addr));
				cout << ipaddr << endl;
				int portNum = ntohs(m_address.sin_port);
				
				if (ipaddr == tIP && portNum == tPortNum){
					cout << "we find an entry"<< endl;
					clientList.erase(it);
					char* message = "Disconnect successfully!";
					send(ClientSocket, message, 25, 0);
					break;
				}
				
			}
			break;

		}
		else
		{

		}
		
	}
	buf[err] = '\0';
	printf("revc: %s\n", buf);
	closesocket(ClientSocket);
	return 1;
}

void main()
{
	//创建套接字
	WORD myVersionRequest;
	WSADATA wsaData;
	HANDLE hThread;
	DWORD dwThreadId;

	myVersionRequest = MAKEWORD(1, 1);
	int err;
	err = WSAStartup(myVersionRequest, &wsaData);
	if (!err)
	{
		printf("已打开套接字\n");
	}
	else
	{
		//进一步绑定套接字
		printf("嵌套字未打开!");
		return;
	}
	SOCKET serSocket = socket(AF_INET, SOCK_STREAM, 0);//创建了可识别套接字
	//需要绑定的参数
	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);//ip地址
	//addr.sin_addr.S_un.S_addr = htonl("127.0.0.1");//ip地址
	addr.sin_port = htons(6000);//绑定端口

	bind(serSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR));//绑定完成
	listen(serSocket, 5);//其中第二个参数代表能够接收的最多的连接数

	//////////////////////////////////////////////////////////////////////////
	//开始进行监听
	//////////////////////////////////////////////////////////////////////////
	SOCKADDR_IN clientsocket;
	int len = sizeof(SOCKADDR);

	
	total = 0;
	while (1)
	{
		SOCKET serConn = accept(serSocket, (SOCKADDR*)&clientsocket, &len);//如果这里不是accept而是conection的话。。就会不断的监听
		total++;
		clientList[total] = serConn;
		flag[total] = false;
		char sendBuf[100];
		hThread = CreateThread(NULL, NULL, AnswerThread, (LPVOID)serConn, 0, &dwThreadId);
		if (hThread == NULL)
			printf("CreatThread AnswerThread() failed.\n");
		else
			printf("CreateThread OK.\n");
		/*
		sprintf(sendBuf, "welcome %s to bejing", inet_ntoa(clientsocket.sin_addr));//找对对应的IP并且将这行字打印到那里
		send(serConn, sendBuf, strlen(sendBuf) + 1, 0);
		char receiveBuf[100];//接收
		recv(serConn, receiveBuf, strlen(receiveBuf) + 1, 0);
		printf("%s\n", receiveBuf);
		closesocket(serConn);//关闭
		*/
	}
	WSACleanup();//释放资源的操作
}