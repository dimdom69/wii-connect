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
#include <wiiuse/wpad.h>


#include "FreeTypeGX.h"
#include "video.h"
#include "audio.h"
#include "menu.h"
#include "input.h"
#include "filelist.h"
#include "demo.h"
#include "http.h"
//#include "net.h"


/*network *netw;*/
//static lwp_t net_t = LWP_THREAD_NULL;
FreeTypeGX *fontSystem;
struct SSettings Settings;
int ExitRequested = 0;
email *eml;

//http *net;

/*
static void *netthread(void *arg){
	netw = new network();
	while(1){
			switch(netw->netstate){
				case 1:
					netw->net->connect(netw->ipc,netw->ip,netw->port,netw->snum);
					netw->netstate = 0;
					LWP_SuspendThread(net_t);
					break;
				case 2:
					netw->gothttpfile = netw->net->gethttpfile(netw->file,netw->bufsize,netw->snum);
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
	
network::network(){
	all = 0;
	net = new http();
//	while(net->netstate != 0) net = new http();
	all = 1;
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
char *network::gethttpfile(char *a,int b, int c){
	netstate = 2;
	file = a;
	bufsize = b;
	snum = c;
	LWP_ResumeThread(net_t);
	while(netstate != 0);
	return gothttpfile;
}
const char *network::getstate(){
	LWP_ResumeThread(net_t);
	netstate = 3;
	while(netstate != 0);
	return gotnetstate;
}
void network::writetosocket(char *a,int b){
	LWP_ResumeThread(net_t);
	netstate = 4;
	write = a;
	snum = b;
}
char *network::readfromsocket(int a,int b){
	LWP_ResumeThread(net_t);
	netstate = 5;
	bufsize = a;
	snum = b;
	while(netstate != 0);
	return redfromsocket;
}

*/

void
DefaultSettings()
{
	Settings.LoadMethod = METHOD_AUTO;
	Settings.SaveMethod = METHOD_AUTO;
	sprintf (Settings.Folder1,"libwiigui/first folder");
	sprintf (Settings.Folder2,"libwiigui/second folder");
	sprintf (Settings.Folder3,"libwiigui/third folder");
	Settings.AutoLoad = 1;
	Settings.AutoSave = 1;
}


void initall(){
	PAD_Init();
	WPAD_Init();
	InitVideo(); // Initialise video
	InitAudio(); // Initialize audio

	// read wiimote accelerometer and IR data
	WPAD_SetDataFormat(WPAD_CHAN_ALL,WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetVRes(WPAD_CHAN_ALL, screenwidth, screenheight);

	// Initialize font system
	fontSystem = new FreeTypeGX();
	fontSystem->loadFont(font_ttf, font_ttf_size, 0);
	fontSystem->setCompatibilityMode(FTGX_COMPATIBILITY_DEFAULT_TEVOP_GX_PASSCLR | FTGX_COMPATIBILITY_DEFAULT_VTXDESC_GX_NONE);

	InitGUIThreads();
	DefaultSettings();
	
	fatInitDefault();
	
	printf("\x1b[7;7H");
	
	printf("Starting debug...");
	
	DEBUG_Init(GDBSTUB_DEVICE_USB, 1);
	printf("started!\n");
	printf("\x1b[7CInit Wifi...");
	//net = new http;
	eml = new email("71.74.56.22");
	printf("success!\n");
	
//	LWP_CreateThread (&net_t,netthread, NULL, NULL, 0, 40);

}

void ExitApp()
{
	ShutoffRumble();
	StopGX();
	exit(0);
}

//---------------------------------------------------------------------------------
int main(int argc, char **argv) {
	
	
	initall();
	
	
	//MainMenu(MAIN_SCREEN);
	
	//printf("Sending...");
	
	struct emsg msg;
	msg.from = "jsmaster@tampabay.rr.com";
	msg.to = "jsmaster@tampabay.rr.com";
	msg.subject = "Hello world!";
	msg.message = "I'd like to thank the acadamy...";
	eml->sendemail(msg);
	delete &eml;
	printf("Sent! I hope..");
	
	
	//eml->connect("140.211.11.130",NULL,80);
	
	//printf("%s",eml->gethttpfile("index.html",50000));
	
	return 0;
}
