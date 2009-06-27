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
http *inet;


void initall(){
	PAD_Init();
	WPAD_Init();
	InitVideo(); // Initialise video
	InitAudio(); // Initialize audio

	// read wiimote accelerometer and IR data
	WPAD_SetDataFormat(WPAD_CHAN_ALL,WPAD_FMT_BTNS_ACC_IR);
	WPAD_SetVRes(WPAD_CHAN_ALL, screenwidth, screenheight);

	
	InitGUIThreads();
	
	fatInitDefault();
	
	printf("\x1b[7;7H");
	
	printf("Starting debug...");
	
	DEBUG_Init(GDBSTUB_DEVICE_USB, 1);
	printf("started!\n");
	printf("\x1b[7CInit Wifi...");
	eml = new email();
//	inet = new http();
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
	//printf("Press A to send the email\nPress B to get the index of example.com.\n");
	
	
	//MainMenu(MAIN_SCREEN);
	
	//struct emsg *msg;
	
	MainMenu(MENU_EMAIL);
	
	/*while(1){
		VIDEO_WaitVSync();
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		if(pressed & WPAD_BUTTON_A){
			struct emsg msg;
			strcpy(msg.from,"jsmaster@tampabay.rr.com");
			strcpy(msg.to,"jsmaster@tampabay.rr.com");
			strcpy(msg.subject,"Hello world!");
			strcpy(msg.message,"I'd like to thank the acadamy...");
			mailsettings settings;
			strcpy(settings.server,"71.74.56.22");
			eml->clearsettings();
			eml->setsettings(&settings);
			eml->sendemail(*msg);*/
			//delete eml;
			//printf("Sent! I hope..\n");
		/*}
		if(pressed & WPAD_BUTTON_B){
			inet->connect("208.77.188.166",80,TCP);
			printf("%s",inet->gethttpfile("index.html","example.com",5000));
		}
	}*/
	
	while(1){
		WPAD_ScanPads();
		u32 pressed = WPAD_ButtonsDown(0);
		if ( pressed & WPAD_BUTTON_HOME ) exit(0);
		
	}
	
	exit(0);
}
