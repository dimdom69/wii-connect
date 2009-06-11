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
	response = "Error recieving data...";
	host = strchr(mess.from,'@')+sizeof(char);
	response = readfromsocket(999,3);
	printf("\x1b[7CServer: %s\n",response);
	asprintf(&line,"HELO %s\r\n", "153-125.96-97.tampabay.res.rr.com");
	writetosocket(line,3);
	printf("\x1b[7CClient: %s",line);
	response = readfromsocket(999,3);
	printf("\x1b[7CServer: %s\n",response);
	sprintf(line,"VRFY %s\r\n",mess.from);
	writetosocket(line,3);
	printf("\x1b[7CClient: %s",line);
	response = readfromsocket(999,3);
	printf("\x1b[7CServer: %s\n",response);
	sprintf(line,"MAIL FROM:<%s>\r\n",mess.from);
	writetosocket(line,3);
	printf("\x1b[7CClient: %s",line);
	response = readfromsocket(999,3);
	printf("\x1b[7CServer: %s\n",response);
	sprintf(line,"RCPT TO:<%s>\r\n",mess.to);
	writetosocket(line,3);
	printf("\x1b[7CClient: %s",line);
	response = readfromsocket(999,3);
	printf("\x1b[7CServer: %s\n",response);
	writetosocket("DATA\r\n",3);
	printf("\x1b[7CClient: %s","DATA\r\n");
	response = readfromsocket(999,3);
	printf("\x1b[7CServer: %s\n",response);
	sprintf(line,"Subject: %s\r\n",mess.subject);
	writetosocket(line,3);
	printf("\x1b[7CClient: %s",line);
	sprintf(line,"%s\r\n",mess.message);
	writetosocket(line,3);
	printf("\x1b[7CClient: %s",line);
	writetosocket("\r\n.\r\n",3);
	printf("\x1b[7CClient: %s",".\r\n");
	response = readfromsocket(999,3);
	printf("\x1b[7CServer: %s\n",response);
	writetosocket("QUIT\r\n",3);
	printf("\x1b[7CClient: %s","QUIT\r\n");
	response = readfromsocket(999,3);
	printf("\x1b[7CServer: %s\n",response);
}
