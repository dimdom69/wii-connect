#include <errno.h>
#include <debug.h>
#include <fat.h>
#include <math.h>
#include <network.h>
#include <ogc/lwp_watchdog.h>
#include <ogcsys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/dir.h>
#include <unistd.h>


#include "internet.h"

#define MAX_SOCKETS 99



internet::internet(){
	netstate = 1;				
	while (net_init() == -EAGAIN);		//Keep trying to connect
	if(if_config(localip,netmask,gateway,true)) netstate = -1;	//See if it worked
	else netstate = 0;	
	buff = NULL;			//Safety measure (also nessesary for readfromsocket() )		
}

internet::~internet(){
	net_close(socket);
	delete [] buff;
}

void internet::connect(char *server, int port,net_protocol protocol){
	netstate = 2;
	/*if(!(server=address_to_ip(server))){
		netstate = -11;
		return;
	}*/
	ipaddr = inet_addr(server);		//Converts ip into standard u32
	struct sockaddr_in connect_addr;
	socket = net_socket(AF_INET, protocol == TCP ? SOCK_STREAM : SOCK_DGRAM,IPPROTO_IP);  //Create the socket
    if (socket < 0){			//Check for errors
    	netstate = -21;
    	return;
	}
	memset(&connect_addr, 0, sizeof(connect_addr));
	connect_addr.sin_family = AF_INET;
	connect_addr.sin_port = port;
	connect_addr.sin_addr.s_addr=ipaddr;
	
	if (net_connect(socket, (struct sockaddr*)&connect_addr, sizeof(connect_addr)) == -1) {    //Connect to the ip..
		net_close(socket);         //..And check for an error
		netstate = -22;
		return;
	}
	netstate = 0; //All is well
}

void internet::writetosocket(const char *write){
	netstate = 3;
	if(net_write(socket,write,strlen(write))){   //Simple function, checks for an error (returns 0 on success)
		netstate = -30;
		return;
	}
	netstate = 0;
}

char *internet::readfromsocket(int bufsize){
	netstate = 4;
	if(buff != NULL){  //If initialized,
		delete [] buff;//delete the buffer
	}
	buff = new char [bufsize];  //And then initialize it
	if(buff == NULL){   //Check for memory errors
		netstate = -10;
		return NULL;
	}
	offset = 0;    //Variable for telling net_read() where to read() to
	while ((red = net_read(socket,buff+offset, bufsize - 1) > 0)){
		offset+=red;               //Adds the amount read to the offset so that the correct memory address can be found
		buff[offset] = '\0';  //So that the char* ends with a \0
	}
	/*if(red<0){					//Error Checking
		netstate = -40;
		delete [] buff;
		buff = NULL;
		break;
	}*/
	return buff;		//Returns what it read
}



char *internet::read(int bufsize){
	netstate = 4;
	if(buff != NULL){
		delete [] buff;
	}
	buff = new char [bufsize+1];
	if(buff == NULL){
		netstate = -10;
		return NULL;
	}
	red = net_read(socket,buff, bufsize);
	buff[red] = '\0';
	return buff;
}

char* internet::getipbyname(char *domain){
	struct hostent *host = NULL;
	host = net_gethostbyname(domain);
	if(host == NULL) {
		return NULL;
	}
	struct sockaddr_in tmp;
	memcpy(&tmp.sin_addr,host->h_addr_list[0],host->h_length);
	return inet_ntoa(tmp.sin_addr);
};


    // are we dealing with an ip or address
char* internet::address_to_ip(char *address_in)
    {
        for (unsigned int i=0;i<strlen(address_in);i++) {
            char digit = address_in[i];
            if ((digit < 48 || digit > 57) && digit != 46) {
                return getipbyname(address_in);
            }
        }

        return address_in; //already ip
    }


const char *internet::getstate(){
	switch(netstate){
		case 0:
		return "Ready.";
		case 1:
		return "Initializing network...";
		case -1:
		return "Failed to init network.";
		case -10:
		return "Out of memory.";
		case -11:
		return "Not a valid host address";
		case 2:
		return "Connecting...";
		case -20:
		return "Not a valid socket.";
		case -21:
		return "Failed to create socket.";
		case -22:
		return "Failed to connect.";
		case 3:
		return "Sending data...";
		case -30:
		return "Failed to send data.";
		case 4:
		return "Recieving data...";
		case -40:
		return "Failed to recieve data";
		case 5:
		return "Downloading HTTP file...";
		default:
		return "Not a valid netstate.";
		
	}
}
