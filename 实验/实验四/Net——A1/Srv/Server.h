#include <iostream>
#include <windows.h>
#include <Winsock.h>//version 1.1
#include <string>
#include <sstream>
#include <fstream>

#pragma comment(lib,"ws2_32.lib")

const int PORT=6001;
const int PDU_LEN=256;
const char S_Wel[PDU_LEN]="Welcome to MiniSrv.\n";
const char S_Err[PDU_LEN]="Invalid Order.\n";
const char S_Ls [PDU_LEN]="Run Ls.\n";
const char S_Get[PDU_LEN]="Srv ready to send.\n";
const char S_Put[PDU_LEN]="Srv ready to receive.\n";
const char S_Pwd[PDU_LEN]="Run Pwd.\n";
const char S_Cd [PDU_LEN]="Run Cd.\n";

