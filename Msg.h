#ifndef _MSG_H_
#define _MSG_H_

#include <queue>
using namespace std;
#define BUF_SIZE 100
#define NAME_SIZE 20
#define HEADER_SIZE 4

class Msg
{

private:
public:
  int header;
  char message[BUF_SIZE + NAME_SIZE + HEADER_SIZE];
  queue<char> unfinished;
  Msg();
  ~Msg();
  int unpacking();
  void packing(char buf[]);
  int seperateHeader();
  void enQueue(char packet[], int strlen);
  void packet(char *packet);
};

#endif
