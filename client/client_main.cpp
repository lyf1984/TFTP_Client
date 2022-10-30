#include"head.h"
FILE* log_file;//日志文件
time_t t;//保存时间
clock_t start, end;//记录传输时间
int main() {
	//初始化日志文件
	log_file = fopen("log.txt", "w+");
	if (log_file == NULL) {
		printf("创建日志文件失败！\n");
		return 0;
	}
	char filename[128];//文件名
	char buffer[BUFFER_SIZE];//保存发送的数据
	int Result;//保存返回值
	//启动Winsocket
	WSADATA wsaData;
	Result = WSAStartup(0x0101, &wsaData);
	if (Result)
	{
		printf("WSAStartup failed with error: %d", Result);
		fprintf(log_file, "ERROR无法启动Winsocket	错误码:%d	%s", WSAGetLastError(), asctime(localtime(&(t = time(NULL)))));
		return 0;
	}
	fprintf(log_file, "启动Winsocket			%s", asctime(localtime(&(t = time(NULL)))));
	//创建套接字
	SOCKET client_sock;
	client_sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (client_sock == INVALID_SOCKET) {
		printf("创建套接字失败\n");
		fprintf(log_file, "ERROR:无法创建套接字	错误码:%d	%s", WSAGetLastError(), asctime(localtime(&(t = time(NULL)))));
		return 0;
	}
	fprintf(log_file, "创建套接字			%s", asctime(localtime(&(t = time(NULL)))));
	//服务端 ip和端口
	sockaddr_in server_addr;
	char serverip[20] = "10.12.180.43";
	int serverport = 69;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(serverport);
	server_addr.sin_addr.S_un.S_addr = inet_addr(serverip);
	//客户端ip和端口
	sockaddr_in client_addr;
	char clientip[20] = "10.12.181.1";
	int clientport = 0;
	client_addr.sin_family = AF_INET;
	client_addr.sin_port = htons(clientport);
	client_addr.sin_addr.S_un.S_addr = inet_addr(clientip);
	//设置为非阻塞模式
	unsigned long Opt = 1;
	Result = ioctlsocket(client_sock, FIONBIO, &Opt);
	if (Result == SOCKET_ERROR) {
		printf("设置非阻塞模式失败\n");
		fprintf(log_file, "ERROR:无法设置非阻塞模式	错误码:%d	%s", WSAGetLastError(), asctime(localtime(&(t = time(NULL)))));
		return 0;
	}
	//绑定客户端接口
	Result = bind(client_sock, (LPSOCKADDR)&client_addr, sizeof(client_addr));
	if (Result == SOCKET_ERROR)
	{
		// 绑定失败
		printf("Client socket bind error!");
		printf("\n按任意键继续...");
		Result = getch();
		fprintf(log_file, "ERROR:无法绑定接口	错误码:%d	%s", WSAGetLastError(), asctime(localtime(&(t = time(NULL)))));
		return 0;
	}
	//主界面
	char choice;
	char mode;
	while (1) {
		system("cls");
		printf("[TFTP文件传输]\n1、上传文件\n2、下载文件\n3、结束程序\n");
		printf("请选择功能:");
		choice = getch();
		system("cls");
		if (choice == '1') {
			printf("[上传文件]\n");
			printf("请输入文件名:");
			scanf("%s", filename);
			system("cls");
			printf("[上传文件]\n");
			printf("1、netascii\n2、octet\n请选择传输模式:");
			mode = getch();
			printf("\n");
			upload(mode - 48, filename, buffer, client_sock, server_addr, sizeof(sockaddr_in));
		}
		if (choice == '2') {
			printf("[下载文件]\n");
			printf("请输入文件名:");
			scanf("%s", filename);
			system("cls");
			printf("[下载文件]\n");
			printf("1、netascii\n2、octet\n请选择传输模式:");
			mode = getch();
			printf("\n");
			download(mode - 48, filename, buffer, client_sock, server_addr, sizeof(sockaddr_in));
		}
		if (choice == '3')
			return 0;
	}
	fclose(log_file);
}




