#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <iostream>
#include "Msg.h"
#include <queue>

using namespace std;




Msg::Msg(){
 header=0;

    
};


void Msg::enQueue(char packet[],int strlen){

    for (int i=0; i<strlen; i++){
        unfinished.push(packet[i]);
    }
}

int Msg::seperateHeader(){

    if (header==0){
        if(unfinished.size()>sizeof(header)){  
            //header가 0이고 queue에 header크기 이상의 char 가 있을때
        
            char temp[4];
            for(int i=0; i<4; i++){
                temp[i]=unfinished.front();
                unfinished.pop();
            }
            memcpy(&header,temp,4);
            return 1;
        }else{
            //header가 0인데 아직 header크기만큼 char를 못 받은 경우
            return 0;
        }
    }else{
        //header가 0이 아닐때
        return -1;
    }
}


int Msg::unpacking(){
    
    if(header<=unfinished.size()){
        for (int i = 0; i<header; i++){
        message[i]=unfinished.front();
        unfinished.pop();
        }
        return 1;    
    }else{
        return 0;
    }    
}
    
 
void Msg::packing(char buf[]){
		
	int header=0;
    for (int i=0; i<sizeof(buf);i++)
    {   
         if(buf[i]==0){
             break;
         }
        header++;
    }

	char* target = message+sizeof(header);
	char* source = buf;
	memcpy(message,&header,sizeof(header));
	strcpy(target,source);	
}
