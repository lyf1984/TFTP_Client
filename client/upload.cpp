#include"head.h"
extern FILE* log_file;
extern time_t t;
extern clock_t start, end;
void upload(int mode, const char* filename, char* buffer, SOCKET sock, sockaddr_in addr, int addrlen) {
	int send_bytes = 0;//记录总传输大小
	sockaddr_in serveraddr = { 0 };
	int max_send = 0;//超时重传次数
	int result;//记录返回值
	int data_size;//每次成功从文件读取的字节数
	int block_num = 0;//传送序号
	char data[DATA_SIZE];//读取文件数据
	char recv_buffer[BUFFER_SIZE];//保存接收数据
	BOOL end_flag = FALSE;//传输是否完成
	BOOL start_flag = TRUE;//传输是否开始
	FILE* fp;
	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("打开文件失败\n");
		return;
	}
	write_request(mode, filename, buffer, sock, addr, addrlen);
	//start = clock();
	while (1) {
		result = receive_ACK(recv_buffer, sock, serveraddr, addrlen);
		//收到有效ACK包
		if (result == 1) {
			max_send = 0;//重置重传次数
			//结束传输
			if (end_flag) {
				printf("传输完毕\n");
				return;
			}
			//发送ACK序号的下一个序号
			block_num = 0;
			block_num += recv_buffer[2];
			block_num = (block_num << 8) + recv_buffer[3];
			memset(data, 0, DATA_SIZE);
			data_size = fread(data, 1, DATA_SIZE, fp);
			fprintf(log_file, "从%s文件读取数据size:%dbytes %s", filename,data_size, asctime(localtime(&(t = time(NULL)))));
			//记录开始传输时间
			if (start_flag) {
				start = clock();
				start_flag = FALSE;
			}
			result = send_data(sock, serveraddr, addrlen, fp, buffer, data, data_size, ++block_num);
			send_bytes += result;
			if (data_size < 512 && result != -1) {
				//记录结束传输时间
				end = clock();
				printf("speed:%.1fkb/s", send_bytes / (1024*(double)(end - start) / CLK_TCK));
				end_flag = TRUE;//传输完毕
			}
		}
		//超时或发送失败重传
		else if (result == -1) {
			max_send++;
			if (max_send > MAX_RETRANSMISSION) {
				printf("重传失败\n");
				fprintf(log_file, "ERROR:重传次数过多 %s", asctime(localtime(&(t = time(NULL)))));
				return;
			}
			if (block_num > 0) {
				fprintf(log_file, "重传数据包 数据包序号:%d %s", block_num, asctime(localtime(&(t = time(NULL)))));
				send_data(sock, serveraddr, addrlen, fp, buffer, data, data_size, block_num);
			}
			else {
				fprintf(log_file, "重传写请求 %s", asctime(localtime(&(t = time(NULL)))));
				write_request(mode, filename, buffer, sock, addr, addrlen);
			}
		}
		//收到错误包
		else {
			return;
		}
	}
}
//发送写请求
int write_request(int mode, const char* filename, char* buffer, SOCKET sock, sockaddr_in addr, int addrlen) {
	int send_size = 0;//请求包数据大小
	int result;//记录返回值
	memset(buffer, 0, sizeof(buffer));//初始化报文
	buffer[++send_size] = WRQ;//operation code头部
	send_size++;
	memcpy(buffer + send_size, filename, sizeof(filename));//写入文件名
	send_size += sizeof(filename);
	buffer[send_size++] = 0;
	//ascii码
	if (mode == NETASCII) {
		strcpy(buffer + send_size, "netascii");
		send_size += 9;
	}
	//二进制流
	else
	{
		strcpy(buffer + send_size, "octet");
		send_size += 6;
	}
	result = sendto(sock, buffer, send_size, 0, (struct sockaddr*)&addr, addrlen);
	if (result == SOCKET_ERROR) {
		printf("发送写请求失败\n");
		fprintf(log_file, "ERROR:发送写请求失败 错误码:%d %s", WSAGetLastError(), asctime(localtime(&(t = time(NULL)))));
	}
	else {
		fprintf(log_file, "发送写请求成功send%dbytes %s", result,asctime(localtime(&(t = time(NULL)))));
	}
	return result;
}
//接收ACK包
int receive_ACK(char* recv_buffer, SOCKET sock, sockaddr_in& addr, int addrlen) {
	memset(recv_buffer, 0, sizeof(recv_buffer));
	struct timeval tv;
	fd_set readfds;
	int result;
	int wait_time;
	for (wait_time = 0; wait_time < TIME_OUT; wait_time++) {
		FD_ZERO(&readfds);
		FD_SET(sock, &readfds);
		tv.tv_sec = 2;
		tv.tv_usec = 0;
		select(sock + 1, &readfds, NULL, NULL, &tv);
		result = recvfrom(sock, recv_buffer, 4, 0, (struct sockaddr*)&addr, (int*)&addrlen);
		if (result > 0 && result < 4) {
			printf("bad packet\n");
			fprintf(log_file, "ERROR:接收包不正确 %s", asctime(localtime(&(t = time(NULL)))));
			return 0;
		}
		else if (result >= 4) {
			if (recv_buffer[1] == ERROR_CODE) {
				printf("ERROR!\n");
				fprintf(log_file, "ERROR:接收到错误包 错误码:%d 错误信息%s %s", recv_buffer[3], recv_buffer+4,asctime(localtime(&(t = time(NULL)))));
				return -2;
			}
			fprintf(log_file, "接收ACK包成功receive%dbytes  ACK序号:%d %s", result,recv_buffer[3]+(recv_buffer[2]<<8),asctime(localtime(&(t = time(NULL)))));
			return 1;
		}
	}
	if (wait_time >= TIME_OUT) {
		printf("接收等待超时\n");
		fprintf(log_file, "ERROR:等待接收超时 %s", asctime(localtime(&(t = time(NULL)))));
		return -1;
	}
}
//发送文件数据
int send_data(SOCKET sock, sockaddr_in addr, int addrlen, FILE* fp, char* buffer, char* data, int data_size, unsigned short block_num) {
	int result;
	int send_size = 0;
	memset(buffer, 0, sizeof(buffer));
	buffer[++send_size] = DATA;
	buffer[++send_size] = (char)(block_num >> 8);
	buffer[++send_size] = (char)block_num;
	send_size++;
	memcpy(buffer + send_size, data, data_size);
	send_size += data_size;
	buffer[send_size] = 0;
	result = sendto(sock, buffer, send_size, 0, (struct sockaddr*)&addr, addrlen);
	if (result == SOCKET_ERROR) {
		printf("发送数据失败\n");
		fprintf(log_file, "ERROR:发送数据失败 数据包序号:%d 错误码:%d %s", block_num,WSAGetLastError(),asctime(localtime(&(t = time(NULL)))));
		return -1;
	}
	else {
		fprintf(log_file, "发送数据包成功send%dbytes 数据包序号:%d %s", result,block_num, asctime(localtime(&(t = time(NULL)))));
		return result;
	}
}