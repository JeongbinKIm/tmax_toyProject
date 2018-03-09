#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <iostream>
	
using namespace std;

#define BUF_SIZE 100
#define NAME_SIZE 20
	
void * send_msg(void * arg);
void * recv_msg(void * arg);
	
char name[NAME_SIZE] = "[DEFAULT]";
char msg[BUF_SIZE];
	
int main(int argc, char *argv[])
{
    // 소켓 생성 및 주소 정보 선언
	int sock;
	struct sockaddr_in serv_addr;
	
    // 멀티쓰레드 기반 구현을 위한 변수
    pthread_t snd_thread, rcv_thread;
	void * thread_return;
	
    // 접속 정보 에러
    if(argc != 4)
    {
        cout << "Usage: " << argv[0] << " <IP> <port> <name>" <<endl;
		
        exit(1);
	}
	
    // 클라이언트 이름 저장
	sprintf(name, "[%s]", argv[3]);
	
    // 소켓 생성
    sock = socket(PF_INET, SOCK_STREAM, 0);
	
    // 주소 정보 초기화
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(atoi(argv[2]));
	  
    // 서버로 연결 요청
	if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == -1)
    {
        // 에러시 connect 함수는 -1 반환
        cout << "connect() error" << endl;

        exit(1);
    }

    // 쓰레드 생성
	pthread_create(&snd_thread, NULL, send_msg, (void*)&sock);  // write 쓰레드
	pthread_create(&rcv_thread, NULL, recv_msg, (void*)&sock);  // read 쓰레드

    // 쓰레드 종료
	pthread_join(snd_thread, &thread_return);
	pthread_join(rcv_thread, &thread_return);
	
    // 소켓 닫기
    close(sock);  
	
    return 0;
}
	
// send thread main
void * send_msg(void * arg)
{
    // 서버 소켓
	int sock = *((int*)arg);
	
    // 전달할 메세지를 담는 변수
    char name_msg[NAME_SIZE+BUF_SIZE];
	
    while(1) 
	{
        // 전달 (순수) 메세지 입력
        fgets(msg, BUF_SIZE, stdin);
		
        // 종료 기준
        if(!strcmp(msg,"q\n")||!strcmp(msg,"Q\n")) 
		{
			close(sock);
			exit(0);
		}
		
        // 이름 및 메세지 통합 저장
        sprintf(name_msg,"%s %s", name, msg);
		
        // 서버로 메세지 전달
        write(sock, name_msg, strlen(name_msg));
	}

	return NULL;
}

// read thread main
void * recv_msg(void * arg)
{
    // 서버 소켓
	int sock = *((int*)arg);
	
    // 전달할 메시지를 담는 변수
    char name_msg[NAME_SIZE+BUF_SIZE];
	
    // 읽을 메세지 길이 정보
    int str_len;
	
    while(1)
	{
        // 메세지 리딩
		str_len = read(sock, name_msg, NAME_SIZE+BUF_SIZE-1);
		
        // 종료 기준
        if(str_len == -1) 
        {
            return (void*)-1;
        }

        name_msg[str_len]=0;
		
        // 전달받은 메세지 출력
        fputs(name_msg, stdout);
	}
    
	return NULL;
}