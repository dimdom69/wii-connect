
/*
 *      http.h
 *      
 *      Copyright 2009 Aaron Lehrian <jsmaster@tampabay.rr.com>
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */


#include <stdlib.h>
#include <network.h>

enum net_protocol{
	UDP,
	TCP
};

using namespace std;


	
	
class internet{
	public:
		internet();
		~internet();
		void connect(char *server, int port,net_protocol protocol);
		int getnetstate();
		const char *getstate();
		void writetosocket(const char *write);
		char *readfromsocket(int bufsize = 1025);
		char *read(int bufsize = 1025);
		char *getipbyname(char *domain);
		char *address_to_ip(char *address_in);
	protected:
		char localip[16];
		char gateway[16];
		char netmask[16];
		s32 netstate;
	
	private:
		int offset;
		u32 ipaddr;
		int red;
		char *buff;
		s32 socket;
		hostent *hostip;
		char *host;
	
};

class http : public internet{

public:
http();
~http();
char *gethttpfile(const char *file,char *host = NULL,int bufsize = 1025);
protected:

private:
struct httpresponse{
  float version;
  int response_code;
  char *text;
  char *date;
  char *modified;
  char *server;
  size_t content_length;
  char *content_type;
  char *charset;
 
};
char *httpget;
}; 


class chat : public internet{
public:
	chat();
	~chat();
	void chat_keepalive();
	void chat_connect();
	void chat_disconnect();
	void chat_sendmessage(char *message,int dest);
	char *chat_getmessages();
protected:

private:
	char *mesbuf;
	char *status;
};
typedef struct mailsettings_s{
	char server[50];
	int port;
	
}mailsettings;
typedef struct messlist_s{
	char *subject;
	char *from;
	char *to;
	char *cc;
	char *date;
	char *data;
	int newmail;
	int important;
	int starred;
}messlist;

typedef struct mlist_s{
	int amount;
	messlist *ml;
}mlist;

struct emsg{
	char from[50];
	char to[50];
	char subject[50];
	char message[200];
	
};	
	
	

class email : public internet{
	

public:
	
	email();
	~email();
	void sendemail(struct emsg mess);
	mlist *getallmail(); //must have set settings first; gets new mail + saved mail
	void setsettings(mailsettings *s);
	void getsettings(mailsettings *s);
	void clearsettings();
	

private:
	char *response;
	char *line;
	char *host;
	mailsettings *settings;
	int port;
	
	
	
	
	
};


/*
class network : public internet{
public:
	network();
	~network();
	void connect(char *a,u32 b = NULL, int c = 80, int d = 1);	
	char *gethttpfile(char *a,char *h = NULL,int b = 1025, int c = 1);
	const char *getstate();
	void writetosocket( char *a,int b = 1);
	char *readfromsocket(int a = 1025,int b = 1);
	char *file;
	char *write;
	int bufsize;
	int snum;
	char *ipc;
	u32 ip;
	int port;
	char *host;
	s32 netstate;
	char *redfromsocket;
	const char *gotnetstate;
	char *gothttpfile;
	http *net;
	int all;
protected:
	
private:

};
*/
