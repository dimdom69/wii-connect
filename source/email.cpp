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
	smtpsettings = new mailsettings;
	popsettings = new mailsettings;
	smtpsettings->port = 0;
	popsettings->port = 0;
}

email::~email(){
	
}

void email::setsettings(settype st , mailsettings *s){
	if(st == SMTP) memcpy(smtpsettings,s,sizeof(mailsettings));
	if(st == POP) memcpy(popsettings,s,sizeof(mailsettings));
	
}
void email::getsettings(settype st,mailsettings *s){
	if(st == SMTP) memcpy(s,smtpsettings,sizeof(mailsettings));
	if(st == POP) memcpy(s,popsettings,sizeof(mailsettings));
}
void email::clearsettings(settype st){
	if(st == SMTP) memset(smtpsettings,0,sizeof(mailsettings));
	if(st == POP) memset(popsettings,0,sizeof(mailsettings));
}

void email::sendemail(struct emsg *mess){
	if(smtpsettings->port == 0){
		smtpport = 25;
	}
	else{
		smtpport = smtpsettings->port;
	}
	connect(smtpsettings->server,smtpport,TCP);
	host = strchr(mess->from,'@')+sizeof(char);
	response = read(200);
	sprintf(line,"HELO %s", localip);
	writetosocket(line);
	writetosocket("\r\n");
	response = read(200);
	sprintf(line,"VRFY %s",mess->from);
	writetosocket(line);
	writetosocket("\r\n");
	response = read(200);
	sprintf(line,"MAIL FROM:<%s>",mess->from);
	writetosocket(line);
	writetosocket("\r\n");
	response = read(200);
	sprintf(line,"RCPT TO:<%s>",mess->to);
	writetosocket(line);
	writetosocket("\r\n");
	response = read(200);
	writetosocket("DATA");
	writetosocket("\r\n");
	response = read(200);
	sprintf(line,"Subject: %s",mess->subject);
	writetosocket(line);
	writetosocket("\r\n");
	sprintf(line,"%s",mess->message);
	writetosocket(line);
	writetosocket("\r\n");
	writetosocket(".");
	writetosocket("\r\n");
	response = read(200);
	writetosocket("QUIT");
	writetosocket("\r\n");
	response = read(200);
}

void email::parsemessage(messlist *ml,char *mess){
	numlines = 0;
	messline = mess;
	if(!strncmp(messline,POP_TO,strlen(POP_TO))){
		ml->to = new char [strchr(messline,'\r')-messline];
		strncpy(ml->to,messline,strchr(messline,'\r')-messline);
	}
	else if(!strncmp(messline,POP_FROM,strlen(POP_FROM))){
		ml->from = new char [strchr(messline,'\r')-messline];
		strncpy(ml->from,messline,strchr(messline,'\r')-messline);
	}
	else if(!strncmp(messline,POP_SUBJECT,strlen(POP_SUBJECT))){
		ml->subject = new char [strchr(messline,'\r')-messline];
		strncpy(ml->subject,messline,strchr(messline,'\r')-messline);
	}
	else if(!strncmp(messline,POP_DATE,strlen(POP_DATE))){
		ml->date = new char [strchr(messline,'\r')-messline];
		strncpy(ml->date,messline,strchr(messline,'\r')-messline);
	}
	else if(!strncmp(messline,POP_CC,strlen(POP_CC))){
		ml->cc = new char [strchr(messline,'\r')-messline];
		strncpy(ml->cc,messline,strchr(messline,'\r')-messline);
	}
	for(size_t x = 0;x<strlen(mess);x++){
		if(mess[x] == '\n'){
			numlines++;
			x++;
			messline += x;
			if(!strncmp(messline,POP_TO,strlen(POP_TO))){
				ml->to = new char [strchr(messline,'\r')-messline];
				strncpy(ml->to,messline,strchr(messline,'\r')-messline);
				continue;
			}
			else if(!strncmp(messline,POP_FROM,strlen(POP_FROM))){
				ml->from = new char [strchr(messline,'\r')-messline];
				strncpy(ml->from,messline,strchr(messline,'\r')-messline);
				continue;
			}
			else if(!strncmp(messline,POP_SUBJECT,strlen(POP_SUBJECT))){
				ml->subject = new char [strchr(messline,'\r')-messline];
				strncpy(ml->subject,messline,strchr(messline,'\r')-messline);
				continue;
			}
			else if(!strncmp(messline,POP_DATE,strlen(POP_DATE))){
				ml->date = new char [strchr(messline,'\r')-messline];
				strncpy(ml->date,messline,strchr(messline,'\r')-messline);
				continue;
			}
			else if(!strncmp(messline,POP_CC,strlen(POP_CC))){
				ml->cc = new char [strchr(messline,'\r')-messline];
				strncpy(ml->cc,messline,strchr(messline,'\r')-messline);
				continue;
			}
			else if(!strncmp(messline-2,POP_BODY,strlen(POP_BODY))){
				x+=2;
				messline+=2;
				ml->body = new char [strchr(messline,'\r')-messline];
				strncpy(ml->body,messline,strchr(messline,'\r')-messline);
				continue;
			}
		}
	}
}

void email::getsize(char *response, int *sizes,int numdata){
	for(int x = 0;x<(int)strlen(response);x++){
		if(response[x] == '\n'){
			p = response + x + 1;
			break;
		}
	}
	for(int x = 0;x<numdata;x++){
		p = (strchr(p,' ')+1);
		for(bl = 0;((int)response[bl]) > 47 && ((int)response[bl]) < 58 ;bl++)
		strncpy(b,response,bl);
		sizes[x] = atoi(b);
		printf("%d",sizes[x]);
	}
}

int email::renderpopresponse(const char *resp){
	pos = 0;
	if(strncmp(resp,"+OK",3)){
		return 0;
	}
	pos = (int)(strchr(resp,' ')-resp);
	pos++;
	nummessages = ((int)resp[pos])-48;			//ASCII number offset
	printf("\x1b[7C%d",nummessages);
	return nummessages;
}

messlist *email::getnewmail(){
	if(!popsettings->port){
		popport = 110;
	}
	else{
		popport = popsettings->port;
	}
	connect(popsettings->server,popport,TCP);
	response = read(200);
	printf("\x1b[7CServer: %s",response);
	sprintf(line,"USER %s\r\n",popsettings->user);
	printf("\x1b[7CClient: %s",line);
	writetosocket(line);
	response = read(200);
	printf("\x1b[7CServer: %s",response);
	sprintf(line,"PASS %s\r\n",popsettings->password);
	printf("\x1b[7CClient: %s",line);
	writetosocket(line);
	response = read(200);
	printf("\x1b[7CServer: %s",response);
	printf("\x1b[7CClient: STAT\r\n");
	writetosocket("STAT\r\n");
	response = read(200);
	printf("\x1b[7CServer: %s",response);
	rendered = renderpopresponse(response);
	messsize = new int [rendered];
//	printf("\x1b[7CClient: LIST\r\n");
//	writetosocket("LIST\r\n");
//	response = read(200);
//	printf("\x1b[7CServer: %s",response);
//	while(strcat(response,read(200)));
//	printf("\x1b[7CServer: %s",response);

	/*if(strcmp(response,".\r\n")){
		printf("\x1b[7CError...\n");
	}*/
	
	newmailroot = new messlist [rendered-1];
	newmailroot->next = 0;
	newmail = newmailroot;
	/*for(int x = 0;x<rendered;x++){
		mailbuffer = new char [1000];
		sprintf(line,"RETR %d\r\n",x+1);
		printf("\x1b[7CClient: %s",line);
		writetosocket(line);
		mailbuffer = read(1000);
		printf("\x1b[7CServer: %s",mailbuffer);
		parsemessage(newmail,mailbuffer);
		newmail = newmail->next;
		newmail->next = 0;
	}
	for(int x = 0;x<rendered;x++){
		sprintf(line,"DELE %d",x+1)
		writetosocket(line);
		response = read(200);
	}*/
	writetosocket("RETR 1\r\n");
	printf("\x1b[7CClient: RETR 1\r\n");
	mailbuffer = new char [2000];
	while(strcmp((mailbuffer = read(2000)),".\r\n")){
		printf("\x1b[7CServer: %s",mailbuffer);
	}
	printf("\x1b[7CClient: QUIT");
	writetosocket("QUIT");
	return newmailroot;
}
