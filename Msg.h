#ifndef _MSG_H_
#define _MSG_H_

#define BUF_SIZE 100

class Msg{

public:
    Msg();
    ~Msg();
    int header;
    char message[BUF_SIZE+50];
    queue<char> unfinished;
    int unpacking();
	void packing(char buf[]);
    int seperateHeader();
    void enQueue(char packet[],int strlen);
};

#endif
