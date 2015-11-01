#include "Server.h"
using namespace std;

void RunGet(SOCKET tSoc, string s){
	send(tSoc, S_Get, PDU_LEN, 0);
	ifstream fin(s, ios::binary);
	char buffer[PDU_LEN];
	int num = PDU_LEN;
	//send file with PDU_LEN bit string 
	while (true){
		fin.read(buffer, PDU_LEN);
		num = (int)fin.gcount();
		send(tSoc, (char *)&num, sizeof(int), 0);
		send(tSoc, buffer, num, 0);
		//string < PDU_LEN only appear in the end
		if (num < PDU_LEN) break;
	}
}
void RunPut(SOCKET tSoc, string s){
	send(tSoc, S_Put, PDU_LEN, 0);
	ofstream fout(s, ios::binary);
	int num=0,sum,i;
	char buffer[PDU_LEN];
	//receive size and data
	while (true){
		recv(tSoc, (char *)&num, sizeof(num), 0);
		for(sum=0;sum<num;sum+=i)
			{
			i= recv(tSoc, buffer+sum, num-sum, 0);
			if (i ==SOCKET_ERROR)
				{
				cout<<"recv failed!\n";
				system("pause");
				return ;
				}
			}
		//write to file
		fout.write(buffer, num);
		if (num < PDU_LEN) break;
	}
	fout.close();
}

void RunCd(SOCKET tSoc,string s){
	send(tSoc, S_Cd, PDU_LEN, 0);
	SetCurrentDirectory(s.c_str());
}

void RunLs(SOCKET tSoc){
	send(tSoc, S_Ls, PDU_LEN, 0);
	//write list to file tmp
	system("dir > tmp"); 
	//send file tmp to client
	ifstream fin("tmp", ios::binary);
	char buffer[PDU_LEN];
	int num = PDU_LEN;
	while (true){
		fin.read(buffer, PDU_LEN);
		num = (int)fin.gcount();
		send(tSoc, (char *)&num, sizeof(int), 0);
		send(tSoc, buffer, num, 0);
		if (num < PDU_LEN){
			break;
		}
	}
	//recevory
	fin.close();
	system("del tmp");
}
void RunPwd(SOCKET tSoc){
	send(tSoc, S_Pwd, PDU_LEN, 0);
	char buf[PDU_LEN];
	GetCurrentDirectory(PDU_LEN, buf);
	send(tSoc, buf, PDU_LEN, 0);
}

void RunErr(SOCKET tSoc)
{
	send(tSoc, S_Err, PDU_LEN, 0);
}
int Run(SOCKET tSoc, string *cmd){
	if (cmd[0] == "get" || cmd[0] == "GET")
		RunGet(tSoc, cmd[1]);
	else if (cmd[0] == "put" || cmd[0] == "PUT")  //put file to   para 2
		RunPut(tSoc, cmd[2]);
	else if (cmd[0] == "cd" || cmd[0] == "CD")    //get file from para 1
		RunCd(tSoc,cmd[1]);
	else if (cmd[0] == "ls" || cmd[0] == "LS")
		RunLs(tSoc);
	else if (cmd[0] == "pwd" || cmd[0] == "PWD")
		RunPwd(tSoc);
	else if(cmd[0]=="quit" || cmd[0]=="QUIT")
		return 0;
	else
		RunErr(tSoc);
	return 1;
}

int main()
{
	string cmd[5];
	char token[PDU_LEN];
	int sum,tmp,i;
	int err;
	int lenAddr=sizeof(SOCKADDR);

	WORD wSoc_Ver;
	WSADATA wSoc_Data;
	
	SOCKET srvSoc;       //Socket item of Server
	SOCKADDR_IN srvAddr; //Addr of Server
	SOCKADDR_IN cliAddr; //Addr of Server

	//Init
	wSoc_Ver = MAKEWORD(1, 1);
	if(err = WSAStartup(wSoc_Ver, &wSoc_Data))
		{
		cout<<"Can't init Win32 Sockets\n";
		system("pause");
		return -1;
		}

	//Check Version=1.1?
	if (LOBYTE(wSoc_Data.wVersion) != 1 || HIBYTE( wSoc_Data.wVersion ) != 1 )
		{
		WSACleanup();
		cout<<"Invalid Winsock version!\n";
		system("pause");
		return -1;
		}

	//Create a Socker for Server
	if(((srvSoc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) ) == INVALID_SOCKET))
		{
		WSACleanup();
		cout<<"socket failed!\n";
		system("pause");
		return -1;
		}

	//Set Addr , bind to Socket
	srvAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	srvAddr.sin_family = AF_INET;
	srvAddr.sin_port = htons(PORT);
	if(bind(srvSoc, (SOCKADDR*)&srvAddr, sizeof(SOCKADDR))==SOCKET_ERROR)
		{
		cout<<"bind failed code:"<< WSAGetLastError()<<endl;
		closesocket(srvSoc);
		WSACleanup();
		system("pause");
		return -1;
		}

	//Listen
	if(listen(srvSoc, 5)==SOCKET_ERROR)
		{
		cout<<"listen failed code:"<< WSAGetLastError()<<endl;
		closesocket(srvSoc);
		WSACleanup();
		system("pause");
		return -1;
		}
	//wait for Client's connect
	cout<<"Srv All Green\n";
	while(1)
		{
		//Get A Connect
		SOCKET tSoc;
		if( (tSoc= accept(srvSoc, (SOCKADDR*)&cliAddr, &lenAddr))==INVALID_SOCKET)
			{
			cout<<"accept failed code:"<< WSAGetLastError()<<endl;
			closesocket(srvSoc);
			WSACleanup();
			system("pause");
			return -1;
			}
		cout<<"Accept client:"<<inet_ntoa(cliAddr.sin_addr)<<":"<< ntohs(cliAddr.sin_port)<<endl;
		stringstream stream;
		//Say Hello
		//send(tSoc, S_Wel.c_str(),S_Wel.length()+1,0);
		while(1)
			{
			//get an string from Client
			for(sum=0;sum<PDU_LEN;sum+=tmp)
				{
					tmp = recv(tSoc, token + sum, PDU_LEN - sum, 0);
					cout<<"Try:"<<token<<endl;
					if(tmp==SOCKET_ERROR)
						{
						cout<<"recv failed!\n";
						system("pause");
						return -1;
						}
				}
			// format
			istringstream sin(token);
			for(i=0;sin >> cmd[i++];);
			//Run
			if(!Run(tSoc, cmd)) break;
			}
		cout<<"Abandon Client.\n";
		closesocket(tSoc);
		}
	closesocket(srvSoc);
	WSACleanup();
}
	
