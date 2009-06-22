/*#include <errno.h>
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
#include <wiiuse/wpad.h>

#include "http.h"

network *netw;
http *net;

static lwp_t net_t = LWP_THREAD_NULL;


network::network(){
	netstate=10;
}
network::~network(){
	delete net;
}
void network::connect(char *a,u32 b, int c, int d){
	netstate = 1;
	ipc = a;
	ip = b;
	port = c;
	snum = d;
	LWP_ResumeThread(net_t);
}
char *network::gethttpfile(char *a,char *h,int b, int c){
	netstate = 2;
	file = a;
	bufsize = b;
	snum = c;
	LWP_ResumeThread(net_t);
	while(netstate != 0);
	return gothttpfile;
}
const char *network::getstate(){
	netstate = 3;
	LWP_ResumeThread(net_t);
	while(netstate != 0);
	return gotnetstate;
}
void network::writetosocket(char *a,int b){
	netstate = 4;
	write = a;
	snum = b;
	LWP_ResumeThread(net_t);
}
char *network::readfromsocket(int a,int b){
	netstate = 5;
	bufsize = a;
	snum = b;
	LWP_ResumeThread(net_t);
	while(netstate != 0);
	return redfromsocket;
}


static void *netthread(void *arg){
	netw = new network();
	while(1){
			switch(netw->netstate){
				case 10:
					net = new http();
					while(net->netstate != 0) net = new http();
					netw->netstate = 0;
					LWP_SuspendThread(net_t);
					break;
				case 1:
					netw->net->connect(netw->ipc,netw->ip,netw->port,netw->snum);
					netw->netstate = 0;
					LWP_SuspendThread(net_t);
					break;
				case 2:
					netw->gothttpfile = netw->net->gethttpfile(netw->file,netw->host,netw->bufsize,netw->snum);
					netw->netstate = 0;
					LWP_SuspendThread(net_t);
					break;
				case 3:
					netw->gotnetstate = netw->net->getstate();
					netw->netstate = 0;
					LWP_SuspendThread(net_t);
					break;
				case 4:
					netw->net->writetosocket(netw->write,netw->snum);
					netw->netstate = 0;
					LWP_SuspendThread(net_t);
					break;
				case 5:
					netw->redfromsocket = netw->net->readfromsocket(netw->bufsize,netw->snum);
					netw->netstate = 0;
					LWP_SuspendThread(net_t);
					break;
			}
		}
	}
*/
