#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <iostream>

using namespace std;

#define BUF_SIZE 1024

int main(int argc, char *argv[])
{
	int sock;							// 클라이언트 소켓 인덱스
	char message[BUF_SIZE];				// 메세지 저장 공간
	int str_len;						// 에코된 메세지의 길이 정보
	struct sockaddr_in serv_adr;		// 소켓에 할당된 주소 정보

	// 주소 및 포트 정보 미입력 에러
	if(argc != 3)
	{
		cout << "Usage: " << argv[0] << " <IP> <port>" << endl;
		exit(1);
	}
	
	// 소켓 생성
	sock = socket(PF_INET, SOCK_STREAM, 0);

	// 소켓 생성 실패시 에러
	if(sock==-1)
	{
		cout << "socket() error" << endl;
	}

	// 주소 정보 초기화
	memset(&serv_adr, 0, sizeof(serv_adr));
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_adr.sin_port = htons(atoi(argv[2]));
	
	// 서버의 대기 큐에 등록(연결)

	// 실패한 경우
	if(connect(sock, (struct sockaddr*)&serv_adr, sizeof(serv_adr)) == -1)
	{
		cout << "connect() error" << endl;
		exit(1);
	}

	// 성공한 경우
	else
	{
		puts("Connected...........");
	}
	
	// 메세지 전달 및 에코 루프
	while(1) 
	{
		// 서버로 전달할 메세지 입력
		fputs("Input message(Q to quit): ", stdout);
		fgets(message, BUF_SIZE, stdin);
		
		// 종료를 뜻하는 메세지
		if(!strcmp(message,"q\n") || !strcmp(message,"Q\n"))
		{
			break;
		}

		// 입력된 메세지를 서버로 전달
		write(sock, message, strlen(message));

		// 서버에서 재전달된 메세지 읽기
		// 메세지의 길이 정보 저장
		str_len=read(sock, message, BUF_SIZE-1);
		message[str_len]=0;
		
		// 전달된 메세지 출력
		cout << "Message from server: " << message;
	}
	
	// 소켓 닫기
	close(sock);

	return 0;
}