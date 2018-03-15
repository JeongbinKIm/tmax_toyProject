#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <iostream>
#include <algorithm>
#include <sys/select.h>

using namespace std;

#define BUF_SIZE 100
#define EPOLL_SIZE 50
void error_handling(char *buf);



class Msg{

private:

    int header;
    char message[BUF_SIZE];


public:
    void unpacking(char packet[]);
	void packing(char buf[],int sizeOfBuf);

};

void Msg::unpacking(char packet[]){
        
	memcpy(&header,packet,sizeof(header)); 

    char* target = message;
	char* source = packet+sizeof(header);
	strcpy(target,source);

    }
    

    
void Msg::packing(char buf[],int sizeOfBuf){
		
	header = sizeOfBuf;

	char* target = message+sizeof(header);
	char* source = buf;
	memcpy(message,&header,sizeof(header));
	strcpy(target,source);
	
}






int main(int argc, char *argv[])
{
	int serv_sock;                              // 서버의 소켓 생성
    int clnt_sock;                              // 클라이언트의 소켓 생성
	
    struct sockaddr_in serv_adr;                // 서버의 주소 표현을 위한 구조체
    struct sockaddr_in clnt_adr;                // 클라이언트의 주소 표현을 위한 구조체
	
	socklen_t adr_sz;                           // 클라이언트의 주소 저장 공간에 대한 사이즈

    int str_len;                                // 클라이언트가 전송한 메세지의 바이트 크기
    int i;                                      // for 문

	char buf[BUF_SIZE];                         // 전송된 메세지의 저장소

	fd_set reads;
	int fd_max;

	struct epoll_event *ep_events;
	struct epoll_event event;
	int epfd, event_cnt;


    // 터미널 기반, 오류 메세지 출력 (포트)
	if(argc != 2)
    {
		cout << "Usage: " << argv[0] << " <port>" << endl;

		exit(1);
	}

    // 소켓 초기화
	serv_sock = socket(PF_INET, SOCK_STREAM, 0);    // IPv4 인터넷 프로토콜 체계로, 스트림 전송 방식으로, 그냥0프로토콜하나로, 소켓 생성; 반환 값은 디스크럽터

	memset(&serv_adr, 0, sizeof(serv_adr));			// 서버의 주소를 표현하는 구조체의 모든 멤버를 0으로 초기화
	serv_adr.sin_family = AF_INET;					// 주소 체계 지정
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY);	// 문자열 기반 IP주소 초기화
	serv_adr.sin_port = htons(atoi(argv[1]));		// 문자열 기반 PORT번호 초기화
	
	// 초기화된 주소정보를 소켓에 할당
	// 할당할 소켓의 파일 디스크럽터에, 주소 정보를 지닌 구조체 변수의 주소 값을 지정하고, 구조체 변수의 길이 정보를 인수로 전달
	if(bind(serv_sock, (struct sockaddr*) &serv_adr, sizeof(serv_adr)) == -1)
    {
        cout << "bind() error !" << endl;
    }
	
	// 연결요청을 위한 대기상태로 진입!
	// 사용될 소켓의 파일디스크럽터에, 연결요청 대기 큐의 크기정보 전달
	if(listen(serv_sock, 5) == -1)
    {
        cout << "listen() error !" << endl;
    }

	epfd = epoll_create(EPOLL_SIZE);
	ep_events=(struct epoll_event*)malloc(sizeof(struct epoll_event)*EPOLL_SIZE);

	event.events=EPOLLIN;
	event.data.fd=serv_sock;
	epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

	FD_ZERO(&reads);
	FD_SET(serv_sock,&reads);
	fd_max=serv_sock;

	while(1)
	{
		event_cnt=epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);
		if(event_cnt==-1)
		{
			cout << "epoll_wait() error" << endl;
			break;
		}

		for(i = 0; i < event_cnt ; i++)
		{		
			if(ep_events[i].data.fd==serv_sock)
			{
				adr_sz=sizeof(clnt_adr);
				clnt_sock=
					accept(serv_sock, (struct sockaddr*)&clnt_adr, &adr_sz);
				event.events=EPOLLIN;
				event.data.fd=clnt_sock;
				epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
				FD_SET(clnt_sock,&reads);
				if(fd_max<clnt_sock){ fd_max=clnt_sock; }
				cout << "연결된 클라이언트 : " << clnt_sock << endl;
			}
			else{
				str_len=read(ep_events[i].data.fd, buf, BUF_SIZE);
				if(str_len==0)
				{
					FD_CLR(ep_events[i].data.fd,&reads);
					epoll_ctl(epfd, EPOLL_CTL_DEL, ep_events[i].data.fd, NULL);
					close(ep_events[i].data.fd);
					cout << "연결 종료된 클라이언트 : " << ep_events[i].data.fd << endl;
				}
				else{
					// 변화가 발생한 파일 디스크럽터(클라이언트)에 메시지 재전달
					for(int j = serv_sock+1; j < fd_max + 1; j++)
					{
						write(j, buf, str_len);
					}
				}
			}
		}
	}

	close(serv_sock);
	close(epfd);
	return 0;
}
