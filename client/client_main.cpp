#include"head.h"
FILE* log_file;//日志文件
char log[512];//写入日志数据
time_t t;//保存时间
int main() {
	//初始化日志文件
	log_file = fopen("log.txt", "w+");
	printf("%s", asctime(localtime(&(t=time(NULL)))));
	if (log_file == NULL) {
		printf("创建日志文件失败！\n");
		return 0;
	}
	char file_path[128];//文件名
	char buffer[BUFFER_SIZE];//保存发送的数据
	//初始化socket
	WSADATA wsaData;
	int Result = WSAStartup(0x0101, &wsaData);
	if (Result)
	{
		printf("WSAStartup failed with error: %d", Result);
		return 0;
	}
	//创建套接字
	SOCKET client_sock;
	client_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (client_sock == INVALID_SOCKET) {
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
	//设置为非阻塞模式
	unsigned long Opt = 1;
	ioctlsocket(client_sock, FIONBIO, &Opt);
	//绑定客户端ip和端口
	Result = bind(client_sock, (LPSOCKADDR)&client_addr, sizeof(client_addr));
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

	upload(1, "test.txt", buffer, client_sock, server_addr, sizeof(sockaddr_in));
	
	
}




