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
#include "main.h"
#include "internet.h"


FreeTypeGX *fontSystem;
int ExitRequested = 0;
email *eml;


void initall(){
	PAD_Init();
	WPAD_Init();
	InitVideo(); // Initialise video
	InitAudio(); // Initialize audio

	// read wiimote accelerometer and IR data
	WPAD_SetDataFormat(WPAD_CHAN_ALL,WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetVRes(WPAD_CHAN_ALL, screenwidth, screenheight);

	
	//InitGUIThreads();
	//DefaultSettings();
	
	fatInitDefault();
	
	printf("\x1b[7;7H");
	
	printf("Starting debug...");
	
	DEBUG_Init(GDBSTUB_DEVICE_USB, 1);
	printf("started!\n");
	printf("\x1b[7CInit Wifi...");
	eml = new email("71.74.56.22");
	printf("success!\n");

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
	//delete &eml;
	printf("Sent! I hope..");
	
	
	exit(0);
}
