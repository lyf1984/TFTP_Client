#include"head.h"
int main() {
	char file_path[128];
	char buffer[BUFFER_SIZE];
	//初始化socket
	WSADATA wsaData;
	int Result = WSAStartup(0x0101, &wsaData);
	if (Result)
	{
		printf("WSAStartup failed with error: %d", Result);
		return 0;
	}
	//创建套接字
	SOCKET tftpsock;
	tftpsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (tftpsock == INVALID_SOCKET) {
		printf("创建套接字失败\n");
		return 0;
	}
	//服务端 ip和端口
	sockaddr_in server_addr;
	char serverip[20] = "10.12.181.168";
	int serverport = 69;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(serverport);
	server_addr.sin_addr.S_un.S_addr = inet_addr(serverip);
	//客户端ip和端口
	sockaddr_in client_addr;
	char clientip[20] = "10.12.181.87";
	int clientport = 0;
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(clientport);
	client_addr.sin_addr.S_un.S_addr = inet_addr(clientip);
	unsigned long Opt = 1;
	ioctlsocket(tftpsock, FIONBIO, &Opt);
	//绑定客户端ip和端口
	Result = bind(tftpsock, (LPSOCKADDR)&client_addr, sizeof(client_addr));
	if (Result == SOCKET_ERROR)
	{
		// 绑定失败
		printf("Client socket bind error!\n");
		return 0;
	}
	//if (scanf("%s", file_path) == NULL) {
//	printf("读取文件名失败");
//	return 0;
//}

	upload(1, "test.txt", buffer, tftpsock, server_addr, sizeof(sockaddr_in));
	
	
}




