#include<iostream>
#include<stdio.h>
#include<winsock2.h>
#include<cstdio>
#include<string.h>
#include<Windows.h>
#include<time.h>
#pragma comment(lib,"ws2_32.lib")

using namespace std;

#define RRQ (short)1
#define WRQ (short)2
#define DATA (short)3
#define ACK (short)4
#define ERROR (short)5
#define OP_LEN 2
#define DATA_LEN 512
#define SERVER_PORT (unsigned short)69
#define FILE_NAME_MAX_SIZE 512
#define BUFFER_SIZE 512

char* socket_ip;
int client_socket;
struct sockaddr_in server_addr;
int addr_len = 4;

struct message_packet
{
	//操作码 opration code
	short OpCode;
	union 
	{
		//文件名称 用于RRQ和WRQ
		char FileName[2];
		//传输模式 transfer mode 用于RRQ和WRQ
		char* TransMode;
		//数据块编号 用于DATA和ACK
		short BlockNum;
		//数据 data
	};

	char Data[DATA_LEN];
};

//void CreateSocket()
//{
//	cout << "Input Server IP ";
//	char ip[20];
//	cin >> ip;
//	cout << "Input Server Port ";
//	int port;
//	cin >> port;
//
//	sockaddr_in addr;
//	addr.sin_family = AF_INET;
//	addr.sin_addr.S_un.S_addr = inet_addr(ip);
//	addr.sin_port = htons(port);
//
//	int sServSock = socket(AF_INET, SOCK_DGRAM, 0);
//	if (sServSock == INVALID_SOCKET)
//	{
//		cout << "Create Socket Failed!";
//	}
//	else
//	{
//		//break;
//	}
//
//	//发送请求
//
//	//Operation Code	1->Read	2->Write
//	short op;
//	cin >> op;
//
//	char buf[20];
//	char filename[20];
//	if (op == READ_REQUEST)
//	{
//		fprintf(fp, "download data:%s\n", filename);
//	}
//	else
//	{
//		fprintf(fp, "upload data:%s\n", filename);
//	}
//
//	memset(buf, 0, sizeof(buf));
//	buf[1] = op;
//	strcpy(buf + OP_LEN, filename);
//	if (type == MODE_OCTET)
//	{
//		strcpy(buf + OP_LEN + sizeof(filename), "octet");
//
//	}
//	else
//	{
//		strcpy(buf + OP_LEN + sizeof(filename), "netascii");
//	}
//
//	//读取数据
//	const int RECV_LOOP_MAX = 10;
//	const int TIMEOUT_SEC = 2;
//	int fd = socket(AF_INET, SOCK_DGRAM, 0);
//
//	char recvBuf[516] = "";
//
//	struct timeval tv;
//	fd_set readfds;
//	int n = 0;
//
//	for (int i = 0; i < RECV_LOOP_MAX; i++)
//	{
//		FD_ZERO(&readfds);
//		FD_SET(fd, &readfds);
//		tv.tv_sec = TIMEOUT_SEC;
//
//		select(fd, &readfds, NULL, NULL, &tv);
//		if (FD_ISSET(fd, &readfds))
//		{
//			//if ((n = recvfrom(fd, recvBuf, sizeof(recvBuf), 0, (struct sockaddr*)addr, ));
//		}
//	}
//}

//上传文件
int SendFile(char* FileName)
{
	message_packet send_packet, recv_packet;
	char buffer[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	strncpy(buffer, FileName, BUFFER_SIZE > sizeof(FileName) ? BUFFER_SIZE : sizeof(FileName));

	//发送请求报文
	send_packet.OpCode = WRQ;

	char snd_packet[sizeof(send_packet)];
	memcpy(snd_packet, &send_packet, sizeof(send_packet));
	sendto(client_socket, snd_packet, sizeof(struct message_packet), 0,
		(struct sockaddr*)&server_addr, sizeof(server_addr));
	while (true)
	{
		int size = recvfrom(client_socket, (char*)&recv_packet, sizeof(message_packet), 0, (struct sockaddr*)&server_addr, &addr_len);
		if (size != -1)
		{
			cout << "size:" << size << endl;
			break;
		}
		else
		{
			cout << "not yet" << endl;
			Sleep(2000);
		}
	}
	return 0;
}

int GetFile(char* FileName)
{
	return 0;
}

int main()
{

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr("10.12.174.1");
	server_addr.sin_port = htons(SERVER_PORT);

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) == 0)
	{
		cout << "WSAStartup succeeded" << endl;
	}
	else
	{
		cout << "WSAStartup failed" << endl;
	}

	client_socket = socket(AF_INET, SOCK_DGRAM, 0);
	if (client_socket < 0)
	{
		cout << "Connect Failed" << endl;
		return 0;
	}

	char FileName[40] = "D:/课程文件/计算机网络/实验/new.txt";
	SendFile(FileName);

	//char file_name[FILE_NAME_MAX_SIZE + 1];
	//memset(file_name, 0, FILE_NAME_MAX_SIZE + 1);

	//printf("Please Input File Name On Server:\t");
	//cin >> file_name;

	//char buffer[BUFFER_SIZE];
	//memset(buffer, 0, BUFFER_SIZE);
	//strncpy_s(buffer, file_name, strlen(file_name) > BUFFER_SIZE ? BUFFER_SIZE : strlen(file_name));
	///* 发送文件名 */
	//if (sendto(client_socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
	//{
	//	cout << "oops!" << endl;
	//	perror("Send File Name Failed:");
	//	exit(1);
	//}
	//closesocket(client_socket);
	return 0;
}