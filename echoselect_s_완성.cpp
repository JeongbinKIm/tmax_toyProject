#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/select.h>
#include <iostream>

using namespace std;

#define BUF_SIZE 100

int main(int argc, char *argv[])
{
	int serv_sock;                              // 서버의 소켓 생성
    int clnt_sock;                              // 클라이언트의 소켓 생성
	
    struct sockaddr_in serv_adr;                // 서버의 주소 표현을 위한 구조체
    struct sockaddr_in clnt_adr;                // 클라이언트의 주소 표현을 위한 구조체

	struct timeval timeout;                     // 타임아웃을 위한 구조체

	fd_set reads;                               // 파일 디스크럽터 저장 공간
    fd_set cpy_reads;                           // 파일 디스크럽터 복사 공간

	socklen_t adr_sz;                           // 클라이언트의 주소 저장 공간에 대한 사이즈

	int fd_max;                                 // 파일 디스크럽트의 수
    int str_len;                                // 클라이언트가 전송한 메세지의 바이트 크기
    int fd_num;                                 // 생성된 파일 디스크럽트 넘버
    int i;                                      // for 문

	char buf[BUF_SIZE];                         // 전송된 메세지의 저장소

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
	
	// 연결요청을 위한 대기상태로 진입
	// 사용될 소켓의 파일디스크럽터에, 연결요청 대기 큐의 크기정보 전달
	if(listen(serv_sock, 5) == -1)
    {
        cout << "listen() error !" << endl;
    }

	// 파일 디스크럽터 셋 정보
	FD_ZERO(&reads);					// 인자로 전달된 주소의 에프디셋 변수의 모든 비트를 0으로 초기화
	FD_SET(serv_sock, &reads);			// 디스크럽터 정보 등록
	fd_max=serv_sock;					// 등록된 디스크럽터는 오직 1개 ==> max는 1.

	while(1)
	{
		// fd_set 정보가 바뀌므로, 기존의 fd_set 정보 복사
		cpy_reads = reads;

		// 타임아웃 정보 지정
		timeout.tv_sec = 5;
		timeout.tv_usec = 5000;

		// 검사 대상이 되는 파일 디스크럽터 수
		// 수신 데이터 존재 여부
		// 데이터 전송 가능 여부
		// 예외 상황 발생 여부
		// 타임아웃 설정 인자
		if((fd_num = select(fd_max+1, &cpy_reads, 0, 0, &timeout)) == -1)
		{
			break;
		}

		// 타임 아웃에 의한 반환 시, select는 0을 반환
		if(fd_num==0)
		{
			continue;
		}

		// select 함수가 반환 성공 했을 때, 실행.
		for(i = 0; i < fd_max + 1; i++)
		{
			// 다음 함수를 호출하면서, 상태변화(수신 데이터가 있는 소켓)가 있었던 파일 디스크럽터를 검색
			// i 인덱스에, 상태변화가 있는가? (복사된 fd_set 안에서)			
			if(FD_ISSET(i, &cpy_reads))
			{
				// 상태변화가 감지되었다면, 서버의 소켓부터 먼저 확인. ; 확인되었으면, 연결요청의 수락 과정 진행.
				if(i == serv_sock)	// connection request!
				{
					adr_sz = sizeof(clnt_adr);													// 저장 공간의 크기 전달 (클라이언트)
					
					// 서버 소켓의 파일 디스크럽터 전달,
					// 연결 요청한 클라이언트의 주소정보를 담을 변수의 주소 값 전달,
					// 두 번째 매개변수에 전달된 주소의 변수 크기를 바이트 단위로 전달,
					// 반환 값은, 생성된 소켓(클라이언트)의 파일 디스크럽트. 실패 시, -1.
					clnt_sock = accept(serv_sock, (struct sockaddr *) &clnt_adr, &adr_sz);

					// 생성(연결)된 클라이언트 소켓의 파일 디스크럽트의 정보를 fd_set 변수에 저장.
					FD_SET(clnt_sock, &reads);

					// 총 디스크럽트 개수 업데이트
					if(fd_max < clnt_sock)
					{
						fd_max = clnt_sock;
					}

					cout << "대화 참여 중인 client 수: " << clnt_sock << endl;
				}

				// 상태 변화가 발생한 소켓이 서버 소켓이 아닌 경우 ==> 수신할 데이터가 있는 경우 실행 ==> 문자열 vs 연결종료(EOF), 인지 구분 필요.
				else	// read message!
				{
					// i 파일 디스크럽트에 있는 메세지를 buf로 읽는다
					// 수신한 데이터의 바이트 수 반환 및 저장
					str_len = read(i, buf, BUF_SIZE);

					// 수신한 데이터가 EOF라면, 소켓을 종료
					if(str_len == 0)    // close request!
					{
						// fd_set 변수 기반, 해당 정보 삭제
						FD_CLR(i, &reads);
						close(i);
						cout << "대화 참여 중인 client 수: " << i << endl;
					}

					else
					{
						// 변화가 발생한 파일 디스크럽터(클라이언트)에 메시지 재전달
						for(int j = serv_sock; j < fd_max + 1; j++)
						{
							if(j != serv_sock)
							{
								write(j, buf, str_len);
							}
						}

						// write(i, buf, str_len);    // echo!
					}
				}
			}
		}
	}

	close(serv_sock);
	
	return 0;
}