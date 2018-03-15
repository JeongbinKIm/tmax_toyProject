#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> 
#include <string.h>
#include <iostream>
#include "Msg.h"


using namespace std;

void Msg::unpacking(char packet[]){
        
    memcpy(&header,packet,sizeof(header)); 

    char* target = message;
	char* source = packet+sizeof(header);
	strcpy(target, source);

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







