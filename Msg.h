#ifndef _MSG_H_
#define _MSG_H_

#include <queue>
using namespace std;
#define BUF_SIZE 100
#define NAME_SIZE 20
#define HEADER_SIZE 4

class Msg
{

public:
  int header;                                       //들어온 메세지의 길이를 담는 변수
  char message[BUF_SIZE + NAME_SIZE + HEADER_SIZE]; //완성된 메세지를 담는 배열
  queue<char> unfinished;                           //완성되지 않은 메세지를 담는 queue
  Msg();
  ~Msg();
  int unpacking();                         //메세지가 완성되어 있는지 판단하고 완성되면 message배열에 담는 함수
  void packing(char buf[]);                //헤더와 buf의 내용을 클래스 내부의 message배열에 담는 함수
  void packet(char *packet);               //packet에 클래스 내부의 헤더와 message를 연결시켜 담는 함수
  int seperateHeader();                    //header를 분리하는 함수
  void enQueue(char packet[], int strlen); //클래스 내부의 queue에 packet의 내욜을 넣는 함수
};

#endif
