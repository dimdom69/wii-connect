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

#include "http.h"


email::email(char *ip,int port){
	connect(ip,NULL,port,3);
	response = new char [100];
	line = new char [100];
}

email::~email(){
	
}

void email::sendemail(struct emsg mess){
	host = strchr(mess.from,'@')+sizeof(char);
	response = readfromsocket(99,3);
	asprintf(&line,"HELO %s", host);
	writetosocket(line,3);
	response = readfromsocket(99,3);
	sprintf(line,"MAIL FROM:<%s>",mess.from);
	writetosocket(line,3);
	response = readfromsocket(99,3);
	sprintf(line,"RCPT TO:<%s>",mess.from);
	response = readfromsocket(99,3);
	writetosocket("DATA",3);
	response = readfromsocket(99,3);
	sprintf(line,mess.message);
	writetosocket(line,3);
	writetosocket("\n.\n",3);
	response = readfromsocket(99,3);
	writetosocket("QUIT",3);
	response = readfromsocket(99,3);
}
