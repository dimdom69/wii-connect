#include <errno.h>
#include <fat.h>
#include <math.h>
#include <debug.h>
#include <network.h>
#include <ogc/lwp_watchdog.h>
#include <ogcsys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <sys/dir.h>
#include <unistd.h>
#include <wiiuse/wpad.h>
#include "http.h"

#define MAX_SOCKETS 99

http::http(){
	netstate = 1;
	while (net_init() == -EAGAIN);
	if(if_config(localip,netmask,gateway,true)) netstate = -1;
	else netstate = 0;
}

http::~http(){
	for(int x = 0;x<MAX_SOCKETS;x++){
		net_close(socket[x]);
	}
	delete [] buff;
}

void http::connect(const char *ipc,u32 ip, int port, int snum){
	netstate = 2;
	if(snum > MAX_SOCKETS){
		netstate = -20;
		return;
	}
	if(ipc != NULL){
		ip = inet_addr(ipc);
	}

	struct sockaddr_in connect_addr;
	socket[snum] = net_socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
    if (socket[snum] < 0){
    	netstate = -21;
    	return;
	}
	memset(&connect_addr, 0, sizeof(connect_addr));
	connect_addr.sin_family = AF_INET;
	connect_addr.sin_port = port;
	connect_addr.sin_addr.s_addr=ip;
	
	if (net_connect(socket[snum], (struct sockaddr*)&connect_addr, sizeof(connect_addr)) == -1) {
		net_close(socket[snum]);
		netstate = -22;
		return;
	}
	netstate = 0;
}

void http::writetosocket(const char *write,const int snum){
	netstate = 3;
	if(net_write(socket[snum],write,strlen(write))){
		netstate = -30;
		return;
	}
	netstate = 0;
}

char *http::readfromsocket(int bufsize,const int snum){
	netstate = 4;
	if(buff != NULL){
		delete [] buff;
		buff = NULL;
	}
	buff = new char [bufsize+1];
	if(buff == NULL){
		netstate = -10;
		return NULL;
	}
	offset = 0;
	while (red = net_read(socket[snum],buff + offset, bufsize - offset)){
		if(red<0){
			netstate = -40;
			delete [] buff;
			buff = NULL;
			break;
		}
		buff[offset+red] = '\0';
		offset+=red;
		if(red == 0) break;
	}
	return buff;
}
/*
char *http::read(int bufsize, int snum){
	netstate = 4;
	if(buff != NULL){
		delete [] buff;
	}
	buff = new char [bufsize+1];
	if(buff == NULL){
		netstate = -10;
		return NULL;
	}
	red = net_read(socket[snum],buff, bufsize);
	return buff;
}
*/


char *http::gethttpfile(const char *file,int bufsize,int snum){
	netstate = 5;
	httpget = new char [strlen("GET / HTTP/1.0\r\n\r\n")+strlen(file)+1];
	if(httpget == NULL){
		netstate = -10;
		return NULL;
	}
	asprintf(&httpget,"GET /%s HTTP/1.0\r\n\r\n",file);
	writetosocket(httpget,snum);
	delete [] httpget;
	netstate = 0;
	return readfromsocket(bufsize,snum);
}

const char *http::getstate(){
	switch(netstate){
		case 0:
		return "Ready.";
		case 1:
		return "Initializing network...";
		case -1:
		return "Failed to init network.";
		case -10:
		return "Out of memory.";
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
