
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
#include "Msg.h"

using namespace std;

#define BUF_SIZE 100
#define NAME_SIZE 20
#define HEADER_SIZE 4
#define EPOLL_SIZE 50
void error_handling(char *buf);

void echo(int serv_sock, int fd_max, Msg *msg)
{

	char packet[BUF_SIZE + NAME_SIZE + HEADER_SIZE];

	(*msg).packet(packet);

	for (int j = serv_sock + 2; j < fd_max + 1; j++)
	{
		write(j, packet, (*msg).header + 4);
	}

	memset((*msg).message, 0, sizeof((*msg).message));
	(*msg).header = 0;
}

int main(int argc, char *argv[])
{
	int serv_sock; // 서버의 소켓 생성
	int clnt_sock; // 클라이언트의 소켓 생성

	struct sockaddr_in serv_adr; // 서버의 주소 표현을 위한 구조체
	struct sockaddr_in clnt_adr; // 클라이언트의 주소 표현을 위한 구조체

	socklen_t adr_sz; // 클라이언트의 주소 저장 공간에 대한 사이즈

	int str_len; // 클라이언트가 전송한 메세지의 바이트 크기
	int i;		 // for 문

	fd_set reads;
	int fd_max;

	struct epoll_event *ep_events;
	struct epoll_event event;
	int epfd, event_cnt;

	// 터미널 기반, 오류 메세지 출력 (포트)
	if (argc != 2)
	{
		cout << "Usage: " << argv[0] << " <port>" << endl;
		exit(1);
	}

	// 소켓 초기화
	serv_sock = socket(PF_INET, SOCK_STREAM, 0); // IPv4 인터넷 프로토콜 체계로, 스트림 전송 방식으로, 그냥0프로토콜하나로, 소켓 생성; 반환 값은 디스크럽터

	memset(&serv_adr, 0, sizeof(serv_adr));		  // 서버의 주소를 표현하는 구조체의 모든 멤버를 0으로 초기화
	serv_adr.sin_family = AF_INET;				  // 주소 체계 지정
	serv_adr.sin_addr.s_addr = htonl(INADDR_ANY); // 문자열 기반 IP주소 초기화
	serv_adr.sin_port = htons(atoi(argv[1]));	 // 문자열 기반 PORT번호 초기화

	// 초기화된 주소정보를 소켓에 할당
	// 할당할 소켓의 파일 디스크럽터에, 주소 정보를 지닌 구조체 변수의 주소 값을 지정하고, 구조체 변수의 길이 정보를 인수로 전달
	if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
	{
		cout << "bind() error !" << endl;
	}

	// 연결요청을 위한 대기상태로 진입!
	// 사용될 소켓의 파일디스크럽터에, 연결요청 대기 큐의 크기정보 전달
	if (listen(serv_sock, 5) == -1)
	{
		cout << "listen() error !" << endl;
	}

	epfd = epoll_create(EPOLL_SIZE);
	ep_events = (struct epoll_event *)malloc(sizeof(struct epoll_event) * EPOLL_SIZE);

	event.events = EPOLLIN;
	event.data.fd = serv_sock;
	epoll_ctl(epfd, EPOLL_CTL_ADD, serv_sock, &event);

	FD_ZERO(&reads);
	FD_SET(serv_sock, &reads);
	fd_max = serv_sock;
	Msg *clntMsg[50];
	memset(clntMsg, 0, 50 * sizeof(Msg));
	int numClntSock = 0;
	while (1)
	{

		event_cnt = epoll_wait(epfd, ep_events, EPOLL_SIZE, -1);

		if (event_cnt == -1)
		{
			cout << "epoll_wait() error" << endl;
			break;
		}

		for (i = 0; i < event_cnt; i++)
		{
			int ep_event_fd = ep_events[i].data.fd;

			if (ep_events[i].data.fd == serv_sock)
			{
				adr_sz = sizeof(clnt_adr);
				clnt_sock =
					accept(serv_sock, (struct sockaddr *)&clnt_adr, &adr_sz);
				event.events = EPOLLIN;
				event.data.fd = clnt_sock;
				epoll_ctl(epfd, EPOLL_CTL_ADD, clnt_sock, &event);
				FD_SET(clnt_sock, &reads);
				if (fd_max < clnt_sock)
				{
					fd_max = clnt_sock;
				}
				clntMsg[clnt_sock] = new Msg();
				numClntSock++;
				cout << "연결된 클라이언트 : " << clnt_sock << endl;
			}
			else
			{
				char buf[BUF_SIZE + NAME_SIZE + HEADER_SIZE]; // 전송된 메세지의 저장소

				str_len = read(ep_event_fd, buf, BUF_SIZE + NAME_SIZE + HEADER_SIZE - 1);

				if (str_len == 0)
				{
					FD_CLR(ep_event_fd, &reads);
					epoll_ctl(epfd, EPOLL_CTL_DEL, ep_event_fd, NULL);
					close(ep_event_fd);
					delete clntMsg[ep_event_fd];
					numClntSock--;
					cout << "연결 종료된 클라이언트 : " << ep_event_fd << endl;

					if (numClntSock == 0)
					{
						cout << "접속된 클라이언트가 없습니다. 종료하시겠습니까? (Y|N)" << endl;
						char quit[10];
						fgets(quit, 10, stdin);
						// 종료 기준
						if (!strcmp(quit, "y\n") || !strcmp(quit, "Y\n"))
						{
							for (int i = 0; i < fd_max; i++)
							{
								if (clntMsg[i] != 0)
								{
									delete clntMsg[i];
								}
							}
							close(serv_sock);
							close(epfd);
							exit(0);
						}
						else
						{
							continue;
						}
					}
				}
				else
				{

					Msg msg = *clntMsg[ep_event_fd];
					msg.enQueue(buf, str_len);
					if (msg.seperateHeader() != 1)
					{
						continue;
					}
					if (msg.unpacking() == 1)
					{
						echo(serv_sock, fd_max, &msg);
					}
					else
					{
						continue;
					}
				}
			}
		}
	}
	for (int i = 0; i < fd_max; i++)
	{
		if (clntMsg[i] != 0)
		{
			delete clntMsg[i];
		}
	}
	close(serv_sock);
	close(epfd);
	exit(0);
	return 0;
}
