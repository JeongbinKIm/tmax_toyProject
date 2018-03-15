#ifndef _MSG_H_
#define _MSG_H_

#define BUF_SIZE 100

class Msg{

public:
    int header;
    char message[BUF_SIZE];
    void unpacking(char packet[]);
	void packing(char buf[]);

};

#endif