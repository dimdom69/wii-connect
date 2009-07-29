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

#include "b64/encode.h"
#include "internet.h"


email::email(){
	response = new char [512];
	line = new char [100];
	smtpsettings = new mailsettings;
	popsettings = new mailsettings;
	smtpsettings->port = 0;
	popsettings->port = 0;
}

email::~email(){
	
}

void email::loadsettings(char *cfile){
	filepos = 0;
	psettings = new mailsettings;
	ssettings = new mailsettings;
	fp = fopen(cfile,"r+");
	if(!fp) return;
	fseek(fp,0,SEEK_END);
	filesize = ftell(fp);
	fseek(fp,0,SEEK_SET);
	
	buffer = new char [filesize+1];
	buffer[filesize] = '\0';
	
	fread(buffer, filesize, 1, fp);		//read file to buffer
	fclose(fp);					//close file
	
	
	if((tmp = strstr(buffer,"smtpserver"))){
		filepos = (tmp-buffer)+strlen("smtpserver");
		while((int)buffer[filepos] < 33 && buffer[filepos]) filepos++;
		if(buffer[filepos] == '='){
			filepos++;
			while((int)buffer[filepos] < 33 && buffer[filepos]) filepos++;
			tmp = strchr(buffer+filepos,';');
			length = tmp-(buffer+filepos);
			ssettings->server = new char [length+1];
			strncpy(ssettings->server,buffer+filepos,length);
			ssettings->server[length] = '\0';
		}
	}
	if((tmp = strstr(buffer,"popserver"))){
		filepos = (tmp-buffer)+strlen("popserver");
		while((int)buffer[filepos] < 33 && buffer[filepos]) filepos++;
		if(buffer[filepos] == '='){
			filepos++;
			while((int)buffer[filepos] < 33 && buffer[filepos]) filepos++;
			tmp = strchr(buffer+filepos,';');
			length = tmp-(buffer+filepos);
			psettings->server = new char [length+1];
			strncpy(psettings->server,buffer+filepos,length);
			psettings->server[length] = '\0';
		}
	}
	if((tmp = strstr(buffer,"smtpuser"))){
		filepos = (tmp-buffer)+strlen("smtpuser");
		while((int)buffer[filepos] < 33 && buffer[filepos]) filepos++;
		if(buffer[filepos] == '='){
			filepos++;
			while((int)buffer[filepos] < 33 && buffer[filepos]) filepos++;
			tmp = strchr(buffer+filepos,';');
			length = tmp-(buffer+filepos);
			ssettings->user = new char [length+1];
			strncpy(ssettings->user,buffer+filepos,length);
			ssettings->user[length] = '\0';
		}
	}
	if((tmp = strstr(buffer,"popuser"))){
		filepos = (tmp-buffer)+strlen("popuser");
		while((int)buffer[filepos] < 33 && buffer[filepos]) filepos++;
		if(buffer[filepos] == '='){
			filepos++;
			while((int)buffer[filepos] < 33 && buffer[filepos]) filepos++;
			tmp = strchr(buffer+filepos,';');
			length = tmp-(buffer+filepos);
			psettings->user = new char [length+1];
			strncpy(psettings->user,buffer+filepos,length);
			psettings->user[length] = '\0';
		}
	}
	if((tmp = strstr(buffer,"smtppassword"))){
		filepos = (tmp-buffer)+strlen("smtppassword");
		while((int)buffer[filepos] < 33 && buffer[filepos]) filepos++;
		if(buffer[filepos] == '='){
			filepos++;
			while((int)buffer[filepos] < 33 && buffer[filepos]) filepos++;
			tmp = strchr(buffer+filepos,';');
			length = tmp-(buffer+filepos);
			ssettings->password = new char [length+1];
			strncpy(ssettings->password,buffer+filepos,length);
			ssettings->password[length] = '\0';
		}
	}
	if((tmp = strstr(buffer,"poppassword"))){
		filepos = (tmp-buffer)+strlen("poppassword");
		while((int)buffer[filepos] < 33 && buffer[filepos]) filepos++;
		if(buffer[filepos] == '='){
			filepos++;
			while((int)buffer[filepos] < 33 && buffer[filepos]) filepos++;
			tmp = strchr(buffer+filepos,';');
			length = tmp-(buffer+filepos);
			psettings->password = new char [length+1];
			strncpy(psettings->password,buffer+filepos,length);
			psettings->password[length] = '\0';
		}
	}
	if((tmp = strstr(buffer,"smtpport"))){
		filepos = (tmp-buffer)+strlen("smtpport");
		while((int)buffer[filepos] < 33 && buffer[filepos]) filepos++;
		if(buffer[filepos] == '='){
			filepos++;
			while((int)buffer[filepos] < 33 && buffer[filepos]) filepos++;
			tmp = strchr(buffer+filepos,';');
			length = tmp-(buffer+filepos);
			tmp = new char [length+1];
			strncpy(tmp,buffer+filepos,length);
			ssettings->port = atoi(tmp);
			delete [] tmp;
		}
	}
	else{
		ssettings->port = 25;
	}
	
	if((tmp = strstr(buffer,"popport"))){
		filepos = (tmp-buffer)+strlen("popport");
		while((int)buffer[filepos] < 33 && buffer[filepos]) filepos++;
		if(buffer[filepos] == '='){
			filepos++;
			while((int)buffer[filepos] < 33 && buffer[filepos]) filepos++;
			tmp = strchr(buffer+filepos,';');
			length = tmp-(buffer+filepos);
			tmp = new char [length+1];
			strncpy(tmp,buffer+filepos,length);
			psettings->port = atoi(tmp);
			delete [] tmp;
		}
	}
	else{
		psettings->port = 110;
	}
	
	delete [] buffer;
	
	setsettings(POP,psettings);
	setsettings(SMTP,ssettings);
	
	delete psettings;
	delete ssettings;

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
void email::parseesmtp(int v,char *in){
	etmp = new char [strlen(in)+1];
	toLowerCase(etmp,in);
	if((esmp = strstr(etmp,"auth"))){
		if((esmp = strstr(etmp,"auth"))){
			v |= ESMTP_AUTH_LOGIN;
		}
	}
	delete [] etmp;
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
	sprintf(line,"EHLO %s\r\n",localip);
	writetosocket(line);
	response = read(512);
	if(response[0] == '5'){
		sprintf(line,"HELO %s\r\n", localip);
		writetosocket(line);
		response = read(200);
	}
	else{
		parseesmtp(esmtp,response);
		if(esmtp & ESMTP_AUTH_LOGIN){
			user64 = new char [50];
			pass64 = new char [50];
			base64::encoder E;
			E.encode(smtpsettings->user,strlen(smtpsettings->user), user64);
			E.encode(smtpsettings->password,strlen(smtpsettings->password), pass64);
			writetosocket("AUTH LOGIN\r\n");
			response = read(200);
			sprintf(line,"%s\r\n",user64);
			writetosocket(line);
			response = read(200);
			sprintf(line,"%s\r\n",pass64);
			writetosocket(line);
			response = read(200);
		}
	}
	sprintf(line,"MAIL FROM:<%s>\r\n",mess->from);
	writetosocket(line);
	response = read(200);
	sprintf(line,"RCPT TO:<%s>\r\n",mess->to);
	writetosocket(line);
	response = read(200);
	writetosocket("DATA\r\n");
	response = read(200);
	sprintf(line,"From: <%s>\r\n",mess->from);
	writetosocket(line);
	sprintf(line,"To: <%s>\r\n",mess->to);
	writetosocket(line);
	sprintf(line,"Subject: %s\r\n",mess->subject);
	writetosocket(line);
	sprintf(line,"\r\n%s\r\n",mess->message);
	writetosocket(line);
	writetosocket(".\r\n");
	response = read(200);
	writetosocket("QUIT\r\n");
	response = read(200);
}

void email::parsemessage(messlist *ml,char *mess){
	bl = 0;
	numlines = 0;
	if(strncmp(messline,"+OK",strlen("+OK"))){
		//error
	}
	ml->subject = 0;
	ml->body = 0;
	mtemp = new char [strlen(mess)];
	toLowerCase(mtemp,mess);
	if((messline = strstr(mess,"\r\n\r\n"))){
		messline += strlen("\r\n\r\n");
		bl = strstr(messline,"\r\n.\r\n")-messline;
		if(bl == 0){
			bl = (strlen(mess)+mess) - messline;
		}
		ml->body = new char [bl+1];
		ml->body[bl] = '\0';
		strncpy(ml->body,messline,bl+1);
	}
	if((messline = strstr(mtemp,"\r\nsubject: "))){
		messline += strlen("\r\nsubject: ");
		bl = strchr(messline,'\r')-messline;
		ml->subject = new char [bl+1];
		strncpy(ml->subject,messline,bl);
		ml->subject[bl] = '\0';
	}
}

void email::getsize(char *resp, int *sizes,int numdata){
	b = new char [10];
	p = strchr(resp,'\n')+1;
	for(int x = 0;x<numdata;x++){
		p = strchr(p,' ')+1;
		for(bl = 0;p[bl] != '\r';bl++);
		strncpy(b,p,bl);
		sizes[x] = atoi(b);
		p = strchr(p,'\n')+1;
	}
	delete [] b;
}

int email::renderpopresponse(const char *resp){
	pos = 0;
	if(strncmp(resp,"+OK",3)){
		return 0;
	}
	pos = (int)(strchr(resp,' ')-resp);
	pos++;
	mlen = (int)(strchr(resp+pos,' ')-(resp+pos));
	mtmp = new char [mlen];
	strncpy(mtmp,resp+pos,mlen);
	nummessages = atoi(mtmp);			//ASCII number offset
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
	sprintf(line,"USER %s\r\n",popsettings->user);
	writetosocket(line);
	response = read(200);
	sprintf(line,"PASS %s\r\n",popsettings->password);
	writetosocket(line);
	response = read(200);
	writetosocket("STAT\r\n");
	response = read(200);
	rendered = renderpopresponse(response);
	if(rendered == 0){
		writetosocket("QUIT\r\n");	
		response = read(200);
		return 0;
	}
	messsize = new int [rendered];
	list = new char [999];
	writetosocket("LIST\r\n");
	strcpy(list,read(200));
	while(!strstr(list,"\r\n.\r\n")){
		strcat(list,read(200));
	}
	memset(strstr(list,"\r\n.\r\n"),0,5);
	messsize = new int [rendered];
	
	getsize(list, messsize, rendered);

	delete [] list;
	if(rendered){
		newmailroot = new messlist;
		newmail = newmailroot;
		newmail->next = 0;
	}
	for(int x = 0;x<rendered;x++){
		mailbuffer = new char [messsize[x]+30];
		sprintf(line,"RETR %d\r\n",x+1);
		writetosocket(line);	
		strcpy(mailbuffer,read((messsize[x]+30)-strlen(mailbuffer)));
		while(!strstr(mailbuffer,"\r\n.\r\n")){
			strcat(mailbuffer,read((messsize[x]+30)-strlen(mailbuffer)));
		}
		parsemessage(newmail,mailbuffer);
		
		delete [] mailbuffer;
		
		if(x != rendered-1){
			newmail->next = new messlist;
			newmail = newmail->next;
			newmail->next = 0;
		}
		
	}
	//for(int y = 0;y<rendered;y++){
	//	sprintf(line,"DELE %d\r\n",y+1);
	//	writetosocket(line);
	//	response = read(200);
	//}
	writetosocket("QUIT\r\n");
	response = read(200);
	return newmailroot;
}
