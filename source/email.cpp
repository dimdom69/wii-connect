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
	printf("Server: %s",response);
	asprintf(&line,"HELO %s\r\n", localip);
	writetosocket(line,3);
	printf("Client: %s",line);
	response = readfromsocket(99,3);
	printf("Server: %s",response);
	sprintf(line,"MAIL FROM:<%s>\r\n",mess.from);
	writetosocket(line,3);
	printf("Client: %s",line);
	response = readfromsocket(99,3);
	printf("Server: %s",response);
	sprintf(line,"RCPT TO:<%s>\r\n",mess.from);
	response = readfromsocket(99,3);
	printf("Server: %s",response);
	writetosocket("DATA\r\n",3);
	printf("Client: %s","DATA\r\n");
	response = readfromsocket(99,3);
	printf("Server: %s",response);
	sprintf(line,"Subject: %s\r\n",mess.subject);
	writetosocket(line,3);
	printf("Client: %s",line);
	sprintf(line,mess.message);
	writetosocket(line,3);
	printf("Client: %s",line);
	writetosocket("\r\n.\r\n",3);
	printf("Client: %s",".\r\n");
	response = readfromsocket(99,3);
	printf("Server: %s",response);
	writetosocket("QUIT\r\n",3);
	printf("Client: %s","QUIT\r\n");
	response = readfromsocket(99,3);
	printf("Server: %s",response);
}
