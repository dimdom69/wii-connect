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
	newmail = new mlist;
	allmail = new mlist;
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
	if(!smtpsettings->port){
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

int email::renderpopresponse(const char *resp){
	error = new char [3];
	strncpy(error,resp,3);
	if(strcmp(error,"+OK")){
		return 0;
	}
	nummessages = (int)resp[4];
	return nummessages;
}

mlist *email::getnewmail(){
	if(!popsettings->port){
		popport = 110;
	}
	else{
		popport = popsettings->port;
	}
	connect(popsettings->server,popport,TCP);
	response = read(200);
	sprintf(line,"USER %s\r\n",popsettings->user);
	writetosocket(line);
	response = read(200);
	sprintf(line,"PASS %s\r\n",popsettings->password);
	writetosocket(line);
	response = read(200);
	writetosocket("STAT");
	response = read(200);
	rendered = renderpopresponse(response);
	messsize = new int [rendered];
	response = read(200);
	for(int x = 0;x<rendered;x++){
		response = read(200);
		messsize[x] = (int)(strchr(response,' ')+1);
	}
	response = read(200);
	if(strcmp(response,".\r\n")){
		//ERROR
	}
	newmail->amount = rendered;
	newmail->ml = new messlist [rendered];
	for(int x = 0;x<rendered;x++){
		mailbuffer = new char [messsize[x]];
		sprintf(line,"RETR %d",x+1);
		writetosocket(line);
		mailbuffer = read(messsize[x]);
		parsemessage(newmail->ml,mailbuffer);
	}/*
	for(int x = 0;x<rendered;x++){
		sprintf(line,"DELE %d",x+1)
		writetosocket(line);
		response = read(200);
	}*/
	writetosocket("QUIT");
	response = read(200);
}
