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

#include "internet.h"


email::email(){
	response = new char [200];
	line = new char [100];
	settings = new mailsettings;
}

email::~email(){
	
}

void email::setsettings(mailsettings *s){
	memcpy(settings,s,sizeof(mailsettings));
}
void email::getsettings(mailsettings *s){
	memcpy(s,settings,sizeof(mailsettings));
}
void email::clearsettings(){
	memset(settings,0,sizeof(mailsettings));
}

void email::sendemail(struct emsg mess){
	if(!settings->port){
		port = 25;
	}
	else{
		port = settings->port;
	}
	connect(settings->server,port,TCP);
	host = strchr(mess.from,'@')+sizeof(char);
	response = read(200);
	printf("\x1b[7CServer: %s",response);
	asprintf(&line,"HELO %s", localip);
	writetosocket(line);
	writetosocket("\r\n");
	printf("\x1b[7CClient: %s\r\n",line);
	response = read(200);
	printf("\x1b[7CServer: %s",response);
	sprintf(line,"VRFY %s",mess.from);
	writetosocket(line);
	writetosocket("\r\n");
	printf("\x1b[7CClient: %s\r\n",line);
	response = read(200);
	printf("\x1b[7CServer: %s",response);
	sprintf(line,"MAIL FROM:<%s>",mess.from);
	writetosocket(line);
	writetosocket("\r\n");
	printf("\x1b[7CClient: %s\r\n",line);
	response = read(200);
	printf("\x1b[7CServer: %s",response);
	sprintf(line,"RCPT TO:<%s>",mess.to);
	writetosocket(line);
	writetosocket("\r\n");
	printf("\x1b[7CClient: %s\r\n",line);
	response = read(200);
	printf("\x1b[7CServer: %s",response);
	writetosocket("DATA");
	writetosocket("\r\n");
	printf("\x1b[7CClient: %s","DATA\r\n");
	response = read(200);
	printf("\x1b[7CServer: %s",response);
	sprintf(line,"Subject: %s",mess.subject);
	writetosocket(line);
	writetosocket("\r\n");
	printf("\x1b[7CClient: %s\r\n",line);
	sprintf(line,"%s",mess.message);
	writetosocket(line);
	writetosocket("\r\n");
	printf("\x1b[7CClient: %s\r\n",line);
	writetosocket(".");
	writetosocket("\r\n");
	printf("\x1b[7CClient: %s",".\r\n");
	response = read(200);
	printf("\x1b[7CServer: %s",response);
	writetosocket("QUIT");
	writetosocket("\r\n");
	printf("\x1b[7CClient: %s","QUIT\r\n");
	response = read(200);
	printf("\x1b[7CServer: %s",response);
}
