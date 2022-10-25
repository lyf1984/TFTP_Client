#include<stdio.h>
#include<string.h>
#include<WinSock2.h>
#include<Windows.h>
#pragma comment(lib,"Ws2_32.lib")
//报文类型
#define RRQ 1
#define WRQ 2
#define DATA 3
#define ACK 4
#define ERROR_CODE 5
#define NETASCII 1
#define OCTET 2
//错误类型
#define NOT_DEFINED 0
#define NOT_FOUND 1
#define ACCESS_VIOLATION 2
#define DISK_FULL 3
#define ILLEGAL_TFTP_OPEREATION 4
#define UNKNOWN_TRANSFER_ID 5
#define FILE_ALREADY_EXISTS 6
#define NO_SUCH_USER 7
//数据大小
#define BUFFER_SIZE 1024
#define DATA_SIZE 512
#define PKT_MAX_RXMT 3
#define PKT_RCV_TIMEOUT 3*1000
//函数声明
int write_request(int mode, const char* filename, char* buffer, SOCKET serversock, sockaddr_in addr, int addrLen);
int receive(char* buffer, SOCKET serversock, sockaddr_in &addr, int addrlen);
void upload(int mode, const char* filename, char* buffer, SOCKET serversock, sockaddr_in addr, int addrlen);
int send_data(SOCKET serversock, sockaddr_in addr, int addrlen, FILE* fp, char* buffer, char* data, int data_size, unsigned short block_num);