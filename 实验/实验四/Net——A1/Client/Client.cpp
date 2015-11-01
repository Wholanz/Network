#include "Client.h" 
using namespace std;
void Rec(SOCKET tSoc,char buffer[])//get a string of PDU_LEN
{
	int sum,i;
	for(sum=0;sum<PDU_LEN;sum+=i)
		{
		i = recv(tSoc, buffer + sum, PDU_LEN - sum, 0);
		if (i ==SOCKET_ERROR)
			{
			cout<<"recv failed!\n";
			system("pause");
			return ;
			}
		}
}

void RunGet(SOCKET tSoc, string cmd){
	ofstream fout(cmd, ios::binary);
	int num=0,sum,i;
	char buffer[PDU_LEN];
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
		fout.write(buffer, num);
		if (num < PDU_LEN) break;
	}
	fout.close();
}

void RunPut(SOCKET tSoc, string cmd){
	ifstream fin(cmd, ios::binary);
	char buffer[PDU_LEN];
	int num = PDU_LEN;
	while (true){
		fin.read(buffer, PDU_LEN);
		num = (int)fin.gcount();
		send(tSoc, (char *)&num, sizeof(int), 0);
		send(tSoc, buffer,num, 0);
		if (num < PDU_LEN) break;
	}
}
void RunCd(SOCKET tSoc, string s) {}
void RunLs(SOCKET tSoc){
	int num=0,sum,i;
	char buffer[PDU_LEN+1];
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
		buffer[num] = '\0';
		cout << buffer;
		if (num < PDU_LEN) break;
	}
}
void RunPwd(SOCKET tSoc){
	char buffer[PDU_LEN];
	Rec(tSoc,buffer);
	cout << buffer << endl;
}
void RunErr() {}
int Run(SOCKET tSoc, string *cmd){
	if (cmd[0] == "quit" || cmd[0] == "QUIT")
		return 0;
	char buffer[PDU_LEN];
	Rec(tSoc,buffer);
	cout<<buffer;
	if (cmd[0] == "get" || cmd[0] == "GET")
		RunGet(tSoc, cmd[2]);
	else if (cmd[0] == "put" || cmd[0] == "PUT") //put file from  para 1
		RunPut(tSoc, cmd[1]);
	else if (cmd[0] == "cd" || cmd[0] == "CD")   //get fike to    para 2
		RunCd(tSoc,cmd[1]); 
	else if (cmd[0] == "ls" || cmd[1] == "LS")
		RunLs(tSoc);
	else if (cmd[0] == "pwd" || cmd[0] == "PWD")
		RunPwd(tSoc);
	else 
		RunErr();
	return 1;
}


int main(){
		string ip,cmd[5],token;
		int err,i; 

		WORD wSoc_Ver;
		WSADATA wSoc_Data;
	
		SOCKET cliSoc;       //Socket item of Client
		SOCKADDR_IN srvAddr; //Addr of Server

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
		//Create Socket
		if(((cliSoc = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP) ) == INVALID_SOCKET))
				{
				WSACleanup();
				cout<<"socket failed!\n";
				system("pause");
				return -1;
				}
		//get ip.set addr
		while (true)
			{
			cout << "Please input the FTPServer IP: ";
			getline(cin, ip);
			srvAddr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
			srvAddr.sin_family = AF_INET;
			srvAddr.sin_port = htons(PORT);
			if(!connect(cliSoc, (SOCKADDR*)&srvAddr, sizeof(SOCKADDR))) break;
			cout << "log error\n";
			}
		cout << "log success\n";
		//Run 
		while (true)
			{
			//get string
			cout << "ftp>";
			getline(cin, token);
			//send to server
			send(cliSoc, token.c_str(), PDU_LEN, 0);
			//format
			istringstream sin(token);
			for(i=0;sin >> cmd[i++];);
			//run
			if(!Run(cliSoc, cmd)) break;
			}
		system("pause");
		closesocket(cliSoc);
		WSACleanup();
}
