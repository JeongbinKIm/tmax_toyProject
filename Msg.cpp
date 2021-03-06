#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include "Msg.h"
#include <queue>

using namespace std;

#define BUF_SIZE 100
#define NAME_SIZE 20
#define HEADER_SIZE 4

Msg::Msg()
{
    header = 0;
}

Msg::~Msg()
{
}

//packet의 내용을 클래스 내부의 queue에 넣는 함수
void Msg::enQueue(char packet[], int strlen)
{
    for (int i = 0; i < strlen; i++)
    {
        unfinished.push(packet[i]);
    }
    memset(message, 0, sizeof(message));
}

//헤더를 분리하는 함수
int Msg::seperateHeader()
{

    if (header == 0)
    {
        if (unfinished.size() > sizeof(header))
        {
            //header가 0이고 queue에 header크기 이상의 char 가 있을때
            char temp[4];
            for (int i = 0; i < 4; i++)
            {
                temp[i] = unfinished.front();
                unfinished.pop();
            }
            memcpy(&header, temp, 4);
            return 1;
        }
        else
        {
            //header가 0인데 아직 header크기만큼 char를 못 받은 경우
            return 0;
        }
    }
    else
    {
        //header가 0이 아닐때
        return -1;
    }
}

//메세지가 헤더의 길이 이상만큼 받았는지 확이하고 메세지를 완성시키는 함수
int Msg::unpacking()
{
    memset(message, 0, sizeof(message));
    if (header <= unfinished.size())
    {
        for (int i = 0; i < header; i++)
        {
            message[i] = unfinished.front();
            unfinished.pop();
        }
        return 1;
    }
    else
    {
        return 0;
    }
}

//클래스 내부의 message배열에 헤더와 문자열을 담는 함수
void Msg::packing(char buf[])
{
    char *target = message + sizeof(header);
    char *source = buf;
    memcpy(message, &header, sizeof(header));
    strcpy(target, source);
}

//매개변수로 입력받은 배열에 클래의 헤더와 message의 내용을 담는 함수
void Msg::packet(char *packet)
{
    memcpy(packet, &header, sizeof(header));
    strcpy(packet + sizeof(header), message);
}
