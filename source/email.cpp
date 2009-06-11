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
	response = new char [1000];
	line = new char [1000];
}

email::~email(){
	
}

void email::sendemail(struct emsg mess){
	host = strchr(mess.from,'@')+sizeof(char);
	response = readfromsocket(99,3);
	printf("\x1b[7CServer: %s\n",response);
	asprintf(&line,"HELO %s", localip);
	writetosocket(line,3);
	writetosocket("\r\n",3);
	printf("\x1b[7CClient: %s\r\n",line);
	response = readfromsocket(99,3);
	printf("\x1b[7CServer: %s\n",response);
	sprintf(line,"VRFY %s",mess.from);
	writetosocket(line,3);
	writetosocket("\r\n",3);
	printf("\x1b[7CClient: %s\r\n",line);
	response = readfromsocket(99,3);
	printf("\x1b[7CServer: %s\n",response);
	sprintf(line,"MAIL FROM:<%s>",mess.from);
	writetosocket(line,3);
	writetosocket("\r\n",3);
	printf("\x1b[7CClient: %s\r\n",line);
	response = readfromsocket(99,3);
	printf("\x1b[7CServer: %s\n",response);
	sprintf(line,"RCPT TO:<%s>",mess.to);
	writetosocket(line,3);
	writetosocket("\r\n",3);
	printf("\x1b[7CClient: %s\r\n",line);
	response = readfromsocket(99,3);
	printf("\x1b[7CServer: %s\n",response);
	writetosocket("DATA",3);
	writetosocket("\r\n",3);
	printf("\x1b[7CClient: %s","DATA\r\n");
	response = readfromsocket(99,3);
	printf("\x1b[7CServer: %s\n",response);
	sprintf(line,"Subject: %s",mess.subject);
	writetosocket(line,3);
	writetosocket("\r\n",3);
	printf("\x1b[7CClient: %s\r\n",line);
	sprintf(line,"%s",mess.message);
	writetosocket(line,3);
	writetosocket("\r\n",3);
	printf("\x1b[7CClient: %s\r\n",line);
	writetosocket(".",3);
	writetosocket("\r\n",3);
	printf("\x1b[7CClient: %s",".\r\n");
	response = readfromsocket(99,3);
	printf("\x1b[7CServer: %s\n",response);
	writetosocket("QUIT",3);
	writetosocket("\r\n",3);
	printf("\x1b[7CClient: %s","QUIT\r\n");
	response = readfromsocket(99,3);
	printf("\x1b[7CServer: %s\n",response);
}
